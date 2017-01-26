// Test laplacian solver dll
//
#define _USE_LS_DLL_
#include <ls_main.h>
#include <LSExport.h>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

Mesh* readConnectivity(std::ostream& readLog, const char* filename)
{
	Graph* g = Graph::create();
	std::ifstream in;
	in.open(filename);
	if (!in) throw std::runtime_error("Could not open test file.");

	std::string line; size_t nElems;

	std::getline(in, line);
	readLog << "Load " << line << std::endl;
	in >> nElems;
	readLog << "Number of elements: " << nElems << std::endl;

	//Load node space positions
	std::vector<V3D> ndPositions(nElems);
	size_t num; //Dummy number
	for (size_t i = 0; i < nElems; ++i)
		in >> num >> ndPositions[i].x >> ndPositions[i].y >> ndPositions[i].z;

	//Skip empty elements
	for (size_t i = 0; i < 7; ++i)
	{
		std::getline(in, line);
		readLog << "Skip line: " << line << "\n";
	}
	std::getline(in, line);
	readLog << "Load " << line << std::endl;
	in >> nElems;
	readLog << "Number of elements: " << nElems << std::endl;

	//Load geometric elements
	for (size_t i = 0; i < nElems; ++i)
	{
		UINT n0, n1, n2, n3, n4, n5, n6, n7;
		in >> n0 >> n1 >> n2 >> n3 >> n4 >> n5 >> n6 >> n7;
		//Note, element indexing is strickly starting at 0.
		//I am not sured, but this can be important
		--n0; --n1; --n2; --n3; --n4; --n5; --n6; --n7;
		g->addHexa(n0, n1, n2, n3, n4, n5, n6, n7);
	}

	Mesh* m = Mesh::create(g, ndPositions);

	in.close();
	Graph::free(g);
	return m;
}

void readBoundaries(PotentialField* f, std::ostream& readLog, const char* filename)
{
	std::set<UINT> labels; //Here we keep the boundary labels, note: start index is 0

	size_t boundaryNum;
	std::string line;
	std::ifstream in;
	in.open(filename);
	if (!in) throw std::runtime_error("Could not open test file.");

	in >> boundaryNum;
	std::getline(in, line);
	readLog << "\nNumber of boundaries: " << boundaryNum << "\n";
	for (size_t i = 0; i < boundaryNum; ++i)
	{
		size_t nElems;
		std::string skip_line;
		std::getline(in, line);
		readLog << "\nReading boundary: " << line << "\n";
		std::getline(in, skip_line); //Skip triangles
		in >> nElems;
		readLog << "Number of square elements is: " << nElems << "\n";
		for (size_t j = 0; j < nElems; ++j)
		{
			UINT label;
			for (size_t k = 0; k < 4; ++k)
			{
				in >> label;
				labels.insert(label - 1);
			}
		}

		//New boundary!!! Labels start at 0 idx
		f->addBoundary(line, labels);
		///

		std::getline(in, skip_line); //Skip one line
		labels.clear(); //Clear for next accumulation
	}
	in.close();
}

/**
 * max difference between vectors
 */
double field_diff(const std::vector<double>& v1, const std::vector<double>& v2)
{
	struct max : public std::binary_function<double, double, double>
	{
		double operator()(double x1, double x2) const { return std::max(x1, x2); }
	};
	struct diff : public std::binary_function<double, double, double>
	{
		double operator()(double x1, double x2) const { return (x1 - x2) * (x1 - x2); }
	};
	return std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0, 
		max(), diff());
}

int main()
{
	try 
	{
		std::cout << "Creating test mesh for cube:\n";
		//Create mesh

		Mesh* m = readConnectivity(std::cout, "test_files/cube.geom");
		PotentialField* f = PotentialField::createZeros(m);

		std::pair<V3D, V3D> box = m->getBox();

		std::cout << "Box containing all geometry: "
			<< "xmin = " << box.first.x << std::endl
			<< "xmax = " << box.second.x << std::endl
			<< "ymin = " << box.first.y << std::endl
			<< "ymax = " << box.second.y << std::endl
			<< "zmin = " << box.first.z << std::endl
			<< "zmax = " << box.second.z << std::endl;

		std::vector<V3D> line(200);
		int n = 0;
		std::generate(line.begin(), line.end(), [&]()->V3D 
		{ 
			double x, y, z;
			x = 0.0052;//box.first.x + (box.second.x - box.first.x) / 199. * (n);
			y = 0.0023; //box.first.y + (box.second.y - box.first.y) / 199. * (n);
			z = box.first.z + (box.second.z - box.first.z) / 199. * (n++);
			return{ x,y,z };
		});

		Mesh::free(m);

		readBoundaries(f, std::cout, "test_files/cube.rgn");

		//Create field
		f->setBoundaryType("F21.16", PotentialField::ZERO_GRAD);
		f->setBoundaryType("F19.16", PotentialField::ZERO_GRAD);
		f->setBoundaryType("F18.16", PotentialField::ZERO_GRAD);
		f->setBoundaryType("F22.16", PotentialField::ZERO_GRAD);
		f->setBoundaryVal("F20.16", 1.0);
		f->setBoundaryVal("F17.16", -1.0);
		f->setBoundaryType("F20.16", PotentialField::FIXED_VAL);
		f->setBoundaryType("F17.16", PotentialField::FIXED_VAL);
		std::cout << "Field calculation: \n";
		f->applyBoundaryConditions();
		for (int i = 0; i < 100; ++i)
		{
			std::vector<double> field = f->getPotentialVals();
			f->diffuse();
			std::vector<double> field2 = f->getPotentialVals();
			std::cout << "step: " << i << "diff: " << field_diff(field, field2) << std::endl;
		}

		std::vector<double> field_vals(line.size());
		std::transform(line.begin(), line.end(), field_vals.begin(),
			[&](V3D pos)->double
		{
			return f->interpolate(pos.x, pos.y, pos.z);
		});

		std::ofstream out;
		out.open("test.dat");
		std::copy(field_vals.begin(), field_vals.end(), std::ostream_iterator<double>(out, "\n"));
		out.close();

		std::vector<double> fld = f->getPotentialVals();

		std::cout << "\nTest identity operator: \n";
		ScalarFieldOperator* op = ScalarFieldOperator::create(f);
		op->applyToField(f);
		std::cout << "Field difference after operator application = " << field_diff(fld, f->getPotentialVals());
		std::cout << std::endl;

		PotentialField::free(f);
		ScalarFieldOperator::free(op);
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		return 1;
	}
}

