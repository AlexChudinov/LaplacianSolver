#pragma once
#ifndef _GRAPH_IMPLEMENTATION_H_
#define _GRAPH_IMPLEMENTATION_H_ 1

#include <data_structs\graph.h>
#include "LSExport.h"

class GraphImplementation : public Graph, public data_structs::graph<uint32_t>
{
	using base_graph = data_structs::graph<uint32_t>;
public:
	void addHexa(
		uint32_t n0,
		uint32_t n1,
		uint32_t n2,
		uint32_t n3,
		uint32_t n4,
		uint32_t n5,
		uint32_t n6,
		uint32_t n7);
};

#endif // !_GRAPH_IMPLEMENTATION_H_
