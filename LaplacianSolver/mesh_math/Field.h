#pragma once
#ifndef _FIELD_H_
#define _FIELD_H_

#ifdef _DEBUG
	#include <iostream>
#endif // _DEBUG

#include <map>
#include <limits>
#include <linearAlgebra\matrixTemplate.h>

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
	using track_info = std::tuple<field_type, uint32_t, uint32_t, uint32_t, uint32_t>;

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
		for (uint32_t l : _boundaries.at(name))
			_node_types[l] = type;
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
				double sqrDist = _node_types[l2] == INNER_POINT ? diff*diff / 2 : diff*diff;
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
	* Interpolates values using closest point
	*/
	track_info closest_point_interpolation(double x, double y, double z, uint32_t start) const
	{
		track_info result;
		std::get<1>(result) = _geometry.find_closest(x, y, z, start);
		std::get<0>(result) = _data[std::get<1>(result)];
		return result;
	}
	/**
	* Interpolates values using closest line
	*/
	track_info closest_line_interpolation(double x, double y, double z, uint32_t start) const
	{
		track_info result = closest_point_interpolation(x, y, z, start);
		vector3f dp0 = vector3f{ x,y,z } -_geometry.spacePositionOf(std::get<1>(result));
		if (math::sqr(dp0) != 0.0)
		{
			//Correct result
			std::get<2>(result) = _geometry.find_line(x, y, z, std::get<1>(result));
			vector3f e0 = _geometry.spacePositionOf(std::get<2>(result)) 
				- _geometry.spacePositionOf(std::get<1>(result));
			std::get<0>(result) += (_data[std::get<2>(result)] - std::get<0>(result))
				* (dp0 * e0) / math::sqr(e0);
		}
		return result;
	}
	/**
	 * Interpolates using closest plane
	 */
	track_info closest_plane_interpolation(double x, double y, double z, uint32_t start) const
	{
		track_info result = closest_line_interpolation(x, y, z, start);
		vector3f dp0 = vector3f{ x,y,z } -_geometry.spacePositionOf(std::get<1>(result));
		double norm = std::max(x, std::max(y, z));
		norm *= norm;
		if (math::sqr(dp0) / norm > std::numeric_limits<double>::epsilon() * 100.)
		{
			vector3f e0 = _geometry.spacePositionOf(std::get<2>(result))
				- _geometry.spacePositionOf(std::get<1>(result));
			vector3f dp1 = dp0 - (dp0*e0) *e0 / math::sqr(e0);
			if (math::sqr(dp1) / norm > std::numeric_limits<double>::epsilon() * 100.)
			{
				std::get<3>(result) = _geometry.find_plane(x, y, z,
					std::get<1>(result),
					std::get<2>(result));
				vector3f e1 = _geometry.spacePositionOf(std::get<3>(result))
					- _geometry.spacePositionOf(std::get<1>(result)),
					dp2 = dp1 - (dp1*e1) * e1 / math::sqr(e1),
					vPos = dp0;
				
				//Create plane basis
				vector3f 
					et0 = e0 / math::abs(e0),
					et1 = e1 - (e1*et0)*et0; et1 /= math::abs(et1);
				//Transform all vectors to a new basis
				math::vector_c<double, 2>
					plane_e0{ et0*e0, et1*e0 },
					plane_e1{ et0*e1, et1*e1 },
					plane_vPos{ et0*vPos, et1*vPos };

				//Find line intersection point
				math::matrix_c<double, 2, 2> m2x2Eqs, tm1, tm2;
				m2x2Eqs.column(0) = plane_e1 - plane_e0; m2x2Eqs.column(1) = plane_vPos;
				tm1.column(0) = plane_e1; tm1.column(1) = m2x2Eqs.column(1);
				tm2.column(0) = m2x2Eqs.column(0); tm2.column(1) = plane_e1;

				double fDet = math::det(m2x2Eqs);

				double
					t1 = math::det(tm1)/fDet,
					t2 = math::det(tm2)/fDet;

				double a0 = _data[std::get<1>(result)];
				double a1 = (_data[std::get<2>(result)] - _data[std::get<3>(result)]) * t1;
				std::get<0>(result) = a0 + (a1 - a0) / t2;
			}
		}
		return result;
	}
	/**
	 * Interpolate field value into a given point
	 * It is better when track_label is a clossest point to a {x,y,z}
	 */
	field_type interpolate(double x, double y, double z, uint32_t * track_label = nullptr) const
	{
		uint32_t start_label = track_label ? *track_label : 0;
		track_info result = closest_plane_interpolation(x, y, z, start_label);

#ifdef _DEBUG
		std::cout << "x = " << x << " y = " << y << " z = " 
			<< z << " Pot: " << std::get<0>(result) << " Node: " 
			<< std::get<1>(result) << "\n";
#endif // _DEBUG

		if (track_label) *track_label = std::get<1>(result);
		return std::get<0>(result);
	}

};

#endif // !_FIELD_H