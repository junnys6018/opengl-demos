#include "Object.h"
#include "debug.h"
#include <assert.h>
#include <chrono>

#define ENABLE_TIMING
// Enums
Draw_Flags operator|(Draw_Flags lhs, Draw_Flags rhs)
{
	return (Draw_Flags)((int)lhs | (int)rhs);
}
Object_Init_Flags operator|(Object_Init_Flags lhs, Object_Init_Flags rhs)
{
	return (Object_Init_Flags)((int)lhs | (int)rhs);
}
Texture_Init_Flags cast(Object_Init_Flags flags)
{
	Texture_Init_Flags result = TEXTURE_INIT_FLAGS_NONE;
	if (flags & OBJECT_INIT_FLAGS_GAMMA_CORRECT)
		result = result | TEXTURE_INIT_FLAGS_GAMMA_CORRECT;
	if (flags & OBJECT_INIT_FLAGS_NOFLIP)
		result = result | TEXTURE_INIT_FLAGS_NOFLIP;
	if (flags & OBJECT_INIT_FLAGS_GEN_MIPMAP)
		result = result | TEXTURE_INIT_FLAGS_GEN_MIPMAP;

	return result;
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
		parse_mtl(mtlPath, flags);

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
	assert(rootDir != "");

	std::ifstream file(filepath);
	std::string linebuf;

	unsigned int vnIndex = 0;
	unsigned int vtIndex = 0;
	unsigned int fIndex = 0; // points to 1 off the last index
	while (std::getline(file, linebuf))
	{
		// Trim trailing whitespace.
		if (linebuf.size() > 0) {
			linebuf = linebuf.substr(0, linebuf.find_last_not_of(" \t") + 1);
		}

		// Trim newline '\r\n' or '\n'
		if (linebuf.size() > 0) {
			if (linebuf[linebuf.size() - 1] == '\n')
				linebuf.erase(linebuf.size() - 1);
		}
		if (linebuf.size() > 0) {
			if (linebuf[linebuf.size() - 1] == '\r')
				linebuf.erase(linebuf.size() - 1);
		}

		// Skip if empty line.
		if (linebuf.empty()) {
			continue;
		}

		// Skip leading space.
		const char* token = linebuf.c_str();
		token += strspn(token, " \t");

		assert(token);
		if (token[0] == '\0') continue;  // empty line
		if (token[0] == '#') continue;  // comment line

		if (token[0] == 'v' && IS_SPACE(token[1]))
		{
			Vertex data;
			token += 2;
			sscanf_s(token, "%f %f %f", &data.position.x, &data.position.y, &data.position.z);
			vBuf.push_back(data);
			continue;
		}

		if (token[0] == 'v' && token[1] == 'n' && IS_SPACE(token[2]))
		{
			assert(vnIndex < vBuf.size());
			token += 3;
			sscanf_s(token, "%f %f %f", &vBuf[vnIndex].normal.x, &vBuf[vnIndex].normal.y, &vBuf[vnIndex].normal.z);
			++vnIndex;
			continue;
		}

		if (token[0] == 'v' && token[1] == 't' && IS_SPACE(token[2]))
		{
			assert(vtIndex < vBuf.size());
			token += 3;
			sscanf_s(token, "%f %f", &vBuf[vtIndex].texCoord.s, &vBuf[vtIndex].texCoord.t);
			++vtIndex;
			continue;
		}
	
		if (token[0] == 'f' && IS_SPACE(token[1]))
		{
			token += 2;
			while (token[0] != '\n' && token[0] != '\r' && token[0] != '\0')
			{
				unsigned int i;
				sscanf_s(token, "%ui", &i);
				iBuf.push_back(i - 1);

				size_t n = strspn(token, "/1234567890");
				token += n;
				n = strspn(token, " \t\r");
				token += n;
			}

			if (mat_ptrs.size() == 0) // if no material has been declared
			{
				mat_ptrs.emplace_back(-1, 0);
			}
			fIndex += 3;
			continue;
		}

		if (strncmp(token, "usemtl", 6) == 0 && IS_SPACE(token[6]))
		{
			char name[512];
			token += 7;
			sscanf_s(token, "%s", name, 512);
			if (mat_ptrs.size() != 0)
				(mat_ptrs.end() - 1)->m_count = fIndex - (mat_ptrs.end() - 1)->m_offset;
			mat_ptrs.emplace_back(-1, fIndex, std::string(name));
			continue;
		}

		if (strncmp(token, "mtllib", 6) == 0 && IS_SPACE(token[6]))
		{
			char name[512];
			token += 7;
			sscanf_s(token, "%s", name, 512);
			mtlPaths.push_back(rootDir + std::string(name));
			continue;
		}
	}
	(mat_ptrs.end() - 1)->m_count = fIndex - (mat_ptrs.end() - 1)->m_offset; // read last index	
	return mtlPaths;
}

void Object::parse_mtl(const std::string filepath, Object_Init_Flags flags)
// TODO: parse shading parameters in mtl file (Ka, Kd, Ks, Ns, Ni, d, Tf, illum)
{
	std::ifstream mtlFile(filepath);
	if (!mtlFile)
	{
		std::cout << "Failed to read " << filepath << std::endl;
		return;
	}
	std::string linebuf;
	std::string rootDir = calc_root_dir(filepath);
	while (std::getline(mtlFile, linebuf))
	{
		// Trim trailing whitespace.
		if (linebuf.size() > 0) {
			linebuf = linebuf.substr(0, linebuf.find_last_not_of(" \t") + 1);
		}

		// Trim newline '\r\n' or '\n'
		if (linebuf.size() > 0) {
			if (linebuf[linebuf.size() - 1] == '\n')
				linebuf.erase(linebuf.size() - 1);
		}
		if (linebuf.size() > 0) {
			if (linebuf[linebuf.size() - 1] == '\r')
				linebuf.erase(linebuf.size() - 1);
		}

		// Skip if empty line.
		if (linebuf.empty()) {
			continue;
		}

		// Skip leading space.
		const char* token = linebuf.c_str();
		token += strspn(token, " \t");

		assert(token);
		if (token[0] == '\0') continue;  // empty line
		if (token[0] == '#') continue;  // comment line

		if (strncmp(token, "newmtl", 6) == 0 && IS_SPACE(token[6]))
		{
			char name[512];
			token += 7;
			sscanf_s(token, "%s", name, 512);
			materials.push_back(Material(std::string(name)));
			continue;
		}
		
		if (strncmp(token, "map_Kd", 6) == 0 && IS_SPACE(token[6]) && (flags & OBJECT_INIT_FLAGS_GEN_TEXTURE))
		{
			char name[512];
			token += 7;
			sscanf_s(token, "%s", name, 512);
			(materials.end() - 1)->genTexture(rootDir + std::string(name), GL_REPEAT, cast(flags));
			continue;
		}

		if (strncmp(token, "map_Bump", 8) == 0 && IS_SPACE(token[8]) && (flags & OBJECT_INIT_FLAGS_GEN_NORMAL))
		{
			char name[512];
			token += 9;
			sscanf_s(token, "%s", name, 512);
			(materials.end() - 1)->genNormMap(rootDir + std::string(name));
			continue;
		}

		if (strncmp(token, "map_Ks", 6) == 0 && IS_SPACE(token[6]) && (flags & OBJECT_INIT_FLAGS_GEN_SPECULAR))
		{
			char name[512];
			token += 7;
			sscanf_s(token, "%s", name, 512);
			(materials.end() - 1)->genSpecMap(rootDir + std::string(name));
			continue;
		}
	}
}
// Tangents have attrib location = 3
// Algorithm described here: http://www.terathon.com/code/tangent.html
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
		// Calculate biTangent direction
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
			if (mat_ptr.m_material->hasSpecMap && (flags & DRAW_FLAGS_SPECULAR))
			{
				mat_ptr.m_material->m_SpecMap->Bind(2);
				shader.setInt("SpecMap", 2);
			}
		}
		GLCall(glDrawElements(GL_TRIANGLES, mat_ptr.m_count, GL_UNSIGNED_INT,
			(void*)(mat_ptr.m_offset * sizeof(unsigned int))));
	}
}