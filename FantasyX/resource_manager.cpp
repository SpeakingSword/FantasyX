#include "resource_manager.h"
#include <stb_image.h>

using namespace fx;

ResourceManager *ResourceManager::_instance = nullptr;

GameObject *ResourceManager::LoadModel(const GLchar *path)
{
    // 遍历模型数据，根据节点层序创建游戏对象，初始化游戏对象（添加Transform, MeshFilter, Render等组件)，再返回父游戏对象的指针
    return nullptr;
}

Texture ResourceManager::LoadTexture2D(const GLchar *path, const GLchar *type, bool gamma)
{
    Texture tex;
    GLuint textureID;
    glGenTextures(1, &textureID);

    GLint width, height, numChannel;
    stbi_set_flip_vertically_on_load(true);
    GLboolean *data = stbi_load(path, &width, &height, &numChannel, 0);
    if (data)
    {

    }

    return tex;
}

const GLchar *ResourceManager::GetAppDir()
{
    if (appDir.empty())
    {
        GLchar buffer[100];
        appDir = _getcwd(buffer, sizeof(buffer));
    }

    return appDir.c_str();
}

const GLchar *ResourceManager::GetShaderDir()
{
    return "\\Shaders\\";
}

const GLchar *ResourceManager::GetImageDir()
{
    return "\\Images\\";
}

const GLchar *ResourceManager::GetFileString(const GLchar *path)
{
    std::string fileString;
    std::ifstream inputFile;

    inputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        inputFile.open(path);
        std::stringstream fileStream;
        fileStream << inputFile.rdbuf();
        inputFile.close();
        fileString = fileStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ENGIN CORE::ERROR::SHADER: File not successfully read !" << std::endl;
        std::cout << path << std::endl;
    }

    return fileString.c_str();
}