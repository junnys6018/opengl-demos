#include "Object.h"
#include "debug.h"
#include <assert.h>
#include <chrono>

#define ENABLE_TIMING
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

Object::Object(const std::string& filepath)
{
#ifdef ENABLE_TIMING
	auto a = std::chrono::steady_clock::now();
#endif
	const std::vector<std::string> mtlPaths = parse_obj(filepath);

	for (auto mtlPath : mtlPaths)
		parse_mtl(mtlPath);

	// Link mat_ptrs to materials
	for (auto& matptr : mat_ptrs)
		for (auto& mat : materials)
			if (matptr.m_mtlName == mat.m_mtlName)
			{
				matptr.m_material = &mat;
				break;
			}

#ifdef ENABLE_TIMING
	auto b = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> work_time = b - a;
	std::cout << "Loaded " << filepath << " in " << work_time.count() << "ms\n";
#endif

	//std::cout << "VERTEX BUFFER\n";
	//for (auto i : vBuf)
	//{
	//	std::cout << "v " << i.position.x << ' ' << i.position.y << ' ' << i.position.z << std::endl;
	//}
	//std::cout << "INDEX BUFFER\n";
	//for (int i = 0; i < iBuf.size(); ++i)
	//	std::cout << iBuf[i] << (i % 3 == 2 ? '\n' : ' ');
	//std::cout << "MTL FILES" << std::endl;
	//for (auto s : mtlPaths)
	//{
	//	std::cout << s << std::endl;
	//}
}

const std::vector<std::string> Object::parse_obj(const std::string filepath)
  // Parses .obj file,
  // TODO: parse shading parameters in mtl file (Ka, Kd, Ks, Ns, Ni, d, Tf, illum)
  // TODO: support parsing for indicies where each component has a differnt index (e.g. 2/3/4)
  // 
  // we assume all indicies draw from the the position for v, vn and vt. ie. of the form
  // x/x/x or x//x
  // 
{
	std::vector<Vertex> vBuf;
	std::vector<unsigned int> iBuf;
	std::vector<std::string> mtlPaths;

	std::string rootDir = calc_root_dir(filepath);

	std::ifstream file(filepath);
	std::string line;

	unsigned int vnIndex = 0;
	unsigned int vtIndex = 0;
	unsigned int fIndex = 0; // points to 1 off the last index
	while (std::getline(file, line))
	{
		std::stringstream s(line);
		std::string id;
		s >> id;
		if (id == "v")
		{
			Vertex data;
			s >> data.position.x;
			s >> data.position.y;
			s >> data.position.z;
			vBuf.push_back(data);
		}
		else if (id == "vn")
		{
			assert(vnIndex < vBuf.size());
			s >> vBuf[vnIndex].normal.x;
			s >> vBuf[vnIndex].normal.y;
			s >> vBuf[vnIndex].normal.z;
			++vnIndex;
		}
		else if (id == "vt")
		{
			assert(vtIndex < vBuf.size());
			s >> vBuf[vtIndex].texCoord.s;
			s >> vBuf[vtIndex].texCoord.t;
			++vtIndex;
		}
		else if (id == "f")
		{
			std::string data;
			while (s >> data)
			{
				int index = data.find('/');

				iBuf.push_back(std::stoul(data.substr(0, index)) - 1);
			}
			if (mat_ptrs.size() == 0) // if no material has been declared
			{
				mat_ptrs.emplace_back(-1, 0);
			}
			fIndex += 3;
		}
		else if (id == "usemtl")
		{
			if (mat_ptrs.size() != 0)
				(mat_ptrs.end() - 1)->m_count = fIndex - (mat_ptrs.end() - 1)->m_offset;
			s >> id;
			mat_ptrs.emplace_back(-1, fIndex, id);
		}
		else if (id == "mtllib")
		{
			// we assume filepath is relative to directory containing .obj file
			s >> id;
			mtlPaths.push_back(rootDir + id);
		}
	}
	(mat_ptrs.end() - 1)->m_count = fIndex - (mat_ptrs.end() - 1)->m_offset; // read last index	

	vertexBuffer = std::make_unique<VertexBuffer>(vBuf.data(), sizeof(Vertex) * vBuf.size());
	indexBuffer = std::make_unique<IndexBuffer>(iBuf.data(), iBuf.size());
	// init vertex array
	vertexArray = std::unique_ptr<VertexArray>(new VertexArray(*vertexBuffer, GL_FLOAT, { 3,3,2 }));
	vertexArray->addIndexBuffer(*indexBuffer);
	return mtlPaths;
}

void Object::parse_mtl(const std::string filepath)
{
	std::ifstream mtlFile(filepath);
	std::string line;
	std::string rootDir = calc_root_dir(filepath);
	while (std::getline(mtlFile, line))
	{
		std::istringstream s(line);
		std::string id;
		s >> id;
		if (id == "newmtl")
		{
			s >> id;
			materials.push_back(Material(id));
		}
		else if (id == "map_Kd")
		{
			s >> id;
			(materials.end() - 1)->genTexture(rootDir + id);
		}
	}
}

inline std::string Object::calc_root_dir(std::string filepath)
	// given "res/Objects/Pokemon/Pikachu.obj" returns "res/Objects/Pokemon/"
{
	unsigned int index = filepath.rfind('/');
	assert(index < filepath.size());
	return filepath.substr(0, index + 1);
}

void Object::Draw(const Shader& shader)
{
	vertexArray->Bind();
	shader.Use();
	for (auto& mat_ptr : mat_ptrs)
	{
		if (mat_ptr.m_mtlName != "")
		{
			mat_ptr.m_material->m_texture->Bind();
			shader.setInt("Texture1", 0);
		}
		GLCall(glDrawElements(GL_TRIANGLES, mat_ptr.m_count, GL_UNSIGNED_INT,
			(void*)(mat_ptr.m_offset * sizeof(unsigned int))));
	}
}



