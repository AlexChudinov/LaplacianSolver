// Test laplacian solver dll
//
#define _USE_LS_DLL_
#include <ls_main.h>
#include <LSExport.h>
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

int main()
{
	try 
	{
		std::cout << "Creating test mesh for cube:\n";
		//Create mesh

		Mesh* m = readConnectivity(std::cout, "test_files/cube.geom");
		PotentialField* f = PotentialField::createZeros(m);
		Mesh::free(m);

		readBoundaries(f, std::cout, "test_files/cube.rgn");

		//Create field
		std::vector<std::string> names = f->getBoundaryNames();
		f->setBoundaryVal(names[0], 1.0);

		f->diffuse();

		PotentialField::free(f);
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		return 1;
	}
}

