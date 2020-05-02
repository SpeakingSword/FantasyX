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
	glm::vec3 Position;		// 顶点位置
	glm::vec3 Normal;		// 顶点法线
	glm::vec2 TexCoord;		// 顶点纹理坐标
	glm::vec3 Tangent;		// 顶点切线
	glm::vec3 Bitangent;	// 顶点副切线
};

struct Texture {
	unsigned int id;		// 纹理ID
	std::string type;		// 纹理类型
	std::string path;		// 纹理路径
};

class Mesh_Old {
public:
	/* 顶点数据 */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;
	glm::vec3 defaultColor;

	// 使用顶点数据、顶点索引、材质纹理构造网格对象	
    Mesh_Old(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	// 绘制网格
	void Draw(Shader_Old shader);
private:
	/* 渲染数据 */
	unsigned int VBO, EBO;
	// 初始化网格数据
	void setupMesh();
};

#endif // !MESH_OLD_H


