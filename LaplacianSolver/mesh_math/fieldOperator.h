#pragma once
#ifndef _FIELD_OPERATOR_
#define _FIELD_OPERATOR_

#include "Field.h"

//Implementation of basic field operations in the shape of linear transforamtions

template<typename field_type>
class FieldLinearOp
{
public:
	using Field = field<field_type>;
	using mesh_geom = mesh_geometry<double, uint32_t>;
	using MatrixElem = typename mesh_geom::InterpCoef;
	using MatrixRow = typename mesh_geom::InterpCoefs;
	using Matrix = std::vector<MatrixRow>;

private:
	Matrix m_matrix;
public:

	FieldLinearOp(const Field& field, size_t nThreads)
		: m_matrix(field.m_pMeshGeometry->size())
	{}

	//Sets inner matrix to identity
	FieldLinearOp& setToIdentity()
	{
		uint32_t i = 0;
		for (MatrixRow& row : m_matrix)
		{
			row.clear();
			row[i++] = 1.0;
		}
		return *this;
	}

	//Calculates laplacian field operator

	//Applies linear operator to a field
	void applyToField(Field& field) const
	{
		typename Field::data_vector data = field.data();
		size_t i = 0;
		for (const auto& row : m_matrix)
		{
			field_type& dataElem = data[i++];
			dataElem = 0.0;
			for (const auto& interpCoef : row)
			{
				dataElem += field._data[interpCoef.first] * interpCoef.second;
			}
		}
		field.data() = data;
	}
};

#endif //_FIELD_OPERATOR_