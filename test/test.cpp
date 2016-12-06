// Test laplacian solver dll
//

#include <ls_main.h>
#include <iostream>
#include <mesh_math\mesh_geometry.h>

int main()
{
	try 
	{
		std::cout << "Dll message test: \n" << std::endl;
		testMsg(L"Hi from dll.");
		std::cout << "Dll message test finished!\n\n" << std::endl;

		std::cout << "Creating mesh for cube with 1.0 side length:\n";
		data_structs::graph<uint32_t> g;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
    return 0;
}

