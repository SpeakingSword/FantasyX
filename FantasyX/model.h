#ifndef MODEL_H
#define MODEL_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "mesh_old.h"
#include "shader_old.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model {
public:
	/* ģ������ */
	vector<Texture> texture_loaded;		// �������м��ع�������
	vector<Mesh_Old> meshes;				// ģ�͵���������
    vector<string> stack;
	string directory;					// ģ�����ڵ�Ŀ¼
	bool gammaCorrection;

	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		LoadModel(path);
	}

	// ����ģ��
	void Draw(Shader_Old shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	bool AddTexture(Texture texture);

private:
	// ����ģ��
	void LoadModel(string const &path);
	// ���������ڵ�
	void  processNode(aiNode *node, const aiScene *scene);
	// ������������
	Mesh_Old processMesh(aiMesh *mesh, const aiScene *scene);
	// ��������ָ�����͵�������ͼ
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif

