﻿#include "scene.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

namespace fx {

#pragma region Scene
    Scene::Scene()
    {
        this->name = "untitle";
        this->renderer = nullptr;

        // 初始化层级结构树（K叉树）,创建根节点
        this->root = new GameObject();
        fx::Render* render = new fx::Render();
        this->root->AddComponent(render);

        opaqueTree.root = new BiTreeNode<GameObject*>(this->root);
        transparentTree.root = opaqueTree.root;

        std::cout << "ENGIN CORE: Scene created ... " << std::endl;
    }

    Scene::~Scene()
    {
        std::cout << "ENGIN CORE: Scene destroyed ... " << std::endl;
    }

    void Scene::Destroy()
    {
        this->~Scene();
    }

    const BiTree<GameObject*>& fx::Scene::GetOpaqueTree()
    {
        return opaqueTree;
    }

    const BiTree<GameObject*>& fx::Scene::GetTransparentTree()
    {
        return transparentTree;
    }

    GameObject* Scene::Start() const
    {
        return this->root->child;
    }

    GameObject* Scene::End() const
    {
        GameObject* p = this->root->child;
        while (p->sibling != nullptr)
        {
            p = p->sibling;
        }

        return p;
    }

    GameObject* Scene::Root() const
    {
        return root;
    }

    void Scene::Init()
    {
        TraverseInit(root);
    }

    void Scene::TraverseInit(GameObject* obj)
    {
        if (obj == nullptr)
            return;

        obj->Init();
        AddTo(obj);

        if (obj->child != nullptr)
            TraverseInit(obj->child);

        if (obj->sibling != nullptr)
            TraverseInit(obj->sibling);

    }

    void Scene::TraverseUpdate(GameObject* obj)
    {
        // 先序遍历层次结构树
        // 如果当前节点为空则返回
        if (obj == nullptr)
            return;

        // 先将父对象的 modelMatrix 压入栈
        modelMatrixStack.push(modelMatrix);

        // 更新当前对象 transform 的 modelMatrix
        modelMatrix = glm::translate(modelMatrix, obj->transform->position);
        // 旋转顺序为 y->x->z
        modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.z), Vector3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, obj->transform->scale);
        obj->transform->modelMatrix = this->modelMatrix;
        obj->Update();
        // 将对象放入对应的容器
        AddTo(obj);

        // 如果左孩子不为空遍历左孩子
        if (obj->child != nullptr)
            TraverseUpdate(obj->child);

        // 如果右兄弟不为空遍历右兄弟，但遍历之前先将 modelMatrix 恢复到父对象的
        modelMatrix = modelMatrixStack.top();
        modelMatrixStack.pop();
        if (obj->sibling != nullptr)
            TraverseUpdate(obj->sibling);
    }

    void Scene::AddTo(GameObject* obj)
    {
        switch (obj->tag)
        {
        case GT_OPAQUE:
            AddToBSPTree(obj, opaqueTree, opaqueTree.root);
            break;
        case GT_TRANSPARENT:
            AddToBSPTree(obj, transparentTree, opaqueTree.root);
            break;
        case GT_LIGHT:
            AddToLights(obj);
            break;
        case GT_CAMERA:
            AddToList(obj, cameras);
        default:
            break;
        }
    }

    void Scene::AddToLights(GameObject* obj)
    {
        Lighting* light = (Lighting*)obj->GetComponent("Light");
        const GLchar* type = light->GetType();
        if (strcmp(type, "DirLight") == 0)
        {
            AddToList(obj, dirLights);
        }
        else if (strcmp(type, "PointLight") == 0)
        {
            AddToList(obj, pointLights);
        }
        else if (strcmp(type, "SpotLight") == 0)
        {
            AddToList(obj, spotLights);
        }
    }

    void Scene::AddToList(GameObject* obj, list<GameObject*>& mList)
    {
        mList.push_back(obj);
    }

    void Scene::AddToBSPTree(GameObject* obj, BiTree<GameObject*>& tree, BiTreeNode<GameObject*>* treeNode)
    {
        // 如果根节点为空，直接将对象插入根节点
        if (tree.root == nullptr)
        {
            tree.root = new BiTreeNode<GameObject*>(obj);
            return;
        }

        // 如果根节点不为空，获取当前节点保存的对象和目标对象的世界位置
        Vector3 nodeObjPosition = treeNode->data->transform->worldPos;
        Vector3 targetObjPosition = obj->transform->worldPos;

        // 如果目标对象在当前节点保存的对象前面
        if (targetObjPosition.z > nodeObjPosition.z)
        {
            if (tree.insert_left(treeNode, obj))
                return;
            else
            {
                // 否则继续遍历左节点
                AddToBSPTree(obj, tree, treeNode->left);
            }
        }
        else    // 如果相等或者在后面
        {
            if (tree.insert_right(treeNode, obj))
                return;
            else
            {
                AddToBSPTree(obj, tree, treeNode->right);
            }
        }

    }

    // 将对象添加到树根的孩子队列之后
    void Scene::Add(GameObject* obj)
    {
        // 首先判断树根左孩子，如果为空，则直接添加。
        // 不为空则获得树根左孩子，将对象添加为树根左孩子的最右兄弟。
        GameObject* root_child = this->root->child;
        if (root_child == nullptr)
        {
            this->root->child = obj;
        }
        else
        {
            GameObject* p = this->root->child;
            while (p->sibling != nullptr)
            {
                p = p->sibling;
            }
            // 此时 p 指向最右的对象
            p->sibling = obj;
        }
    }

    // 将对象添加到树根孩子队列的指定位置
    void Scene::Add(GameObject* obj, GLuint index)
    {
        GameObject* root_child = this->root->child;
        if (root_child == nullptr)
        {
            this->root->child = obj;
        }
        else
        {
            if (index == 0 || index == 1)
            {
                obj->sibling = root_child;
                this->root->child = obj;
            }

            GameObject* p = root_child;
            GameObject* last;
            GLuint i = 1;
            while (p->sibling != nullptr)
            {
                last = p;
                p = p->sibling;
                i++;

                if (i == index)
                {
                    obj->sibling = p;
                    last->sibling = obj;
                }
            }

            if (index > i)
                p->sibling = obj;

        }
    }

    void Scene::Update()
    {
        opaqueTree.remove(opaqueTree.root->left);
        opaqueTree.remove(opaqueTree.root->right);
        transparentTree.remove(transparentTree.root->left);
        transparentTree.remove(transparentTree.root->right);
        dirLights.clear();
        pointLights.clear();
        spotLights.clear();
        cameras.clear();

        TraverseUpdate(root);
    }

    void Scene::Render()
    {
        if (renderer != nullptr)
        {
            SceneData d;
            d.opaqueTree = &opaqueTree;
            d.transparentTree = &transparentTree;
            d.dirLights = &dirLights;
            d.pointLights = &pointLights;
            d.spotLights = &spotLights;
            d.cameras = &cameras;
            renderer->RenderScene(d);
        }
        else
        {
            std::cout << "ENGIN CORE::WARNNING: Scene renderer not set yet ! " << std::endl;
        }

    }
    void fx::Scene::InorderPrintBSP(BiTreeNode<GameObject*>* treeNode)
    {
        if (treeNode == nullptr)
            return;

        InorderPrintBSP(treeNode->left);

        if (treeNode->data != nullptr)
        {
            treeNode->data->PrintName();
        }

        InorderPrintBSP(treeNode->right);
    }

#pragma endregion

}