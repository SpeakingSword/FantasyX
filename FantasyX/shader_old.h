#ifndef SHADER_OLD_H
#define SHADER_OLD_H

#include <glad\glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Shader_Old
{
public:
	// 程序ID
	unsigned int ID;
	// 读取并构建着色器程序(使用顶点、片段着色器)
	Shader_Old(const char *vertexPath, const char *fragmentPath);
	// 读取并构建着色器程序(使用顶点、片段、几何着色器)
	Shader_Old(const char *vertexPath, const char *fragmentPath, const char *geomatryPath);
	// 启用着色器程序
	void use();
	// 设置着色器中uniform声明的boolean变量
	void setBool(const std::string &name, bool value) const;
	// 设置着色器中uniform声明的int变量
	void setInt(const std::string &name, int value) const;
	// 设置着色器中uniform声明的float变量
	void setFloat(const std::string &name, float value) const;
	// 设置着色器中uniform声明的mat4变量
	void setMat4(const std::string &name, glm::mat4 &value) const;
	// 设置着色器中uniform声明的vec3变量
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	// 使用标量设置着色器中uniform声明的vec3变量
	void setVec3(const std::string &name, float x, float y, float z) const;
};

#endif