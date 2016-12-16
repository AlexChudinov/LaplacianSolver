#pragma once
#ifndef _POTENTIAL_FIELD_IMPLEMENTATION_H_
#define _POTENTIAL_FIELD_IMPLEMENTATION_H_ 1

#include "LSExport.h"
#include "mesh_math\Field.h"

using mesh_geometry = MeshGeom<double, UINT>;
using basic_field = Field<double>;

class PotentialFieldImplementation : public PotentialField, public basic_field
{
public:
	PotentialFieldImplementation(const mesh_geometry& meshGeom);

	std::vector<double> getPotentialVals() const;

	void setBoundarydVal(const std::string& name, double val);
};

#endif // !_POTENTIAL_FIELD_IMPLEMENTATION_H_
