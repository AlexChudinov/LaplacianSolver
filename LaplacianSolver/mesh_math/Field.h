#pragma once
#ifndef _FIELD_H_
#define _FIELD_H_

#include "mesh_geometry.h"

/**
* Field manipulation class
*/
template<typename FieldType>
//Field type could be scalar or vector
class Field
{
	using MeshGeom = MeshGeom<double, uint32_t>;
	using DataVector = std::vector<FieldType>;

	const MeshGeom& _geometry;
	DataVector _data;
public:
	Field(const MeshGeom& geometry_, const DataVector& data_) 
		: _geometry(geometry_), _data(data_)
	{
		if (geometry_.getGraph().size() != _data.size()) throw std::runtime_error("Graph and data sizes mismatch!\n");
	}

	const DataVector& data() const { return _data; }

	/**
	 * Set values on a boundary
	 */
	void setBoundaryValUniform(const char* name, const FieldType& val)
	{
		std::for_each(_geometry.beginOf(name), _geometry.endOf(name), [&](uint32_t l) { _data[l] = val; });
	}
	void setBoundaryVal(const char* name, const DataVector& vals)
	{
		
	}
};

#endif // !_FIELD_H_