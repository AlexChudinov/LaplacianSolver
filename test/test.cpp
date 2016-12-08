// Test laplacian solver dll
//

#include <ls_main.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <mesh_math\mesh_geometry.h>

int main()
{
	try 
	{
		using MeshGeom = MeshGeom<double, uint32_t>;
		using graph = typename MeshGeom::graph;

		std::cout << "Creating test mesh for cube:\n";
		data_structs::graph<uint32_t> g;
		std::ifstream in;
		in.open("test_files/cube.geom");
		if (!in) throw std::runtime_error("Could not open test file.");

		//Load node positions
		std::string line; size_t nElems;
		std::getline(in, line);
		std::cout << "Load " << line << std::endl;
		in >> nElems;
		std::cout << "Number of elements: " << nElems << std::endl;
		typename MeshGeom::node_positions ndPositions(nElems);
		size_t num;
		for (size_t i = 0; i < nElems; ++i) in >> num >> ndPositions[i];

		//Skip empty elements
		for (size_t i = 0; i < 7; ++i)
		{
			std::getline(in, line);
			std::cout << "Skip line: " << line << "\n";
		}
		std::getline(in, line);
		std::cout << "Load " << line << std::endl;
		in >> nElems;
		std::cout << "Number of elements: " << nElems << std::endl;

		for ()

		in.close();
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
    return 0;
}

