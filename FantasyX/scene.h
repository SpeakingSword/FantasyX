#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <list>
#include <stack>

#include "renderer.h"
#include "gameobject.h"
#include "mesh.h"
#include "bitree.h"
#include "component.h"
#include "clocker.h"

#include <glad\glad.h>

using namespace std;

namespace fx {

    class Scene {
    public:
        string name;
        Renderer *renderer;

        Scene();
        GameObject *Start() const;
        GameObject *End() const;
        GameObject *Root() const;

        void Init();
        void Add(GameObject *obj);
        void Add(GameObject *obj, GLuint index);
        void Update();
        void Render();
        void Destroy();
        const BiTree<GameObject *> &GetOpaqueTree();
        const BiTree<GameObject *> &GetTransparentTree();
        static void InorderPrintBSP(BiTreeNode<GameObject *> *treeNode);
    
    private:
        BiTree<GameObject *> opaqueTree;
        BiTree<GameObject *> transparentTree;
        list<GameObject *> dirLights;
        list<GameObject *> pointLights;
        list<GameObject *> spotLights;
        list<GameObject *> cameras;
        stack<Matrix4x4> modelMatrixStack;
        Matrix4x4 modelMatrix;

        GameObject *root;
        ~Scene();

        void TraverseInit(GameObject *obj);
        void TraverseUpdate(GameObject *obj);
        void AddTo(GameObject *obj);
        void AddToLights(GameObject *obj);
        void AddToList(GameObject *obj, list<GameObject *> &mList);
        void AddToBSPTree(GameObject *obj, BiTree<GameObject *> &tree, BiTreeNode<GameObject *> *treeNode);
    }; 
}

#endif // !SCENE_H
