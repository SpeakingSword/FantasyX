#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <unordered_map>
#include <direct.h>
#include <glad\glad.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "mesh.h"
#include "texture.h"
#include "gameobject.h"
#include "component.h"

using namespace std;

namespace fx {

    class ResourceManager {
    private:
        vector<PolygonMesh *> meshes;    // 记录 PolygonMesh 的引用
        vector<Texture> textures;       // 记录 Texture 的引用
        string appDir;
        string shaderDir;
        string imageDir;

        // 使用哈希表防止重复导入网格和贴图等数据
        // string 保存的是文件的路径
        unordered_map<string, GLuint> store;

        static ResourceManager *_instance;
        ResourceManager()
        {
            cout << "ENGIN CORE: ResourceManager created ... " << endl;
        }

        class GC {
        public:
            GC() { cout << "ENGIN CORE: ResourceManager GC created ... " << endl; }
            ~GC()
            {
                cout << "ENGIN CORE: ResourceManager GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "ENGIN CORE: ResourceManager destroyed ... " << endl;
                }
            }
        };
        static GC gc;
   
    public:
        static ResourceManager *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new ResourceManager();
            }
            return _instance;
        }
    
        GameObject *LoadModel(const GLchar *path);
        Texture LoadTexture2D(const GLchar *path, const GLchar *type, bool gamma);
        const GLchar *GetAppDir();
        const GLchar *GetShaderDir();
        const GLchar *GetImageDir();
        GLchar *GetFileString(const GLchar *path);
        void ProcessNode(aiNode *node, const aiScene *scene, GameObject *obj, const GLchar *path);
        PolygonMesh *ProcessMesh(aiMesh *mesh, const GLchar *path);
    };
}

#endif // !RESOURCE_MANAGER_H