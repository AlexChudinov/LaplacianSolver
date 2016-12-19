#pragma once
#ifndef _GRAPH_IMPLEMENTATION_H_
#define _GRAPH_IMPLEMENTATION_H_ 1

#include <data_structs\graph.h>
#include "LSExport.h"

class GraphImplementation : public Graph, public data_structs::graph<uint32_t>
{
	using base_graph = data_structs::graph<uint32_t>;
public:
	void addHexa(UINT n0, UINT n1, UINT n2, UINT n3, UINT n4, UINT n5, UINT n6, UINT n7);
};

#endif // !_GRAPH_IMPLEMENTATION_H_
