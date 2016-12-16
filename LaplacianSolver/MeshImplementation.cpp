#include "MeshImplementation.h"

MeshImplementation::MeshImplementation(const graph& g, const node_positions& np)
	: Mesh(), MeshGeom<double, UINT>(g, np)
{}

void MeshImplementation::addBoundary(const std::string & name, const std::set<UINT>& nodeLabels)
{
	basic_mesh_geometry::addBoundary(std::make_pair(name, nodeLabels));
}

bool MeshImplementation::setBoundaryType(const std::string & name, BOUNDARY_TYPE type)
{
	switch (type)
	{
	case Mesh::FIXED_VAL:
		return basic_mesh_geometry::setBoundaryType(name, basic_mesh_geometry::BOUNDARY_FIXED_VALUE);
	case Mesh::ZERO_GRAD:
		return basic_mesh_geometry::setBoundaryType(name, basic_mesh_geometry::BOUNDARY_ZERO_GRADIENT);
	default:
		return false;
	}
}

Mesh::BOUNDARY_TYPE MeshImplementation::getBoundaryType(const std::string & name) const
{
	switch (basic_mesh_geometry::getBoundaryType(name))
	{
	case basic_mesh_geometry::BOUNDARY_FIXED_VALUE: return FIXED_VAL;
	case basic_mesh_geometry::BOUNDARY_ZERO_GRADIENT: return ZERO_GRAD;
	default: throw std::runtime_error(
		"MeshImplementation::getBoundaryType: "
		"Ops! The program logic ways is unpredictable sometimes!");
	}
}
