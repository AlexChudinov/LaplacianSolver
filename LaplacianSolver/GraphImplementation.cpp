#include "GraphImplementation.h"

void GraphImplementation::addHexa(uint32_t n0, uint32_t n1, uint32_t n2, uint32_t n3, uint32_t n4, uint32_t n5, uint32_t n6, uint32_t n7)
{
	base_graph::addHexa({ n0, n1, n2, n3, n4, n5, n6, n7 });
}
