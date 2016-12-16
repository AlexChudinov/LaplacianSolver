#include "PotentialFieldImplementation.h"

PotentialFieldImplementation::PotentialFieldImplementation(const mesh_geometry& meshGeom)
	: basic_field(meshGeom)
{}

std::vector<double> PotentialFieldImplementation::getPotentialVals() const
{
	return basic_field::data();
}

void PotentialFieldImplementation::setBoundarydVal(const std::string & name, double val)
{
	basic_field::setBoundaryValUniform(name, val);
}

