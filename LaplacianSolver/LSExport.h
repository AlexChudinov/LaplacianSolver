#pragma once
#ifndef _LS_EXPORT_H_
#define _LS_EXPORT_H_

#include "ls_main.h"

#include <set>
#include <vector>

class LAPLACIAN_SOLVER_EXPORT Graph 
{
public:
	//Adds hexahedral
	virtual void addHexa(UINT n0, UINT n1, UINT n2, UINT n3, UINT n4, UINT n5, UINT n6, UINT n7) = 0;

	//Creates new graph
	static Graph* create();

	//Deletes graph instance
	static void free(Graph* g);
};

//Dummy struct that can be changed to a Vector3D
struct V3D { double x, y, z; };

class LAPLACIAN_SOLVER_EXPORT Mesh
{
public:
	//Acceptable types of boundaries
	enum BOUNDARY_TYPE { FIXED_VAL, ZERO_GRAD};

	/**
	 * Creates new mesh
	 */
	static Mesh* create(const Graph* g, const std::vector<V3D>& nodePositions);

	//Deletes mesh instance
	static void free(Mesh* m);

	//Adds new boundary node labels should be listed
	virtual void addBoundary(const std::string& name, const std::set<UINT>& nodeLabels) = 0;

	//Works with the types of the boundaries, boundary type by default is FIXED_VAL
	//Set boundary type returns true if it was succeded
	virtual bool setBoundaryType(const std::string& name, BOUNDARY_TYPE type) = 0;
	virtual BOUNDARY_TYPE getBoundaryType(const std::string& name) const = 0;
};

class LAPLACIAN_SOLVER_EXPORT PotentialField
{
public:
	//Creates potential field filled with zeros
	static PotentialField* createZeros(const Mesh* m);
	static void free(PotentialField* f);

	//Get current field values. The indices of the values correspond to number of labels in a graph
	virtual std::vector<double> getPotentialVals() const = 0;

	//Set boundary field values
	virtual void setBoundaryVal(const std::string& name, double val) = 0;
};

#endif // !_LS_EXPORT_H_
