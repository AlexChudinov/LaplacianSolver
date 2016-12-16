#include "MeshImplementation.h"

MeshImplementation::MeshImplementation(const graph& g, const node_positions& np)
	: Mesh(), MeshGeom<double, uint32_t>(g, np)
{}