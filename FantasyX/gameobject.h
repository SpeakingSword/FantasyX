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

    class GameObject {
    public:
        string name;
        GLuint tag;
        bool visible;
        GameObject *child;
        GameObject *sibling;
        Transform *transform;

        GameObject();
        GameObject(const GLchar *name, GLuint tag);
        void Add(GameObject *);
        void Add(GameObject *, GLuint index);
        void Release(const GLchar *name);
        void Release(GLuint index);
        void Update();
        void Draw();
        void Destroy();
        void Print();
        void Print(const GLchar *prefix);
        static void PrintAll(GameObject *root, string &prefix);
        static void PrintAllName(GameObject *root, string &prefix);
        bool ContainComponent(const GLchar *name) const;
        Component *GetComponent(const GLchar *name) const;
        void AddComponent(Component *);
        void RemoveComponent(const GLchar *name);
        void ShowAllComponent();

    private:
        static GLuint gameObjectCount;
        ComponentSystem *componentSystem;
        ~GameObject();
    };

}


#endif // !GAMEOBJECT_H