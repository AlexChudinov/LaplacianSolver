#pragma once
#ifndef _MESH_IMPLEMENTATION_H_
#define _MESH_IMPLEMENTATION_H_ 1

#include "LSExport.h"
#include "mesh_math\mesh_geometry.h"

using graph = data_structs::graph<uint32_t>;
using vector3f = math::vector_c<double, 3>;
using node_positions = std::vector<vector3f>;

class MeshImplementation : public Mesh, public MeshGeom<double, uint32_t>
{
public:
	MeshImplementation(const graph& g, const node_positions& np);
};

#endif // !_MESH_IMPLEMENTATION_H_
