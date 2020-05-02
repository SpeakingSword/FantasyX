#include "model.h"
#include <stb_image.h>



void Model::LoadModel(string const &path)
{
	// ���س�����ģ���ļ�����ĳ�����
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// �жϳ����Ƿ���ȷ������
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// ���ģ�����ڵ�Ŀ¼
	directory = path.substr(0, path.find_last_of('/'));

	// �������������нڵ�
	processNode(scene->mRootNode, scene);

	std::cout << "Loading " << path << " success." << std::endl;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	// ������ǰ�ڵ����������
    vector<string>::iterator iter;
    for (iter = stack.begin(); iter != stack.end(); iter++)
    {
        std::cout << iter->c_str();
    }

    std::cout << node->mName.C_Str() << " " << node->mNumMeshes << std::endl;
    if (node->mNumMeshes > 0)
        stack.push_back("-");
    
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "-" << mesh->mName.C_Str() << std::endl;
		meshes.push_back(processMesh(mesh, scene));
	}

	// ����������ǰ�ڵ���ӽڵ�
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

    if (!stack.empty())
        stack.pop_back();
}

Mesh_Old Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// �������������Ķ�������
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// ������ǰ��������ж������ݣ���������λ�á����ߡ���������ȵ�
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 mVector;
		// λ��
		mVector.x = mesh->mVertices[i].x;
		mVector.y = mesh->mVertices[i].y;
		mVector.z = mesh->mVertices[i].z;
		vertex.Position = mVector;
		// ����
		mVector.x = mesh->mNormals[i].x;
		mVector.y = mesh->mNormals[i].y;
		mVector.z = mesh->mNormals[i].z;
		vertex.Normal = mVector;
		// ��������
		if (mesh->mTextureCoords[0])	// ������������������
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoord = vec;
		}
		else
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);

		// ����
		mVector.x = mesh->mTangents[i].x;
		mVector.y = mesh->mTangents[i].y;
		mVector.z = mesh->mTangents[i].z;
		vertex.Tangent = mVector;
		// ������
		mVector.x = mesh->mBitangents[i].x;
		mVector.y = mesh->mBitangents[i].y;
		mVector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = mVector;
		// ���һ����������
		vertices.push_back(vertex);
	}

	// �������ж��������������ݣ�
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// �������������
	if (mesh->mMaterialIndex >= 0)		// ���������ʹ��������Ļ�
	{
		// �������Ĳ���
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		// ������������ͼ
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// ���ظ߹���ͼ
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// ���ط�����ͼ
		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// ���ط�����ͼ
		vector<Texture> reflectionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
		textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
		// �����û���ͼ
		vector<Texture> displacementMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_displacement");
		textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());
	}

	return Mesh_Old(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;				// �����Ƿ񱻼��ع��ı�־
		for (unsigned int j = 0; j < texture_loaded.size(); j++)
		{
			if (std::strcmp(texture_loaded[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(texture_loaded[j]);
				skip = true;
				break;
			}
		}
		
		if (!skip)
		{
			Texture texture;
			if (typeName == "texture_diffuse")
				texture.id = TextureFromFile(str.C_Str(), this->directory, true);	// ������ͼ����GAMMAУ��
			else
				texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			texture_loaded.push_back(texture);		// ��¼�������Ѿ������ع���
		}
	}

	return textures;
}

bool Model::AddTexture(Texture texture)
{
	for (unsigned int i = 0; i < texture_loaded.size(); i++)
	{
		if (texture_loaded[i].id == texture.id)
		{
			std::cout << "Texture id: " << texture.id << " type: " << texture.type << " already been loaded in Model!" << std::endl;
			return false;
		}
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].textures.push_back(texture);
	}

	return true;
}

// ��ʼ��һ��OpenGL��������������
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	//stbi_set_flip_vertically_on_load(true);
	int width, height, numComponent;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &numComponent, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (numComponent == 1)
			internalFormat = dataFormat = GL_RED;
		else if (numComponent == 3)
		{
			internalFormat = gamma ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (numComponent == 4) 
		{
			internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
