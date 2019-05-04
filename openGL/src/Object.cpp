#include "Object.h"
#include "debug.h"
#include <assert.h>
#include <chrono>

#define ENABLE_TIMING
Draw_Flags operator|(Draw_Flags lhs, Draw_Flags rhs)
{
	return (Draw_Flags)((int)lhs | (int)rhs);
}
Object::Object(const std::string& filepath, Object_Init_Flags flags)
{
#ifdef ENABLE_TIMING
	auto a = std::chrono::steady_clock::now();
#endif
	std::vector<Vertex> vBuf;
	std::vector<unsigned int> iBuf;

	const std::vector<std::string> mtlPaths = parse_obj(filepath, vBuf, iBuf);

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
	if (flags & OBJECT_INIT_FLAGS_GEN_TANGENT)
	{
		vertexBuffer = std::make_unique<VertexBuffer>((sizeof(Vertex) + sizeof(glm::vec4)) * vBuf.size());
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vBuf.size(), vBuf.data()));
	}
	else
	{
		vertexBuffer = std::make_unique<VertexBuffer>(vBuf.data(), sizeof(Vertex) * vBuf.size());
	}
	indexBuffer = std::make_unique<IndexBuffer>(iBuf.data(), iBuf.size());
	// init vertex array
	vertexArray = std::unique_ptr<VertexArray>(new VertexArray(*vertexBuffer, GL_FLOAT, { 3,3,2 }));
	vertexArray->addIndexBuffer(*indexBuffer);

	if (flags & OBJECT_INIT_FLAGS_GEN_TANGENT)
		genTangents(vBuf, iBuf);

}

const std::vector<std::string> Object::parse_obj(const std::string filepath, std::vector<Vertex>& vBuf, std::vector<unsigned int>& iBuf )
  // Parses .obj file,
  // TODO: support parsing for indicies where each component has a differnt index (e.g. 2/3/4)
  // 
  // we assume all indicies draw from the the position for v, vn and vt. ie. of the form
  // x/x/x or x//x
  // 
{
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
	return mtlPaths;
}

void Object::parse_mtl(const std::string filepath)
// TODO: parse shading parameters in mtl file (Ka, Kd, Ks, Ns, Ni, d, Tf, illum)
{
	std::ifstream mtlFile(filepath);
	if (!mtlFile)
	{
		std::cout << "Failed to read " << filepath << std::endl;
		return;
	}
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
		else if (id == "map_Bump")
		{
			s >> id;
			(materials.end() - 1)->genNormMap(rootDir + id);
		}
	}
}
// Tangents have attrib location = 3
void Object::genTangents(const std::vector<Vertex>& vBuf, const std::vector<unsigned int>& iBuf)
{
#ifdef ENABLE_TIMING
	auto a = std::chrono::steady_clock::now();
#endif
	std::vector<glm::vec3> Tangnets;
	std::vector<glm::vec3> biTangents;

	Tangnets.resize(vBuf.size());
	biTangents.resize(vBuf.size());

	// For each triangle
	for (unsigned int i = 0; i < iBuf.size(); i += 3)
	{
		unsigned int index1 = iBuf[i];
		unsigned int index2 = iBuf[i + 1];
		unsigned int index3 = iBuf[i + 2];

		glm::vec3 pos1 = vBuf[index1].position;
		glm::vec3 pos2 = vBuf[index2].position;
		glm::vec3 pos3 = vBuf[index3].position;

		glm::vec2 uv1 = vBuf[index1].texCoord;
		glm::vec2 uv2 = vBuf[index2].texCoord;
		glm::vec2 uv3 = vBuf[index3].texCoord;

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float invDet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		glm::vec3 tangent;
		tangent.x = invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);
		glm::vec3 bitangent;
		bitangent.x = invDet * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = invDet * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = invDet * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		Tangnets[index1] += tangent;
		Tangnets[index2] += tangent;
		Tangnets[index3] += tangent;

		biTangents[index1] += bitangent;
		biTangents[index2] += bitangent;
		biTangents[index3] += bitangent;
	}
	std::vector<glm::vec4> finTangents;
	for (unsigned int i = 0; i < vBuf.size(); i++)
	{
		const glm::vec3& n = vBuf[i].normal;
		const glm::vec3& t = Tangnets[i];
		// Gram-Schmidt orthogonalize
		glm::vec4 Tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 1.0f);
		// Calculate biTan direction
		Tangent.w = glm::dot(glm::cross(n, t), biTangents[i]) < 0.0f ? -1.0f : 1.0f;
		finTangents.push_back(Tangent);
	}
	vertexArray->Bind();
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vBuf.size(), sizeof(glm::vec4) * finTangents.size(), finTangents.data()));
	GLCall(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(Vertex) * vBuf.size())));
	GLCall(glEnableVertexAttribArray(3));
	vertexArray->unBind();

#ifdef ENABLE_TIMING
	auto b = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> work_time = b - a;
	std::cout << "Generated tangents in " << work_time.count() << "ms\n";
#endif
}

inline std::string Object::calc_root_dir(std::string filepath)
	// given "res/Objects/Pokemon/Pikachu.obj" returns "res/Objects/Pokemon/"
{
	unsigned int index = filepath.rfind('/');
	if (index < filepath.size())
		return filepath.substr(0, index + 1U);
	else return "";
}

void Object::Draw(const Shader& shader, Draw_Flags flags)
{
	vertexArray->Bind();
	shader.Use();
	for (auto& mat_ptr : mat_ptrs)
	{
		if (mat_ptr.m_material)
		{
			if (mat_ptr.m_material->hasTexture && (flags & DRAW_FLAGS_DIFFUSE))
			{
				mat_ptr.m_material->m_texture->Bind();
				shader.setInt("Texture1", 0);
			}
			if (mat_ptr.m_material->hasNormMap && (flags & DRAW_FLAGS_NORMAL))
			{
				mat_ptr.m_material->m_NormMap->Bind(1);
				shader.setInt("NormMap", 1);
			}
		}
		GLCall(glDrawElements(GL_TRIANGLES, mat_ptr.m_count, GL_UNSIGNED_INT,
			(void*)(mat_ptr.m_offset * sizeof(unsigned int))));
	}
}



