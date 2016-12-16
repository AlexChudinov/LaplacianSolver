#include <vector>

#include "LSExport.h"
#include "GraphImplementation.h"
#include "MeshImplementation.h"

Graph * Graph::create()
{
	return new GraphImplementation;
}

void Graph::free(Graph * g)
{
	delete g;
}

Mesh * Mesh::create(const Graph * g, const std::vector<double[3]>& nodePositions)
{
	const GraphImplementation& g_p = dynamic_cast<const GraphImplementation&>(*g);
	return new MeshImplementation(g_p, std::vector<vector3f>(nodePositions));
}
