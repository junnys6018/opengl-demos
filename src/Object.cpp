#include "Object.h"
#include "debug.h"

Object::Object(const char* filepath)
{
	std::vector<float> vb;
	std::vector<unsigned int> ib;

	std::ifstream file(filepath);
	std::string line;
	// PARSE OBJ FILE
	while (std::getline(file, line))
	{
		std::stringstream s(line);
		char id;
		s >> id;
		if (id == 'v')
		{
			float data;
			while (s >> data)
				vb.push_back(data);
		}
		else if (id == 'f')
		{
			unsigned int data;
			while (s >> data)
				ib.push_back(data - 1);
		}
	}
	// TODO: ERROR CHECKING

	vertexBuffer = VertexBuffer(vb.data(), sizeof(float) * vb.size());
	indexBuffer = IndexBuffer(ib.data(), ib.size());

	//std::cout << "VERTEX BUFFER\n";
	//for (auto i : vb)
	//	std::cout << i << '\n';
	//std::cout << "INDEX BUFFER\n";
	//for (auto i : ib)
	//	std::cout << i << '\n';
}

