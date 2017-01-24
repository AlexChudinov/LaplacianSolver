#pragma once
#ifndef _POTENTIAL_FIELD_IMPLEMENTATION_H_
#define _POTENTIAL_FIELD_IMPLEMENTATION_H_ 1

#include <memory>

#include "LSExport.h"
#include "mesh_math\Field.h"

using mesh_geom = mesh_geometry<double, UINT>;
using basic_field = field<double>;

class PotentialFieldImplementation : public PotentialField, public basic_field
{
public:
	PotentialFieldImplementation(Mesh* meshGeom);

	const std::vector<double>& getPotentialVals() const;

	void setBoundaryVal(const std::string& name, double val);

	void setBoundaryVal(const std::string& name, const std::vector<double>& vals);

	void addBoundary(const std::string& name, const std::set<UINT>& nodeLabels);

	void setBoundaryType(const std::string& name, BOUNDARY_TYPE type);

	void applyBoundaryConditions();

	void diffuse();

	double interpolate(double x, double y, double z, UINT* track_label) const;
};

#endif // !_POTENTIAL_FIELD_IMPLEMENTATION_H_
