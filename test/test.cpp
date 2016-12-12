// Test laplacian solver dll
//

#include <ls_main.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <mesh_math\mesh_geometry.h>
#include <mesh_math\Field.h>

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
		//Load geometric elements
		for (size_t i = 0; i < nElems; ++i)
		{
			math::vector_c<uint32_t, 8> verts;
			in >> verts;
			verts -= 1U;
			g.addHexa(verts);
		}
		in.close();
		MeshGeom meshGeom(g, ndPositions);
		//Open boundary file
		in.open("test_files/cube.rgn");
		if (!in) throw std::runtime_error("Could not open test file.");
		in >> nElems;
		std::cout << "Number of boundaries: " << nElems << ".\n";
		std::getline(in, line);
		for (size_t i = 0; i < nElems; ++i)
		{
			MeshGeom::boundary_entry bdEntry;
			std::getline(in, line);
			std::cout << "Load " << line << ".\n";
			bdEntry.first = line;
			 //Skip line
			std::getline(in, line);
			size_t nSquares;
			in >> nSquares;
			std::cout << "Number of elements: " << nSquares << ".\n";
			for (size_t j = 0; j < nSquares; ++j)
			{
				for (size_t k = 0; k < 4; ++k)
				{
					uint32_t vertex;
					in >> vertex;
					bdEntry.second.insert(vertex-1);
				}
			}
			meshGeom.addBoundary(bdEntry);
			std::getline(in, line);
		}
		in.close();

		//Substract boundaries list
		std::vector<std::string> boundaryList(meshGeom.boundaryNum());
		meshGeom.getBoundaryNames(boundaryList.begin());

		std::cout << "\nZero filled field creation" << std::endl;
		Field<double> field(meshGeom);
		std::vector<double> boundaryVal = field.getBoundaryVal(boundaryList[0].c_str());

	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		return 1;
	}
    return 0;
}

