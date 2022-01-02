#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <fstream>
#include <sstream>
#include <iostream>

#include "types.h"

#include <glad\glad.h>

using namespace std;

namespace fx {

    class Component;
    class ComponentSystem;
    class Transform;
    class Shader;
    class Material;
    class Renderer;

    class GameObject {
    public:
        string name;
        GLuint tag;
        bool visible;
        GameObject *child;
        GameObject *sibling;
        Transform *transform;
        ComponentSystem *componentSystem;

        GameObject();
        GameObject(const GLchar *name, GLuint tag);
        void Init();
        void Add(GameObject *);
        void Add(GameObject *, GLuint index);
        void Release(const GLchar *name);
        void Release(GLuint index);
        void Update();
        void Draw();
        void Draw(Shader *shader);
        void Destroy();
        void Print();
        void PrintName();
        void Print(const GLchar *prefix);
        static void PrintAll(GameObject *root, string &prefix);
        static void PrintAllName(GameObject *root, string &prefix);
        bool ContainComponent(const GLchar *name) const;
        Component *GetComponent(const GLchar *name) const;
        void AddComponent(Component *);
        void RemoveComponent(const GLchar *name);
        void ShowAllComponent();
        void SetMaterial(Material *mat);
        void SetVisible(bool visible);
        Material *GetMaterial();

        static GameObject *Cube();
        static GameObject *DirLight();
        static GameObject *PointLight();
        static GameObject *SpotLight();
        static GameObject *Camera();

    private:
        static GLuint gameObjectCount;
        ~GameObject();
        static void TraverseSetMat(GameObject *obj, Material *mat);
        static void TraverseSetVisible(GameObject *obj, bool visible);
    };

}


#endif // !GAMEOBJECT_H