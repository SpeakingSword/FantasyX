#include "gameobject.h"
#include "component.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace fx;

GLuint GameObject::gameObjectCount = 0;
// GLuint GameObject::cubeCount = 0;

GameObject::GameObject()
{
    ++GameObject::gameObjectCount;
    name = (string("GameObject") + std::to_string(gameObjectCount)).c_str();
    tag = GT_NONE;
    visible = true;

    child = nullptr;
    sibling = nullptr;

    transform = new Transform(this);
    componentSystem = new ComponentSystem();
    componentSystem->host = this;

    std::cout << "ENGIN CORE::GameObejct created ... " << std::endl;
}

GameObject::GameObject(const GLchar *name, GLuint tag)
{
    ++GameObject::gameObjectCount;
    this->name = name;
    this->tag = tag;
    visible = true;

    child = nullptr;
    sibling = nullptr;

    transform = new Transform(this);
    componentSystem = new ComponentSystem();
    componentSystem->host = this;

    std::cout << "ENGIN CORE::GameObejct created ... " << std::endl;
}

GameObject::~GameObject()
{
    --GameObject::gameObjectCount;
    transform->Destroy();
    componentSystem->Destroy();
    std::cout << "ENGIN CORE::GameObject destroyed ... " << std::endl;
}

void GameObject::Add(GameObject *obj)
{
    GameObject *child = this->child;
    if (child == nullptr)
    {
        this->child = obj;
    }
    else
    {
        GameObject *p = child;
        while (p->sibling != nullptr)
        {
            p = p->sibling;
        }
        // 此时 p 指向最右的对象
        p->sibling = obj;
    }
}

void GameObject::Add(GameObject *obj, GLuint index)
{

}

void GameObject::Release(const GLchar *name)
{

}

void GameObject::Release(GLuint index)
{

}

void GameObject::Update()
{
    this->transform->Update();
    this->componentSystem->Update();
}

void GameObject::Draw()
{
    Render *render = (Render *)this->GetComponent("Render");
    if (render != nullptr && render->mesh != nullptr)
    {
        render->material->MappingProperty();
        render->material->shader->Bind();
        render->material->shader->SetMat4("modelMatrix", this->transform->modelMatrix);
        render->material->shader->SetMat4("normalMatrix", glm::transpose(glm::inverse(this->transform->modelMatrix)));
        glBindVertexArray(render->mesh->VAO);
        if (render->mesh->indices.size() > 0)
        {
            glDrawElements(GL_TRIANGLES, render->mesh->indices.size(), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, render->mesh->vertices.size());
        }
        render->material->shader->Unbind();
        glBindVertexArray(0);
    }
}

void GameObject::Draw(Shader *shader)
{
    // 使用外部要求的、特殊的Shader绘制网格
}

void GameObject::Destroy()
{
    this->~GameObject();
}

void GameObject::Print()
{
    std::cout << "GameObject Name: " << name.c_str() << std::endl;
    std::cout << "GameObject Tag: " << tag << std::endl;
    std::cout << "GameObject Display: " << visible << std::endl;
    std::cout << "GameObject Child: " << (void *)child << std::endl;
    std::cout << "GameObject Sibling: " << (void *)sibling << std::endl;

}

void GameObject::Print(const GLchar *prefix)
{
    std::cout << prefix << "GameObject Name: " << name.c_str() << std::endl;
    std::cout << prefix << "GameObject Tag: " << tag << std::endl;
    std::cout << prefix << "GameObject Display: " << visible << std::endl;
    std::cout << prefix << "GameObject Child: " << (void *)child << std::endl;
    std::cout << prefix << "GameObject Sibling: " << (void *)sibling << std::endl;
}

void GameObject::PrintAll(GameObject *root, std::string &prefix)
{
    // 先序遍历
    if (root == nullptr)
        return;

    root->Print(prefix.c_str());

    PrintAll(root->child, prefix.append(prefix.c_str(), 1));

    prefix = prefix.substr(0, prefix.length() - 1);
    PrintAll(root->sibling, prefix);
}

void GameObject::PrintAllName(GameObject *root, std::string &prefix)
{
    // 先序遍历
    if (root == nullptr)
        return;

    std::cout << prefix.c_str() << root->name.c_str() << std::endl;

    PrintAllName(root->child, prefix.append(prefix.c_str(), 1));

    prefix = prefix.substr(0, prefix.length() - 1);
    PrintAllName(root->sibling, prefix);
}

bool GameObject::ContainComponent(const GLchar *name) const
{
    return componentSystem->Contain(name);
}

Component *GameObject::GetComponent(const GLchar *name) const
{
    return componentSystem->GetComponent(name);
}

void GameObject::AddComponent(Component *com)
{
    componentSystem->AddComponent(com);
}

void GameObject::RemoveComponent(const GLchar *name)
{
    componentSystem->RemoveComponent(name);
}

void GameObject::ShowAllComponent()
{
    componentSystem->ShowAllComponents();
}

GameObject *GameObject::Cube()
{
    GameObject *cube = new GameObject();
    cube->name = "Cube";
    cube->tag = GT_OPAQUE;

    Render *render = new Render();
    PBRSimpleMaterial *PBRSimple = new PBRSimpleMaterial();
    render->material = PBRSimple;
    render->mesh = CubeMesh::GetInstance();
    cube->AddComponent(render);

    return cube;
}

GameObject *GameObject::DirLight()
{
    GameObject *dirLight = new GameObject();
    dirLight->name = "DirLight";
    dirLight->tag = GT_LIGHT;

    fx::DirLight *lighting = new fx::DirLight();
    dirLight->AddComponent(lighting);

    Render *render = new Render();
    // 应该使用2D材质（渲染图标）或者添加球体的mesh（可视化）
    PBRSimpleMaterial *PBRSimple = new PBRSimpleMaterial();
    render->material = PBRSimple;
    dirLight->AddComponent(render);

    return dirLight;
}

GameObject *GameObject::PointLight()
{
    GameObject *pointLight = new GameObject();
    pointLight->name = "PointLight";
    pointLight->tag = GT_LIGHT;

    fx::PointLight *lighting = new fx::PointLight();
    pointLight->AddComponent(lighting);

    Render *render = new Render();
    // 应该使用2D材质（渲染图标）或者添加球体的mesh（可视化）
    PBRSimpleMaterial *PBRSimple = new PBRSimpleMaterial();
    render->material = PBRSimple;
    pointLight->AddComponent(render);

    return pointLight;
}

GameObject *GameObject::SpotLight()
{
    GameObject *spotLight = new GameObject();
    spotLight->name = "SpotLight";
    spotLight->tag = GT_LIGHT;

    fx::SpotLight *lighting = new fx::SpotLight();
    spotLight->AddComponent(lighting);

    Render *render = new Render();
    // 应该使用2D材质（渲染图标）或者添加球体的mesh（可视化）
    PBRSimpleMaterial *PBRSimple = new PBRSimpleMaterial();
    render->material = PBRSimple;
    spotLight->AddComponent(render);

    return spotLight;
}

GameObject *GameObject::Camera()
{
    GameObject *camera = new GameObject();
    camera->name = "Camera";
    camera->tag = GT_CAMERA;

    fx::Camera *cam_com = new fx::Camera();
    camera->AddComponent(cam_com);

    Render *render = new Render();
    // 应该使用2D材质（渲染图标）或者添加球体的mesh（可视化）
    PBRSimpleMaterial *PBRSimple = new PBRSimpleMaterial();
    render->material = PBRSimple;
    camera->AddComponent(render);

    return camera;
}

