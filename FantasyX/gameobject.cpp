#include "gameobject.h"
#include "component.h"

using namespace fx;

GLuint GameObject::gameObjectCount = 0;

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

    PrintAll(root->child, prefix.append(prefix.c_str()));

    prefix = prefix.substr(0, prefix.length() - 1);
    PrintAll(root->sibling, prefix);
}

void GameObject::PrintAllName(GameObject *root, std::string &prefix)
{
    // 先序遍历
    if (root == nullptr)
        return;

    std::cout << prefix.c_str() << root->name.c_str() << std::endl;

    PrintAllName(root->child, prefix.append(prefix.c_str()));

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

