#include "mesh_old.h"

Mesh_Old::Mesh_Old(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->defaultColor = glm::vec3(0.4f, 0.4f, 0.4f);

	setupMesh();
}

void Mesh_Old::Draw(Shader_Old shader)
{
	unsigned int diffuseN = 1;
	unsigned int specularN = 1;
	unsigned int normalN = 1;
	unsigned int reflectionN = 1;
	unsigned int displacementN = 1;
	unsigned int metallicN = 1;
	unsigned int roughnessN = 1;
	unsigned int aoN = 1;

	if (textures.size() > 0)		// 如果次材质有纹理的话
	{
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseN++);
			else if (name == "texture_specular")
				number = std::to_string(specularN++);
			else if (name == "texture_normal")
				number = std::to_string(normalN++);
			else if (name == "texture_reflection")
				number = std::to_string(reflectionN++);
			else if (name == "texture_displacement")
				number = std::to_string(displacementN++);
			else if (name == "texture_metallic")
				number = std::to_string(metallicN++);
			else if (name == "texture_roughness")
				number = std::to_string(roughnessN++);
			else if (name == "texture_ao")
				number = std::to_string(aoN++);

			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
	}

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh_Old::setupMesh()
{
	// 创建缓存对象
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 发送数据给缓存对象
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	/* 设置顶点属性指针 */
	// 顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// 顶点纹理坐标
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
	glEnableVertexAttribArray(1);
	// 顶点法线
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	// 顶点切线
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(3);
	// 顶点副切线
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
}

