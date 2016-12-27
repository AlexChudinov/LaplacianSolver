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
	/**
	 * Creates new mesh
	 */
	static Mesh* create(const Graph* g, const std::vector<V3D>& nodePositions);

	//Deletes mesh instance
	static void free(Mesh* m);

	//Returns box defined by two points containing all mesh vertices
	virtual std::pair<V3D, V3D> getBox() const = 0;
};

class LAPLACIAN_SOLVER_EXPORT PotentialField
{
public:
	//Acceptable types of boundaries
	enum BOUNDARY_TYPE { FIXED_VAL, ZERO_GRAD };

	//Creates potential field filled with zeros
	static PotentialField* createZeros(Mesh* m);
	static void free(PotentialField* f);

	//Get current field values. The indices of the values correspond to the number of labels in a graph
	virtual std::vector<double> getPotentialVals() const = 0;

	//Set boundary field values
	virtual void setBoundaryVal(const std::string& name, double val) = 0;

	//Set boundary field values
	virtual void setBoundaryVal(const std::string& name, const std::vector<double>& vals) = 0;

	//Adds new boundary node labels should be listed
	virtual void addBoundary(const std::string& name, const std::set<UINT>& nodeLabels) = 0;

	//Sets boundary type
	virtual void setBoundaryType(const std::string& name, BOUNDARY_TYPE type) = 0;

	//Returns list of all boundary names
	virtual std::vector<std::string> getBoundaryNames() const = 0;

	//Make one step of laplacian solver
	virtual void diffuse() = 0;

	//Interpolate field value at current point
	virtual double interpolate(double x, double y, double z, UINT* track_label = NULL) const = 0;
};

#endif // !_LS_EXPORT_H_
