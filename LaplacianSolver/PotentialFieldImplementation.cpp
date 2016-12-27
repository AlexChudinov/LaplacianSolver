#include "PotentialFieldImplementation.h"
#include "MeshImplementation.h"

PotentialFieldImplementation::PotentialFieldImplementation(Mesh* meshGeom)
	: 
	basic_field(*dynamic_cast<MeshImplementation*>(meshGeom)->geometryPtr()),
	_pGeometry(dynamic_cast<MeshImplementation*>(meshGeom)->geometryPtr())
{}

std::vector<double> PotentialFieldImplementation::getPotentialVals() const
{
	return basic_field::data();
}

void PotentialFieldImplementation::setBoundaryVal(const std::string & name, double val)
{
	basic_field::set_boundary_uniform_val(name, val);
}

void PotentialFieldImplementation::setBoundaryVal(const std::string & name, const std::vector<double> vals)
{
	basic_field::set_boundary_vals(name, vals);
}

void PotentialFieldImplementation::addBoundary(const std::string & name, const std::set<UINT>& nodeLabels)
{
	basic_field::add_boundary(name, nodeLabels);
}

void PotentialFieldImplementation::setBoundaryType(const std::string & name, BOUNDARY_TYPE type)
{
	switch (type)
	{
	case FIXED_VAL: return basic_field::set_boundary_type(name, BOUNDARY_FIXED_VALUE);
	case ZERO_GRAD: return basic_field::set_boundary_type(name, BOUNDARY_ZERO_GRADIENT);
	default: throw std::runtime_error(
		"PotentialFieldImplementation::setBoundaryType :"
		"Unsupported boundary field type");
	}
}

std::vector<std::string> PotentialFieldImplementation::getBoundaryNames() const
{
	return basic_field::get_boundary_names();
}

void PotentialFieldImplementation::diffuse()
{
	basic_field next = basic_field::diffuse();
	data() = next.data();
}

double PotentialFieldImplementation::interpolate(double x, double y, double z, UINT * track_label) const
{
	return basic_field::interpolate(x, y, z, track_label);
}

