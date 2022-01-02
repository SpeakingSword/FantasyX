#ifndef MESH_H
#define MESH_H

#include <vector>
#include <iostream>
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "types.h"
#include "texture.h"

using namespace std;

namespace fx {

    class Mesh {
    public:
        string name;
        string path;
        vector<Vertex> vertices;
        vector<GLuint> indices;
        GLuint VAO, VBO, EBO;
    
        virtual void InitMesh() = 0;
        virtual void Print() = 0;
        virtual void Destroy() = 0;
        void SetupMesh();
    };
    
    class CubeMesh : public Mesh {
    public:
        static CubeMesh *GetInstance();
        void Print() override;

    private:
        static CubeMesh *_instance;
        CubeMesh();

        class GC {
        public:
            GC() { cout << "ENGIN CORE: CubeMesh GC created ... " << endl; }
            ~GC()
            {
                cout << "ENGIN CORE: CubeMesh GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "ENGIN CORE: CubeMesh destroyed ... " << endl;
                }
            }
        };
        static GC gc;

        void InitMesh() override;
        void Destroy() override;
    };

    class RectangleMesh : public Mesh {
    public:
        static RectangleMesh *GetInstance();
        void Print() override;

    private:
        static RectangleMesh *_instance;
        RectangleMesh();

        class GC {
        public:
            GC() { cout << "ENGIN CORE: RectangleMesh GC created ... " << endl; }
            ~GC()
            {
                cout << "ENGIN CORE: RectangleMesh GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "ENGIN CORE: RectangleMesh destroyed ... " << endl;
                }
            }
        };
        static GC gc;

        void InitMesh() override;
        void Destroy() override;
    };
    
    class PolygonMesh : public Mesh {
    public:
        PolygonMesh(const vector<Vertex> &vertices, const vector<GLuint> &indices, const GLchar* path);
        void Print() override;
        void Destroy() override;

    private:
        void InitMesh() override;
        void InitMeshData(const vector<Vertex> &vertices, const vector<GLuint> &indices, const GLchar* path);
        ~PolygonMesh();
    };

}

#endif // !MESH_H