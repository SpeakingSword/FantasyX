#include "shader_old.h"

Shader_Old::Shader_Old(const char *vertexPath, const char *fragmentPath)
{
	// ���ļ�·���л�ȡ����/Ƭ����ɫ��
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ��֤ifstream��������׳��쳣
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// ���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		// ��ȡ�ļ��Ļ������ݵ���������
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// �ر��ļ�������
		vShaderFile.close();
		fShaderFile.close();
		// ת����������string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		std::cout << vertexPath << std::endl;
		std::cout << fragmentPath << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();

	// ������ɫ��
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// ��ӡ���ܵı������
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// ��ӡ���ܵı������
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// ��ɫ������
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// ��ӡ���ܵ����Ӵ���
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader_Old::Shader_Old(const char *vertexPath, const char *fragmentPath, const char *geomatryPath)
{
	// ���ļ�·���л�ȡ����/Ƭ����ɫ��
	std::string vertexCode;
	std::string fragmentCode;
	std::string geomatryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ��֤ifstream��������׳��쳣
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// ���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		gShaderFile.open(geomatryPath);
		// ��ȡ�ļ��Ļ������ݵ���������
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		// �ر��ļ�������
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();
		// ת����������string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geomatryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		std::cout << vertexPath << std::endl;
		std::cout << geomatryPath << std::endl;
		std::cout << fragmentPath << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	const char *gShaderCode = geomatryCode.c_str();

	// ������ɫ��
	unsigned int vertex, fragment, geomatry;
	int success;
	char infoLog[512];

	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// ��ӡ���ܵı������
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// ��ӡ���ܵı������
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// ������ɫ��
	geomatry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geomatry, 1, &gShaderCode, NULL);
	glCompileShader(geomatry);
	// ��ӡ���ܵı������
	glGetShaderiv(geomatry, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geomatry, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMATRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// ��ɫ������
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glAttachShader(ID, geomatry);
	glLinkProgram(ID);
	// ��ӡ���ܵ����Ӵ���
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geomatry);
}

void Shader_Old::use()
{
	glUseProgram(ID);
}

void Shader_Old::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader_Old::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader_Old::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader_Old::setMat4(const std::string &name, glm::mat4 &value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader_Old::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader_Old::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}