#include "resource_manager.h"
#include "material.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace fx;

ResourceManager *ResourceManager::_instance = nullptr;

GameObject *ResourceManager::LoadModel(const GLchar *path)
{
    // 遍历模型数据，根据节点层序创建游戏对象，初始化游戏对象（添加Transform, MeshFilter, Render等组件)，再返回父游戏对象的指针
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ENGIN CORE::ERROR::RESOURCE MANAGER: " << importer.GetErrorString() << endl;
        return nullptr;
    }

    GameObject *rootObj = new GameObject();

    ProcessNode(scene->mRootNode, scene, rootObj, path);

    return rootObj;
}

void ResourceManager::ProcessNode(aiNode *node, const aiScene *scene, GameObject *obj, const GLchar *path)
{
    // 首先，根据当前节点信息初始化Gameobject
    // 第二，如果当前节点有子节点，则新建GameObject，继续遍历子节点

    // 设置GameObject的名称，并设置为不透明
    obj->name = node->mName.C_Str();
    obj->tag = GT_OPAQUE;

    // 给GameObject添加 Render 组件
    Render *render = new Render();
    // 设置默认材质
    PBRSimpleMaterial *PBRSimple = new PBRSimpleMaterial();
    render->material = PBRSimple;
    // 如果当前节点存有 mesh 则添加
    if (node->mNumMeshes > 0)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[0]];
        render->mesh = ProcessMesh(mesh, path);
    }
    obj->AddComponent(render);

    for (GLuint i = 0; i < node->mNumChildren; i++)
    {
        GameObject *obj_child = new GameObject();
        ProcessNode(node->mChildren[i], scene, obj_child, path);
        obj->Add(obj_child);
    }
}

PolygonMesh *ResourceManager::ProcessMesh(aiMesh *mesh, const GLchar *path)
{
    // 网格对象待填满的顶点数据
    vector<Vertex> vertices;
    vector<GLuint> indices;

    // 遍历当前网格的所有顶点数据，包括顶点位置、法线、纹理坐标等等
    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        Vector3 mVector;
        // 位置
        mVector.x = mesh->mVertices[i].x;
        mVector.y = mesh->mVertices[i].y;
        mVector.z = mesh->mVertices[i].z;
        vertex.position = mVector;
        // 法线
        mVector.x = mesh->mNormals[i].x;
        mVector.y = mesh->mNormals[i].y;
        mVector.z = mesh->mNormals[i].z;
        vertex.normal = mVector;
        // 纹理坐标
        if (mesh->mTextureCoords[0])	// 如果顶点存在纹理坐标
        {
            Vector2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texcoord = vec;
        }
        else
            vertex.texcoord = Vector2(0.0f, 0.0f);

        // 切线
        mVector.x = mesh->mTangents[i].x;
        mVector.y = mesh->mTangents[i].y;
        mVector.z = mesh->mTangents[i].z;
        vertex.tangent = mVector;
        // 副切线
        mVector.x = mesh->mBitangents[i].x;
        mVector.y = mesh->mBitangents[i].y;
        mVector.z = mesh->mBitangents[i].z;
        vertex.bitangent = mVector;
        // 添加一个顶点数据
        vertices.push_back(vertex);
    }

    // 遍历所有顶点索引（面数据）
    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    PolygonMesh *polygon = new PolygonMesh(vertices, indices, path);
    polygon->name = mesh->mName.C_Str();
    return polygon;
}

Texture ResourceManager::LoadTexture2D(const GLchar *path, const GLchar *type, bool gamma)
{
    Texture tex;
    GLuint textureID;
    glGenTextures(1, &textureID);

    GLint width, height, numChannel;
    // stbi_set_flip_vertically_on_load(true);
    GLboolean *data = stbi_load(path, &width, &height, &numChannel, 0);
    if (data)
    {
        GLenum interFormat;
        GLenum dataFormat;
        if (numChannel == 1)
            interFormat = dataFormat = GL_RED;
        else if (numChannel == 3)
        {
            interFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (numChannel == 4)
        {
            interFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, interFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        tex.id = textureID;
        tex.path = path;
        tex.type = type;

        std::cout << "ENGIN CORE::RESOURCE MANAGER: " << path << " successfully loaded ... " << std::endl;
    }
    else
    {
        std::cout << "ENGIN CORE::ERROR::RESOURCE MANAGER: Texture failed to load: " << path << std::endl;
    }

    stbi_image_free(data);
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

GLchar *ResourceManager::GetFileString(const GLchar *path)
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
        std::cout << "ENGIN CORE::ERROR::REOURCE MANAGER: File not successfully read !" << std::endl;
        std::cout << path << std::endl;
    }

    GLchar *str = new GLchar[fileString.size() + 1];
    strcpy_s(str, fileString.size() + 1, fileString.c_str());

    return str;
}