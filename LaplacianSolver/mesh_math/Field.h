#pragma once
#ifndef _FIELD_H_
#define _FIELD_H_

#ifdef _DEBUG
	#include <iostream>
#endif // _DEBUG

#include <map>
#include <memory>
#include <limits>
#include <linearAlgebra\matrixTemplate.h>

#include "mesh_geometry.h"

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
	using node_types_list = std::vector<bool>; //true if it is inner point and false if it is a boundary
	using node_labels_list = std::map<uint32_t, field_type>;
	using boundary_entry = std::pair<std::string, node_labels_list>;
	using boundary_list = std::map<std::string, node_labels_list>;
	
	using BoundaryMesh = typename mesh_geom::BoundaryMesh;
	using MeshSharedPtr = std::shared_ptr<mesh_geom>;
private:
	//Keep reference to a space mesh
	MeshSharedPtr m_pMeshGeometry;
	BoundaryMesh m_boundaryMesh;

	data_vector _data; //Field data itself
	node_types_list _node_types; //Types of a field nodes
	boundary_list _boundaries; //A list of all field boundaries
public:
	/**
	 * Creates zero filled field
	 */
	field(const MeshSharedPtr& meshGeometry)
		: 
		m_pMeshGeometry(meshGeometry), 
		_data(geometry_.size(), field_type(0.0)),
		_node_types(geometry_.size(), true)
	{}

	const data_vector& data() const { return _data; }
	data_vector& data() { return _data; }

	/**
	 * Adds new boundary to a field
	 */
	void add_boundary(const std::string& name, const node_labels_list& nodes)
	{
		std::set<uint32_t> labels;
		m_boundaryMesh.addBoundary(name, )
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
	 * Interpolate field value into a given point
	 * It is better when track_label is a clossest point to a {x,y,z}
	 */
	field_type interpolate(double x, double y, double z, uint32_t * track_label = nullptr) const
	{
		uint32_t start_label = track_label ? *track_label : 0;
		//track_info result = closest_plane_interpolation(x, y, z, start_label);
		mesh_geom::InterpCoefs coefs = _geometry.interpCoefs(x, y, z, start_label);

		if (track_label) *track_label = coefs.begin()->first;
		return std::accumulate(coefs.begin(),coefs.end(),0.0,
			[&](field_type val, mesh_geom::InterpCoef c)->field_type
		{
			return val += _data[c.first] * c.second;
		});
	}

};

#endif // !_FIELD_H