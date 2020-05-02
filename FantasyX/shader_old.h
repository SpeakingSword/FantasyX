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
	// ����ID
	unsigned int ID;
	// ��ȡ��������ɫ������(ʹ�ö��㡢Ƭ����ɫ��)
	Shader_Old(const char *vertexPath, const char *fragmentPath);
	// ��ȡ��������ɫ������(ʹ�ö��㡢Ƭ�Ρ�������ɫ��)
	Shader_Old(const char *vertexPath, const char *fragmentPath, const char *geomatryPath);
	// ������ɫ������
	void use();
	// ������ɫ����uniform������boolean����
	void setBool(const std::string &name, bool value) const;
	// ������ɫ����uniform������int����
	void setInt(const std::string &name, int value) const;
	// ������ɫ����uniform������float����
	void setFloat(const std::string &name, float value) const;
	// ������ɫ����uniform������mat4����
	void setMat4(const std::string &name, glm::mat4 &value) const;
	// ������ɫ����uniform������vec3����
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	// ʹ�ñ���������ɫ����uniform������vec3����
	void setVec3(const std::string &name, float x, float y, float z) const;
};

#endif