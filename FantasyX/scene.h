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

    // 场景使用面向对象的思想保存所有渲染需要的数据
    class Scene {
    public:
        string name;
        Renderer *renderer;

        Scene();
        GameObject *Start() const;
        GameObject *End() const;
        GameObject *Root() const;

        /*
          初始化场景数据，为渲染做准备
        */
        void Init();

        /*
          为场景添加对象
        */
        void Add(GameObject *obj);
        void Add(GameObject *obj, GLuint index);

        /*
          更新场景的数据和行为
        */
        void Update();

        /*
          渲染BSP树中的物体
        */
        void Render();

        void Destroy();
    
    private:
        /*
          BSP树，表示场景中物体的空间顺序，以Z轴正向为前，负向为后（Z轴与电脑屏幕垂直，指向屏幕外）
        */
        BiTree<GameObject *> opaqueTree;
        BiTree<GameObject *> transparentTree;
        list<GameObject *> dirLights;
        list<GameObject *> pointLights;
        list<GameObject *> spotLights;
        list<GameObject *> cameras;
        stack<Matrix4x4> modelMatrixStack;
        Matrix4x4 modelMatrix;

        /*
          层次结构树根，默认左孩子为空，右孩子为空
        */
        GameObject *root;
        ~Scene();

        /*
          遍历层级结构树初始化
        */
        void TraverseInit(GameObject *obj);

        /*
          遍历层次结构树更新数据
        */
        void TraverseUpdate(GameObject *obj);

        /*
          将对象添加到对应的容器
        */
        void AddTo(GameObject *obj);

        /*
          将对象添加到灯光集合
        */
        void AddToLights(GameObject *obj);

        /*
          将对象添加到列表
        */
        void AddToList(GameObject *obj, list<GameObject *> &mList);

        /*
          将游戏对象插入BSP树
        */
        void AddToBSPTree(GameObject *obj, BiTree<GameObject *> &tree, BiTreeNode<GameObject *> *treeNode);
    }; 
}

#endif // !SCENE_H
