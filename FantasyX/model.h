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
	/* 模型数据 */
	vector<Texture> texture_loaded;		// 保存所有加载过的纹理
	vector<Mesh_Old> meshes;				// 模型的所有网格
    vector<string> stack;
	string directory;					// 模型所在的目录
	bool gammaCorrection;

	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		LoadModel(path);
	}

	// 绘制模型
	void Draw(Shader_Old shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	bool AddTexture(Texture texture);

private:
	// 加载模型
	void LoadModel(string const &path);
	// 遍历场景节点
	void  processNode(aiNode *node, const aiScene *scene);
	// 遍历场景网格
	Mesh_Old processMesh(aiMesh *mesh, const aiScene *scene);
	// 返回所有指定类型的纹理贴图
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif

