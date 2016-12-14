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
	/**
	 * Attachs geometry info to a vectorized data
	 */
	Field(const MeshGeom& geometry_, const DataVector& data_) 
		: _geometry(geometry_), _data(data_)
	{
		if (geometry_.getGraph().size() != _data.size()) throw std::runtime_error("Graph and data sizes mismatch!\n");
	}
	/**
	 * Creates zero filled field
	 */
	Field(const MeshGeom& geometry_)
		: _geometry(geometry_), _data(geometry_.size(), FieldType(0.0))
	{
		;
	}

	const DataVector& data() const { return _data; }

	/**
	 * Iterator for values that belong only one boundary
	 */
	class BoundaryIterator : public MeshGeom::boundary_const_iterator
	{
		using base_iterator = MeshGeom::boundary_const_iterator;
		FieldType* _data;

		BoundaryIterator(base_iterator& it = base_iterator(), FieldType* data_ = nullptr)
			: base_iterator(it), _data(data_)
		{}

		//Friends return iterator to a boundary begin or end
		friend BoundaryIterator Field<FieldType>::boundaryBegin(const std::string& name);
		friend BoundaryIterator Field<FieldType>::boundaryEnd(const std::string& name);
	public:
		FieldType& operator * ()
		{ 
			return _data[*static_cast<const base_iterator>(*this)]; 
		}

		FieldType& operator ->() 
		{ 
			return _data[*static_cast<const base_iterator>(*this)]; 
		}
	};

	class ConstBoundaryIterator : public MeshGeom::boundary_const_iterator
	{
		using base_iterator = MeshGeom::boundary_const_iterator;
		const FieldType* _data;

		ConstBoundaryIterator(base_iterator& it = base_iterator(), const FieldType* data_ = nullptr)
			: base_iterator(it), _data(data_)
		{}

		//Friends return iterator to a boundary begin or end
		friend ConstBoundaryIterator Field<FieldType>::boundaryConstBegin(const std::string& name) const;
		friend ConstBoundaryIterator Field<FieldType>::boundaryConstEnd(const std::string& name) const;

	public:
		const FieldType& operator * () const 
		{ 
			return _data[*static_cast<const base_iterator>(*this)]; 
		}

		const FieldType& operator ->() const 
		{ 
			return _data[*static_cast<const base_iterator>(*this)]; 
		}
	};

	/**
	 * Returns iterators to a boundary end and begin
	 */
	BoundaryIterator boundaryBegin(const std::string& name)
	{
		return BoundaryIterator(_geometry.constBeginOf(name), _data.data());
	}
	BoundaryIterator boundaryEnd(const std::string& name)
	{
		return BoundaryIterator(_geometry.constEndOf(name), _data.data());
	}
	ConstBoundaryIterator boundaryConstBegin(const std::string& name) const
	{
		return ConstBoundaryIterator(_geometry.constBeginOf(name), _data.data());
	}
	ConstBoundaryIterator boundaryConstEnd(const std::string& name) const
	{
		return ConstBoundaryIterator(_geometry.constEndOf(name), _data.data());
	}

	/**
	 * Set values on a boundary
	 */
	void setBoundaryValUniform(const char* name, const FieldType& val)
	{
		std::for_each(_geometry.beginOf(name), _geometry.endOf(name), [&](uint32_t l) { _data[l] = val; });
	}
	void setBoundaryVal(const char* name, const DataVector& vals)
	{
		if (vals.size() != _geometry.boundarySize(name)) throw std::runtime_error("Boundary and input vector sizes mismatch.\n");
		std::copy(vals.begin(), vals.end(), boundaryBegin(name));
	}

	/**
	 * Get values on a boundary
	 */
	DataVector getBoundaryVal(const char* name) const
	{
		return DataVector(boundaryConstBegin(name), boundaryConstEnd(name));
	}
};

#endif // !_FIELD_H_