#include "LSExport.h"
#include "PotentialFieldImplementation.h"
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

Mesh * Mesh::create(const Graph * g, const std::vector<V3D>& nodePositions)
{
	const GraphImplementation& g_p = dynamic_cast<const GraphImplementation&>(*g);
	std::vector<vector3f> np(nodePositions.size());
	std::transform(nodePositions.begin(), nodePositions.end(), np.begin(),
		[](V3D x)->vector3f { return vector3f{ x.x, x.y, x.z }; });
	return new MeshImplementation(g_p, np);
}

void Mesh::free(Mesh * m)
{
	delete m;
}

PotentialField * PotentialField::createZeros(const Mesh * m)
{
	return new PotentialFieldImplementation(*dynamic_cast<const mesh_geometry*>(m));
}

void PotentialField::free(PotentialField * f)
{
	delete f;
}
