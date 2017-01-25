#pragma once
#ifndef _FIELD_OPERATOR_
#define _FIELD_OPERATOR_

#include "Field.h"

//Implementation of basic field operations in the shape of linear transforamtions

template<typename field_type>
class FieldLinearOp
{
	using Field = field<field_type>;
	using MatrixRow = typename Field::mesh_geom::InterpCoefs;
	using Matrix = std::vector<MatrixRow>;

	Matrix m_matrix;

	FieldLinearOp(const Field& field)
		:
		m_matrix(field.m_pMeshGeometry->size())
	{}
public:
	enum OpType
	{
		Identity //Identity field transform matrix
	};

	static FieldLinearOp create(const Field& field, OpType type = Identity)
	{
		FieldLinearOp result(field);
		switch (type)
		{
		case Identity:
			for(MatrixRow& row : result.m_matrix)
		default:
			break;
		}
	}
};

#endif //_FIELD_OPERATOR_