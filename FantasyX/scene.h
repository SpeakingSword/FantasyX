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

    // ����ʹ����������˼�뱣��������Ⱦ��Ҫ������
    class Scene {
    public:
        string name;
        Renderer *renderer;

        Scene();
        GameObject *Start() const;
        GameObject *End() const;
        GameObject *Root() const;

        /*
          ��ʼ���������ݣ�Ϊ��Ⱦ��׼��
        */
        void Init();

        /*
          Ϊ������Ӷ���
        */
        void Add(GameObject *obj);
        void Add(GameObject *obj, GLuint index);

        /*
          ���³��������ݺ���Ϊ
        */
        void Update();

        /*
          ��ȾBSP���е�����
        */
        void Render();

        void Destroy();
    
    private:
        /*
          BSP������ʾ����������Ŀռ�˳����Z������Ϊǰ������Ϊ��Z���������Ļ��ֱ��ָ����Ļ�⣩
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
          ��νṹ������Ĭ������Ϊ�գ��Һ���Ϊ��
        */
        GameObject *root;
        ~Scene();

        /*
          �����㼶�ṹ����ʼ��
        */
        void TraverseInit(GameObject *obj);

        /*
          ������νṹ����������
        */
        void TraverseUpdate(GameObject *obj);

        /*
          ��������ӵ���Ӧ������
        */
        void AddTo(GameObject *obj);

        /*
          ��������ӵ��ƹ⼯��
        */
        void AddToLights(GameObject *obj);

        /*
          ��������ӵ��б�
        */
        void AddToList(GameObject *obj, list<GameObject *> &mList);

        /*
          ����Ϸ�������BSP��
        */
        void AddToBSPTree(GameObject *obj, BiTree<GameObject *> &tree, BiTreeNode<GameObject *> *treeNode);
    }; 
}

#endif // !SCENE_H
