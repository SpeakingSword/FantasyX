#include "scene.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace fx;

#pragma region Scene
Scene::Scene()
{
    this->name = "untitle";
    this->renderer = nullptr;

    // ��ʼ���㼶�ṹ����K������,�������ڵ�
    this->root = new GameObject();
    fx::Render *render = new fx::Render();
    this->root->AddComponent(render);

    opaqueTree.root = new BiTreeNode<GameObject *>(this->root);
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

GameObject *Scene::Start() const
{
    return this->root->child;
}

GameObject *Scene::End() const
{
    GameObject *p = this->root->child;
    while (p->sibling != nullptr)
    {
        p = p->sibling;
    }

    return p;
}

void Scene::Init()
{
    TraverseInit(root);
}

void Scene::TraverseInit(GameObject *obj)
{
    // ���������νṹ��
    // �����ǰ�ڵ�Ϊ���򷵻�
    if (obj == nullptr)
        return;

    // �Ƚ�������� modelMatrix ѹ��ջ
    modelMatrixStack.push(modelMatrix);

    // ���µ�ǰ���� transform �� modelMatrix
    modelMatrix = glm::translate(modelMatrix, obj->transform->position);
    // ��ת˳��Ϊ y->x->z
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.z), Vector3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, obj->transform->scale);
    obj->transform->modelMatrix = this->modelMatrix;
    obj->Update();
    // ����������Ӧ������
    AddTo(obj);

    // ������Ӳ�Ϊ�ձ�������
    if (obj->child != nullptr)
        TraverseInit(obj->child);
    
    // ������ֵܲ�Ϊ�ձ������ֵܣ�������֮ǰ�Ƚ� modelMatrix �ָ����������
    modelMatrix = modelMatrixStack.top();
    modelMatrixStack.pop();
    if (obj->sibling != nullptr)
        TraverseInit(obj->sibling);
    
}

void Scene::TraverseUpdate(GameObject *obj)
{
    // ���������νṹ��
    // �����ǰ�ڵ�Ϊ���򷵻�
    if (obj == nullptr)
        return;

    // �Ƚ�������� modelMatrix ѹ��ջ
    modelMatrixStack.push(modelMatrix);

    // ���µ�ǰ���� transform �� modelMatrix
    modelMatrix = glm::translate(modelMatrix, obj->transform->position);
    // ��ת˳��Ϊ y->x->z
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->transform->rotation.z), Vector3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, obj->transform->scale);
    obj->transform->modelMatrix = this->modelMatrix;
    obj->Update();
    // ����������Ӧ������
    // AddTo(obj);

    // ������Ӳ�Ϊ�ձ�������
    if (obj->child != nullptr)
        TraverseUpdate(obj->child);

    // ������ֵܲ�Ϊ�ձ������ֵܣ�������֮ǰ�Ƚ� modelMatrix �ָ����������
    modelMatrix = modelMatrixStack.top();
    modelMatrixStack.pop();
    if (obj->sibling != nullptr)
        TraverseUpdate(obj->sibling);
}

void Scene::AddTo(GameObject *obj)
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

void Scene::AddToLights(GameObject *obj)
{
    Lighting *light = (Lighting *)obj->GetComponent("Light");
    const GLchar *type = light->GetType();
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

void Scene::AddToList(GameObject *obj, list<GameObject *> &mList)
{
    mList.push_back(obj);
}

void Scene::AddToBSPTree(GameObject *obj, BiTree<GameObject *> &tree, BiTreeNode<GameObject *> *treeNode)
{
    // ������ڵ�Ϊ�գ�ֱ�ӽ����������ڵ�
    if (tree.root == nullptr)
    {
        tree.root = new BiTreeNode<GameObject *>(obj);
        return;
    }

    // ������ڵ㲻Ϊ�գ���ȡ��ǰ�ڵ㱣��Ķ����Ŀ����������λ��
    Vector3 nodeObjPosition = treeNode->data->transform->worldPos;
    Vector3 targetObjPosition = obj->transform->worldPos;

    // ���Ŀ������ڵ�ǰ�ڵ㱣��Ķ���ǰ��
    if (targetObjPosition.z > nodeObjPosition.z)
    {
        if (tree.insert_left(treeNode, obj))
            return;
        else
        {
            // �������������ڵ�
            AddToBSPTree(obj, tree, treeNode->left);
        }
    }
    else    // �����Ȼ����ں���
    {
        if (tree.insert_right(treeNode, obj))
            return;
        else
        {
            AddToBSPTree(obj, tree, treeNode->right);
        }
    }
    
}

// ��������ӵ������ĺ��Ӷ���֮��
void Scene::Add(GameObject *obj)
{
    // �����ж��������ӣ����Ϊ�գ���ֱ����ӡ�
    // ��Ϊ�������������ӣ����������Ϊ�������ӵ������ֵܡ�
    GameObject *root_child = this->root->child;
    if (root_child == nullptr)
    {
        this->root->child = obj;
    }
    else
    {
        GameObject *p = this->root->child;
        while (p->sibling != nullptr)
        {
            p = p->sibling;
        }
        // ��ʱ p ָ�����ҵĶ���
        p->sibling = obj;
    }
}

// ��������ӵ��������Ӷ��е�ָ��λ��
void Scene::Add(GameObject *obj, GLuint index)
{
    GameObject *root_child = this->root->child;
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
       
        GameObject *p = root_child;
        GameObject *last;
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
#pragma endregion
