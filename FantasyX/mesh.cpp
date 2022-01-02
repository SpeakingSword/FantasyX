#include "mesh.h"

namespace fx {

#pragma region Mesh
    void Mesh::SetupMesh()
    {
        // 创建缓存对象
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // 发送数据给缓存对象
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        /* 设置顶点属性指针 */
        // 顶点位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // 顶点纹理坐标
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
        glEnableVertexAttribArray(1);
        // 顶点法线
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        // 顶点切线
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);
        // 顶点副切线
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);
    }
#pragma endregion

#pragma region CubeMesh
    CubeMesh* CubeMesh::_instance = nullptr;

    CubeMesh::CubeMesh()
    {
        InitMesh();
        cout << "ENGIN CORE: CubeMesh created ... " << endl;
    }

    void CubeMesh::Destroy()
    {
        // Do nothing ...
    }

    void CubeMesh::InitMesh()
    {
        name = "CubeMesh";
        path = "\\CubeMesh";
        GLfloat cube[] = {
            // 顶点位置（逆时针）	  // 纹理坐标  // 顶点法向量
            // Back face
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,// Bottom-left
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,// top-right
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f,// bottom-right         
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,// top-right
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,// bottom-left
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f,// top-left
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,// bottom-left
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f,  1.0f,// bottom-right
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f,  1.0f,// top-right
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f,  1.0f,// top-right
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  1.0f,// top-left
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,// bottom-left
            // Left face
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,-1.0f, 0.0f,  0.0f,// top-right
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,-1.0f, 0.0f,  0.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,-1.0f, 0.0f,  0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,-1.0f, 0.0f,  0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,-1.0f, 0.0f,  0.0f, // bottom-right
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,-1.0f, 0.0f,  0.0f, // top-right
            // Right face
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f,  0.0f, // top-left
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f,  0.0f,// bottom-right
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f,  0.0f,// top-right         
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f,  0.0f,// bottom-right
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f,  0.0f,// top-left
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f,// bottom-left     
            // Bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,// top-right
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f,// top-left
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,// bottom-left
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,// bottom-left
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f,// bottom-right
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,// top-right
            // Top face
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// top-left
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,// bottom-right
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,// top-right     
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,// bottom-right
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// top-left
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f // bottom-left        
        };

        for (int i = 0; i < sizeof(cube) / (sizeof(GLfloat) * 8); i++)
        {
            Vertex vertex;
            vertex.position.x = cube[i * 8 + 0];
            vertex.position.y = cube[i * 8 + 1];
            vertex.position.z = cube[i * 8 + 2];

            vertex.texcoord.x = cube[i * 8 + 3];
            vertex.texcoord.y = cube[i * 8 + 4];

            vertex.normal.x = cube[i * 8 + 5];
            vertex.normal.y = cube[i * 8 + 6];
            vertex.normal.z = cube[i * 8 + 7];

            vertices.push_back(vertex);
        }

        for (int i = 0; i < vertices.size(); i += 3)
        {
            Vector3 tangent, bitangent;

            Vector3 edge1 = vertices[i + 1].position - vertices[i + 0].position;
            Vector3 edge2 = vertices[i + 2].position - vertices[i + 0].position;
            Vector2 deltaUV1 = vertices[i + 1].texcoord - vertices[i + 0].texcoord;
            Vector2 deltaUV2 = vertices[i + 2].texcoord - vertices[i + 0].texcoord;

            GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);

            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);

            vertices[i + 0].tangent = tangent;
            vertices[i + 1].tangent = tangent;
            vertices[i + 2].tangent = tangent;

            vertices[i + 0].bitangent = bitangent;
            vertices[i + 1].bitangent = bitangent;
            vertices[i + 2].bitangent = bitangent;
        }

        for (int i = 0; i < vertices.size(); i++)
        {
            indices.push_back(i);
        }

        SetupMesh();
    }

    CubeMesh* CubeMesh::GetInstance()
    {
        if (_instance == nullptr)
        {
            _instance = new CubeMesh();
        }
        return _instance;
    }

    void CubeMesh::Print()
    {
        std::cout << "Mesh Name: " << this->name.c_str() << std::endl;
        std::cout << "Mesh Path: " << this->path.c_str() << std::endl;
    }
#pragma endregion

#pragma region RectangleMesh
    RectangleMesh* RectangleMesh::_instance = nullptr;

    RectangleMesh::RectangleMesh()
    {
        InitMesh();
        cout << "ENGIN CORE: " << name.c_str() << " created ... " << endl;
    }

    void RectangleMesh::Destroy()
    {
        // Do nothing ... 
    }

    void RectangleMesh::InitMesh()
    {
        name = "RectangleMesh";
        path = "\\RectangleMesh";
        float rectangle[] = {
            // 顶点位置           // 纹理坐标  //顶点法向量
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        for (int i = 0; i < sizeof(rectangle) / (sizeof(GLfloat) * 8); i++)
        {
            Vertex vertex;
            vertex.position.x = rectangle[i * 8 + 0];
            vertex.position.y = rectangle[i * 8 + 1];
            vertex.position.z = rectangle[i * 8 + 2];

            vertex.texcoord.x = rectangle[i * 8 + 3];
            vertex.texcoord.y = rectangle[i * 8 + 4];

            vertex.normal.x = rectangle[i * 8 + 5];
            vertex.normal.y = rectangle[i * 8 + 6];
            vertex.normal.z = rectangle[i * 8 + 7];

            vertices.push_back(vertex);
        }

        for (int i = 0; i < vertices.size(); i += 3)
        {
            Vector3 tangent, bitangent;

            Vector3 edge1 = vertices[i + 1].position - vertices[i + 0].position;
            Vector3 edge2 = vertices[i + 2].position - vertices[i + 0].position;
            Vector2 deltaUV1 = vertices[i + 1].texcoord - vertices[i + 0].texcoord;
            Vector2 deltaUV2 = vertices[i + 2].texcoord - vertices[i + 0].texcoord;

            GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);

            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);

            vertices[i + 0].tangent = tangent;
            vertices[i + 1].tangent = tangent;
            vertices[i + 2].tangent = tangent;

            vertices[i + 0].bitangent = bitangent;
            vertices[i + 1].bitangent = bitangent;
            vertices[i + 2].bitangent = bitangent;
        }

        for (int i = 0; i < vertices.size(); i++)
        {
            indices.push_back(i);
        }

        SetupMesh();
    }

    RectangleMesh* RectangleMesh::GetInstance()
    {
        if (_instance == nullptr)
        {
            _instance = new RectangleMesh();
        }

        return _instance;
    }

    void RectangleMesh::Print()
    {
        std::cout << "Mesh Name: " << this->name.c_str() << std::endl;
        std::cout << "Mesh Path: " << this->path.c_str() << std::endl;
    }

#pragma endregion

#pragma region PolygonMesh
    PolygonMesh::PolygonMesh(const vector<Vertex>& vertices, const vector<GLuint>& indices, const GLchar* path)
    {
        InitMeshData(vertices, indices, path);
        InitMesh();
        cout << "ENGIN CORE: PolygonMesh created ... " << endl;
    }

    PolygonMesh::~PolygonMesh()
    {
        cout << "ENGIN CORE: PolygonMesh destroyed ..." << endl;
    }

    void PolygonMesh::Destroy()
    {
        this->~PolygonMesh();
    }

    void PolygonMesh::InitMesh()
    {
        SetupMesh();
    }

    void PolygonMesh::InitMeshData(const vector<Vertex>& vertices, const vector<GLuint>& indices, const GLchar* path)
    {
        this->path = path;

        this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());

        this->indices.insert(this->indices.end(), indices.begin(), indices.end());
    }

    void PolygonMesh::Print()
    {
        std::cout << "Mesh Name: " << this->name.c_str() << std::endl;
        std::cout << "Mesh Path: " << this->path.c_str() << std::endl;
    }
#pragma endregion

}