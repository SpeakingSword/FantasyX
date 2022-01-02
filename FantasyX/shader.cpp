#include "shader.h"

namespace fx {

#pragma region Shader
    void Shader::Bind()
    {
        glUseProgram(ID);
    }

    void Shader::Unbind()
    {
        glUseProgram(0);
    }

    void Shader::SetBool(const GLchar* name, GLboolean value) const
    {
        glUniform1i(glGetUniformLocation(ID, name), (GLint)value);
    }

    void Shader::SetInt(const GLchar* name, GLint value) const
    {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetFloat(const GLchar* name, GLfloat value) const
    {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetMat4(const GLchar* name, const Matrix4x4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetVec3(const GLchar* name, const Vector3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
    }

    void Shader::SetVec4(const GLchar* name, const Vector4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
    }

    void Shader::SetVec3(const GLchar* name, GLfloat x, GLfloat y, GLfloat z) const
    {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }


    GLuint Shader::CreateShaderProgram(const GLchar* vertexShaderCode, const GLchar* fragmentShaderCode)
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
            system("pause");
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
            system("pause");
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
            system("pause");
        }

        // 删除着色器
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return shaderProgramID;
    }

    GLuint Shader::CreateShaderProgram(const GLchar* vertexShaderCode, const GLchar* geomatryShaderCode, const GLchar* fragmentShaderCode)
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
            system("pause");
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
            system("pause");
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
            system("pause");
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
            system("pause");
        }

        // 删除着色器
        glDeleteShader(vertex);
        glDeleteShader(geomatry);
        glDeleteShader(fragment);

        return shaderProgramID;
    }
#pragma endregion

#pragma region LegacySimpleShader
    LegacySimpleShader* LegacySimpleShader::_instance = nullptr;

    void LegacySimpleShader::InitShader()
    {
        name = "LegacySimpleShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Standard.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "LegacySimple.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
    }
#pragma endregion

#pragma region LegacyStandardShader
    LegacyStandardShader* LegacyStandardShader::_instance = nullptr;

    void LegacyStandardShader::InitShader()
    {
        name = "LegacyStandardShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Standard.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "LegacyStandard.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
    }
#pragma endregion

#pragma region PBRSimpleShader
    PBRSimpleShader* PBRSimpleShader::_instance = nullptr;

    void PBRSimpleShader::InitShader()
    {
        name = "PBRSimpleShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PBRSimple.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PBRSimple.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 绑定着色器共享内存变量
        GLuint SharedMatrcesBlock = glGetUniformBlockIndex(ID, "SharedMatrices");
        GLuint SharedCameraValuesBlock = glGetUniformBlockIndex(ID, "SharedCameraValues");
        glUniformBlockBinding(ID, SharedMatrcesBlock, 0);
        glUniformBlockBinding(ID, SharedCameraValuesBlock, 1);
    }
#pragma endregion

#pragma region PBRStandardShader
    PBRStandardShader* PBRStandardShader::_instance = nullptr;

    void PBRStandardShader::InitShader()
    {
        name = "PBRStandardShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PBRStandard.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PBRStandard.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 绑定着色器共享内存变量
        GLuint SharedMatrcesBlock = glGetUniformBlockIndex(ID, "SharedMatrices");
        GLuint SharedCameraValuesBlock = glGetUniformBlockIndex(ID, "SharedCameraValues");
        glUniformBlockBinding(ID, SharedMatrcesBlock, 0);
        glUniformBlockBinding(ID, SharedCameraValuesBlock, 1);
    }

#pragma endregion

#pragma region DisplayShader
    DisplayShader* DisplayShader::_instance = nullptr;

    void DisplayShader::InitShader()
    {
        name = "DisplayShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Display.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Display.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 特别的初始化步骤，针对特别处理着色器
        this->Bind();
        this->SetInt("display", 0);
        this->Unbind();
    }

#pragma endregion

#pragma region DirLightShadowShader
    DirLightShadowShader* DirLightShadowShader::_instance = nullptr;

    void DirLightShadowShader::InitShader()
    {
        name = "DirLightShadowShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "DirLightShadow.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "DirLightShadow.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
    }

#pragma endregion

#pragma region PBRLightingShader
    PBRLightingShader* PBRLightingShader::_instance = nullptr;

    void PBRLightingShader::InitShader()
    {
        name = "PBRLightingShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PBRLighting.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PBRLighting.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        this->Bind();
        this->SetInt("gPos", 0);
        this->SetInt("gDepth", 1);
        this->SetInt("gAlbedo", 2);
        this->SetInt("gMetallic", 3);
        this->SetInt("gNormal", 4);
        this->SetInt("gRoughness", 5);
        this->SetInt("gAo", 6);
        this->SetInt("irradianceMap", 7);
        this->SetInt("prefilterMap", 8);
        this->SetInt("brdfLutMap", 9);
        this->SetInt("dirLightShadowMap", 12);
        this->Unbind();
    }

#pragma endregion

#pragma region BrightCatchShader
    BrightCatchShader* BrightCatchShader::_instance = nullptr;

    void BrightCatchShader::InitShader()
    {
        name = "BrightCatchShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "BrightCatch.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "BrightCatch.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 特别的初始化步骤，针对特别处理着色器
        this->Bind();
        this->SetInt("lightMap", 0);
        this->Unbind();
    }

#pragma endregion

#pragma region GaussianBlurShader
    GaussianBlurShader* GaussianBlurShader::_instance = nullptr;

    void GaussianBlurShader::InitShader()
    {
        name = "GaussianBlurShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "GaussianBlur.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "GaussianBlur.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 特别的初始化步骤，针对特别处理着色器
        this->Bind();
        this->SetInt("brightMap", 0);
        this->Unbind();
    }

#pragma endregion

#pragma region BloomShader

    BloomShader* BloomShader::_instance = nullptr;

    void BloomShader::InitShader()
    {
        name = "BloomShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Bloom.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Bloom.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 特别的初始化步骤，针对特别处理着色器
        this->Bind();
        this->SetInt("lightMap", 0);
        this->SetInt("blurMap", 1);
        this->Unbind();
    }

#pragma endregion

#pragma region SkyBoxShader
    SkyBoxShader* SkyBoxShader::_instance = nullptr;

    void SkyBoxShader::InitShader()
    {
        name = "SkyboxShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Skybox.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "Skybox.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        // 特别的初始化步骤，针对特别处理着色器
        this->Bind();
        this->SetInt("skybox", 0);
        this->Unbind();

        GLuint SharedMatrcesBlock = glGetUniformBlockIndex(ID, "SharedMatrices");
        glUniformBlockBinding(ID, SharedMatrcesBlock, 0);
    }
#pragma endregion

#pragma region HdrBoxShader
    HdrBoxShader* HdrBoxShader::_instance = nullptr;

    void HdrBoxShader::InitShader()
    {
        name = "HdrBoxShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "HdrBox.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "HdrBox.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        this->Bind();
        this->SetInt("hdrTexture", 0);
        this->Unbind();
    }

#pragma endregion

#pragma region IrradianceBoxShader
    IrradianceBoxShader* IrradianceBoxShader::_instance = nullptr;

    void IrradianceBoxShader::InitShader()
    {
        name = "IrradianceBoxShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "IrradianceBox.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "IrradianceBox.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        this->Bind();
        this->SetInt("hdr_cube_map", 0);
        this->Unbind();
    }

#pragma endregion

#pragma region PrefilterBoxShader
    PrefilterBoxShader* PrefilterBoxShader::_instance = nullptr;

    void PrefilterBoxShader::InitShader()
    {
        name = "PrefilterBoxShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PrefilterBox.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "PrefilterBox.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);

        this->Bind();
        this->SetInt("hdr_cube_map", 0);
        this->Unbind();
    }

#pragma endregion

#pragma region BRDFShader
    BRDFShader* BRDFShader::_instance = nullptr;

    void BRDFShader::InitShader()
    {
        name = "BRDFShader";
        ResourceManager* res = ResourceManager::GetInstance();
        const GLchar* vertexShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "BRDF.vert").c_str());
        const GLchar* fragmentShaderCode = res->GetFileString((string(res->GetAppDir()) + string(res->GetShaderDir()) + "BRDF.frag").c_str());
        ID = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
    }

#pragma endregion

}