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
	using BoundaryMeshSharedPtr = std::shared_ptr<mesh_geometry<double, uint32_t>::BoundaryMesh>; 
	using MeshSharedPtr = std::shared_ptr<mesh_geometry<double, uint32_t>>;
	using InterpCoef = mesh_geometry<double, uint32_t>::InterpCoef;
	using InterpCoefs = mesh_geometry<double, uint32_t>::InterpCoefs;
	using NodeTypes = std::vector<bool>;
	using vector3f = mesh_geom::vector3f;

private:
	Matrix m_matrix;
	MeshSharedPtr m_pMeshGeometry;
	BoundaryMeshSharedPtr m_pBoundaryMesh;

	NodeTypes m_nodeTypes;

	//Interpolation coefficients arithmetics
	static MatrixRow operator+(const MatrixRow& r1, const MatrixRow& r2)
	{
		MatrixRow result = r1.size() <= r2.size() ? r1 : r2;
		const MatrixRow& t = result.size() == r1.size() ? r2 : r1;
		for (const InterpCoef& coef : t)
		{
			MatrixRow::iterator it = result.find(coef.first);
			if(it != result.end())
		}
	}
public:
	FieldLinearOp(const Field& field)
		: 
		m_matrix(field.m_pMeshGeometry->size()),
		m_pMeshGeometry(field.m_pMeshGeometry),
		m_pBoundaryMesh(field.m_pBoundaryMesh),
		m_nodeTypes(field._node_types)
	{}

	//Gets the size of a field
	size_t size() const { return m_matrix.size(); }

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

	//Creates solver for equations system Ax=0, where A is laplacian
	FieldLinearOp& laplacianSolver()
	{
		for (uint32_t i = 0; i < size(); ++i)
		{
			double h = m_pMeshGeometry->shortestEdgeLength(i) / 2.0; //calculate small step
			if (m_pBoundaryMesh->isBoundary(i))
			{
				if (m_pBoundaryMesh->isFirstType(i))
				{
					m_matrix[i] = InterpCoefs{ InterpCoef{i, 1.0} };
				}
				else
				{//Zero gradient condition
					std::vector<uint32_t> neighbour(m_pMeshGeometry->neighbour(i).begin(), 
						m_pMeshGeometry->neighbour(i).end());
					std::vector<uint32_t>::iterator end
						= std::remove_if(neighbour.begin(), neighbour.end(),
							[=](uint32_t l)->bool { return m_nodeTypes[l]; });
					std::vector<vector3f> neighbourNodes(std::distance(neighbour.begin(), end), vector3f(0.0));
					std::transform(neighbour.begin(), end, neighbourNodes.begin(),
						[=](uint32_t l)->vector3f { return m_pMeshGeometry->spacePositionOf(l); });
					//Calculate first eigen vector
					vector3f e1 = math::eigenVectorSimple(math::cov(neighbourNodes));
					//Calculate average plane point
					vector3f z = std::accumulate(neighbourNodes.begin(), neighbourNodes.end(), vector3f(0.0));
					z /= double(neighbourNodes.size());
					//Remove first component
					std::for_each(neighbourNodes.begin(), neighbourNodes.end(),
						[=](vector3f& vec) 
					{
						vec -= z;
						vec -= (vec*e1)*e1;
					});
					//Calculate second eigen vector
					vector3f e2 = math::eigenVectorSimple(math::cov(neighbourNodes));
					//Calculate plane normal
					vector3f n = math::crossProduct(e1, e2);
					//Find first inner point
					std::set<uint32_t>::const_iterator it = std::find_if(m_pMeshGeometry->neighbour(i).begin(),
						m_pMeshGeometry->neighbour(i).end(), [=](uint32_t l)->bool {return m_nodeTypes[l]; });
					n = m_pMeshGeometry->spacePositionOf(*it) * n > 0.0 ? n : -n;
					//Calculate interpolation point
					vector3f r = z + n*h;
					m_matrix[i] = m_pMeshGeometry->interpCoefs(r[0], r[1], r[2], i);
				}
			}
		}
		return *this;
	}

	//Applies linear operator to a field
	void applyToField(Field& field) const
	{
		if (field.size() != size()) throw
			std::runtime_error("FieldLinearOp::applyToField:"
				"Field and operator sizes mismatch.");
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