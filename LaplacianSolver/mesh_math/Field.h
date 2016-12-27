#pragma once
#ifndef _FIELD_H_
#define _FIELD_H_

#include <map>
#include "mesh_geometry.h"

/**
 *  Type of the field node
 */
enum  NODE_TYPE : uint8_t
{
	INNER_POINT = 0x00,
	BOUNDARY_ZERO_GRADIENT = 0x01,
	BOUNDARY_FIXED_VALUE = 0x02
};

/**
* Field manipulation class
*/
template<typename field_type>
//Field type could be scalar or vector
class field
{
public:
	using mesh_geom = mesh_geometry<double, uint32_t>;
	using data_vector = std::vector<field_type>;
	using node_types_list = std::vector<NODE_TYPE>;
	using node_labels_list = std::set<uint32_t>;
	using boundary_entry = std::pair<std::string, node_labels_list>;
	using boundary_list = std::map<std::string, node_labels_list>;
	using boundary_iterator = typename node_labels_list::const_iterator;

private:
	//Keep reference to a space mesh
	const mesh_geom& _geometry;


	data_vector _data; //Field data itself
	node_types_list _node_types; //Types of a field nodes
	boundary_list _boundaries; //A list of all field boundaries
public:
	/**
	 * Creates zero filled field
	 */
	field(const mesh_geom& geometry_)
		: 
		_geometry(geometry_), 
		_data(geometry_.size(), field_type(0.0)),
		_node_types(geometry_.size(), INNER_POINT)
	{}

	const data_vector& data() const { return _data; }
	data_vector& data() { return _data; }

	/**
	 * Adds new boundary to a field
	 */
	void add_boundary(const std::string& name, const node_labels_list& nodes)
	{
		_boundaries[name] = nodes;
		for (uint32_t l : nodes) _node_types[l] = BOUNDARY_FIXED_VALUE;
	}

	/**
	 * Set values on a boundary
	 */
	void set_boundary_uniform_val(const std::string& name, const field_type& val)
	{
			for (uint32_t l : _boundaries.at(name)) _data[l] = val;
	}

	void set_boundary_vals(const std::string& name, const data_vector& vals)
	{
		node_labels_list ls = _boundaries.at(name);
		if (vals.size() != ls .size())
			throw std::runtime_error("Boundary and input vector sizes mismatch.\n");
		size_t i = 0;
		for (uint32_t l : ls) _data[l] = vals[i++];
	}

	/**
	 * Get values on a boundary
	 */
	data_vector get_boundary_vals(const std::string& name) const
	{
		return data_vector(_boundaries.at(name).begin(), _boundaries.at(name).end());
	}

	/**
	 * Sets a boundary type
	 */
	void set_boundary_type(const std::string& name, NODE_TYPE type)
	{
		for(uint32_t l : _boundaries.at(name))
			switch (_node_types[l])
			{
			case INNER_POINT:
			case BOUNDARY_ZERO_GRADIENT:
				_node_types[l] = type;
				break;
			case BOUNDARY_FIXED_VALUE:
				if(type == INNER_POINT) _node_types[l] = type; //If we want to switch off the boundary
				break;
			default:
				break;
			}
	}

	/**
	 * Names of boundary patches
	 */
	std::vector<std::string> get_boundary_names() const
	{
		std::vector<std::string> names(_boundaries.size());
		size_t i = 0;
		for (const boundary_entry& entry : _boundaries)
			names[i++] = entry.first;
		return names;
	}

	/**
	 * Diffusion of a field using squared distances to a neighbour points
	 * returns new point value
	 */
	field_type diffuse_one_point(uint32_t l1) const
	{
		switch (_node_types[l1])
		{
		case BOUNDARY_FIXED_VALUE: return _data[l1]; //Returns fixed value as it is
		case INNER_POINT:
		{
			double totalSquaredDistance = 0;
			field_type fieldVal = 0;
			auto visitor = [&](uint32_t l2)
			{
				vector3f diff = _geometry.spacePositionOf(l2) - _geometry.spacePositionOf(l1);
				double sqrDist = diff*diff;
				fieldVal += _data[l2] / sqrDist;
				totalSquaredDistance += 1./sqrDist;
			};
			_geometry.visit_neigbour(l1, visitor);
			return fieldVal / totalSquaredDistance;
		}
		case BOUNDARY_ZERO_GRADIENT:
		{
			double totalSquaredDistance = 0;
			field_type fieldVal = 0;
			auto visitor = [&](uint32_t l2)
			{
				vector3f diff = _geometry.spacePositionOf(l2) - _geometry.spacePositionOf(l1);
				double sqrDist = _node_types[l2] == BOUNDARY_ZERO_GRADIENT ? diff*diff : diff*diff / 2.;
				fieldVal += _data[l2] / sqrDist;
				totalSquaredDistance += 1. / sqrDist;
			};
			_geometry.visit_neigbour(l1, visitor);
			return fieldVal / totalSquaredDistance;
		}
		default:
			throw std::runtime_error("Field::diffuse_one_point : Strange node type.");
		}
		
	}

	/**
	 * Returns diffused field
	 */
	field diffuse() const
	{
		field result(*this);
		for (UINT i = 0; i < _data.size(); ++i)
			result._data[i] = diffuse_one_point(i);
		return result;
	}

	/**
	 * Interpolate field value into a given point
	 * It is better when start_label is a clossest point to a {x,y,z}
	 */
	field_type interpolate(double x, double y, double z, uint32_t * track_label = nullptr) const
	{
		uint32_t start_label;
		vector3f pos{ x,y,z };

		if (track_label)
		{
			start_label = _geometry.find_closest(x, y, z, *track_label);
			*track_label = start_label;
		}
		else
		{
			start_label = _geometry.find_closest(x, y, z);
		}

		/*double sqrDist =
			(pos - _geometry.spacePositionOf(start_label))
			*(pos - _geometry.spacePositionOf(start_label));

		if (sqrDist == 0.0) return _data[start_label]; //Check if it is the exact node position
		*/
		double distTotal = 0.;//1. / sqrDist;
		field_type result = 0.;//_data[start_label] / sqrDist;
		auto visitor = [&](uint32_t label)->void
		{
			double dist = math::abs(pos - _geometry.spacePositionOf(label));
			distTotal += 1. / dist;
			result += _data[label] / dist;
		};
		_geometry.visit_neigbour(start_label, visitor);
		return result / distTotal;
	}

};

#endif // !_FIELD_H_