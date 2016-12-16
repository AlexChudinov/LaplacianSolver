#pragma once
#ifndef _MESH_IMPLEMENTATION_H_
#define _MESH_IMPLEMENTATION_H_ 1

#include "LSExport.h"
#include "mesh_math\mesh_geometry.h"

using graph = data_structs::graph<UINT>;
using vector3f = math::vector_c<double, 3>;
using node_positions = std::vector<vector3f>;

class MeshImplementation : public Mesh, public MeshGeom<double, UINT>
{
	using basic_mesh_geometry = MeshGeom<double, UINT>;
public:
	MeshImplementation(const graph& g, const node_positions& np);

	void addBoundary(const std::string& name, const std::set<UINT>& nodeLabels);

	bool setBoundaryType(const std::string& name, BOUNDARY_TYPE type);

	BOUNDARY_TYPE getBoundaryType(const std::string& name) const;
};

#endif // !_MESH_IMPLEMENTATION_H_
