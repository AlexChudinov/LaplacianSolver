#pragma once
#ifndef _LS_EXPORT_H_
#define _LS_EXPORT_H_

#include "ls_main.h"
#include <stdint.h>

class LAPLACIAN_SOLVER_EXPORT Graph 
{
public:
	//Adds hexahedral
	virtual void addHexa(uint32_t n0, uint32_t n1, uint32_t n2, uint32_t n3, 
		uint32_t n4, uint32_t n5, uint32_t n6, uint32_t n7) = 0;

	//Creates new graph
	static Graph* create();

	//Deletes graph instance
	static void free(Graph* g);
};

//Forward class declaration
namespace std { template<typename ... T> class vector; }
//Dummy class that can be removed with typedef Vector3D v3d
struct v3d { double x, y, z; };

class LAPLACIAN_SOLVER_EXPORT Mesh
{
public:
	static Mesh* create(const Graph* g, const std::vector<v3d>& nodePositions);
};

#endif // !_LS_EXPORT_H_
