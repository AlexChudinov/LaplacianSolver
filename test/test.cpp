// Test laplacian solver dll
//
#define _USE_LS_DLL_
#include <ls_main.h>
#include <LSExport.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main()
{
	try 
	{
		std::cout << "Creating test mesh for cube:\n";
		Graph* g = Graph::create();
		std::ifstream in;
		in.open("test_files/cube.geom");
		if (!in) throw std::runtime_error("Could not open test file.");

		//Load node positions
		std::string line; size_t nElems;
		std::getline(in, line);
		std::cout << "Load " << line << std::endl;
		in >> nElems;
		std::cout << "Number of elements: " << nElems << std::endl;
		std::vector<double> ndPositions(3*nElems);
		size_t num, j = 0;
		for (size_t i = 0; i < nElems; ++i) 
			in >> num >> ndPositions[j++] >> ndPositions[j++] >> ndPositions[j++];

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
			uint32_t n0, n1, n2, n3, n4, n5, n6, n7;
			in >> n0 >> n1 >> n2 >> n3 >> n4 >> n5 >> n6 >> n7;
			--n0; --n1; --n2; --n3; --n4; --n5; --n6; --n7;
			g->addHexa(n0, n1, n2, n3, n4, n5, n6, n7);
		}
		in.close();
		Graph::free(g);
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		return 1;
	}
    return 0;
}

