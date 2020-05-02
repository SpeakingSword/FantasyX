#include "shader.h"

using namespace fx;

#pragma region Shader
void Shader::Bind()
{
    glUseProgram(ID);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetBool(const GLchar *name, GLboolean value) const
{
    glUniform1i(glGetUniformLocation(ID, name), (GLint)value);
}

void Shader::SetInt(const GLchar *name, GLint value) const
{
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetFloat(const GLchar *name, GLfloat value) const
{
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetMat4(const GLchar *name, const Matrix4x4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVec3(const GLchar *name, const Vector3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::SetVec3(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) const
{
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}


GLuint Shader::CreateShaderProgram(const GLchar *vertexShaderCode, const GLchar *fragmentShaderCode)
{
    // 编译着色器
    GLuint vertex, fragment, shaderProgramID;
    GLint success;
    GLchar infoLog[512];

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);
    // 打印可能的编译错误
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: vertex shader compilation failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    // 打印可能的编译错误
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: fragment shader compilation failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 着色器程序
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertex);
    glAttachShader(shaderProgramID, fragment);
    glLinkProgram(shaderProgramID);
    // 打印可能的链接错误
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: shader program linking failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 删除着色器
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgramID;
}

GLuint Shader::CreateShaderProgram(const GLchar *vertexShaderCode, const GLchar *geomatryShaderCode, const GLchar *fragmentShaderCode)
{
    // 编译着色器
    GLuint vertex, fragment, geomatry, shaderProgramID;
    GLint success;
    GLchar infoLog[512];

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);
    // 打印可能的编译错误
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: vertex shader compilation failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 几何着色器
    geomatry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geomatry, 1, &geomatryShaderCode, NULL);
    glCompileShader(geomatry);
    // 打印可能的编译错误
    glGetShaderiv(geomatry, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geomatry, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: geomatry shader compilation failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    // 打印可能的编译错误
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: fragment shader compilation failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 着色器程序
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertex);
    glAttachShader(shaderProgramID, geomatry);
    glAttachShader(shaderProgramID, fragment);
    glLinkProgram(shaderProgramID);
    // 打印可能的链接错误
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        std::cout << "ENGIN CORE::ERROR::SHADER: shader program linking failed !\n" << infoLog << std::endl;
        exit(0);
    }

    // 删除着色器
    glDeleteShader(vertex);
    glDeleteShader(geomatry);
    glDeleteShader(fragment);

    return shaderProgramID;
}
#pragma endregion

#pragma region LegacySimpleShader
LegacySimpleShader *LegacySimpleShader::_instance = nullptr;

void LegacySimpleShader::InitShader()
{
    name = "LegacySimpleShader";
    ResourceManager *res = ResourceManager::GetInstance();
    const GLchar *vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Standard.vert").c_str());
    const GLchar *fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "LegacySimple.frag").c_str());
    ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
}
#pragma endregion

#pragma region LegacyStandardShader
LegacyStandardShader *LegacyStandardShader::_instance = nullptr;

void LegacyStandardShader::InitShader()
{
    name = "LegacyStandardShader";
    ResourceManager *res = ResourceManager::GetInstance();
    const GLchar *vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Standard.vert").c_str());
    const GLchar *fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "LegacyStandard.frag").c_str());
    ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
}
#pragma endregion
