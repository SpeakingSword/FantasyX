#ifndef MESH_OLD_H
#define MESH_OLD_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "shader_old.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


struct Vertex {
	glm::vec3 Position;		// ����λ��
	glm::vec3 Normal;		// ���㷨��
	glm::vec2 TexCoord;		// ������������
	glm::vec3 Tangent;		// ��������
	glm::vec3 Bitangent;	// ���㸱����
};

struct Texture {
	unsigned int id;		// ����ID
	std::string type;		// ��������
	std::string path;		// ����·��
};

class Mesh_Old {
public:
	/* �������� */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;
	glm::vec3 defaultColor;

	// ʹ�ö������ݡ����������������������������	
    Mesh_Old(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	// ��������
	void Draw(Shader_Old shader);
private:
	/* ��Ⱦ���� */
	unsigned int VBO, EBO;
	// ��ʼ����������
	void setupMesh();
};

#endif // !MESH_OLD_H


