#ifndef COMPONENT_H
#define COMPONENT_H

#include <unordered_map>
#include <iostream>

#include "types.h"
#include "mesh.h"

#include <glad\glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

namespace fx {

    class GameObject;
    class Material;

    class Component {
    public:
        string name;
        GameObject* gameObject;

        virtual void Awake() = 0;
        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Finish() = 0;
        virtual void DrawUIElements() = 0;
        virtual void Destroy() = 0;
        virtual void Print() = 0;
    };

    class ComponentSystem {
    public:
        GameObject* host;

        ComponentSystem();

        void Start();
        void Update();
        bool Contain(const GLchar* name) const;
        Component* GetComponent(const GLchar* name) const;
        void AddComponent(Component*);
        void RemoveComponent(const GLchar* name);
        void ShowAllComponents() const;
        void DrawUIElements();
        void Destroy();

    private:
        ~ComponentSystem();
        unordered_map<string, Component*> componentMap;
    };

    class Transform : public Component {
    public:
        Vector3 position;

        // rotation.y eq yaw, rotation.x eq pitch
        Vector3 rotation;
        Vector3 scale;

        // 下面的变量需要在每一帧都更新
        Vector3 up;
        Vector3 right;
        Vector3 front;
        Vector3 worldUp;

        // 对象的世界位置为模型变换矩阵乘上模型局部空间的原点
        // 一般为坐标原点(0, 0, 0)
        Vector3 worldPos;

        // 当前对象的模型变换矩阵为在父对象的模型矩阵基础上
        // 接着做缩放、旋转、位移变换之后的矩阵
        // 所以遍历层级结构时，会更新当前节点的模型变换矩阵
        Matrix4x4 modelMatrix;

        Transform();
        Transform(GameObject* host);
        ~Transform();
        void Awake() override;
        void Start() override;
        void Update() override;
        void Finish() override;
        void DrawUIElements() override;
        void Translate(Vector3 dir, GLfloat far, Space mode);
        void Rotate(Vector3 axis, GLfloat deg, Space mode);
        void RotateAround(Vector3 pos, Vector3 axis, GLfloat deg, Space mode);
        void UpdateVector();
        void Destroy() override;
        void Print() override;
    };

    class Render : public Component {
    public:
        Mesh* mesh;
        Material* material;

        Render();
        void Awake() override;
        void Start() override;
        void Update() override;
        void Finish() override;
        void DrawUIElements() override;
        void Destroy() override;
        void Print() override;
    private:
        ~Render();
    };

    class Lighting : public Component {
    public:
        string type;

        virtual void Awake() = 0;
        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Finish() = 0;
        virtual void DrawUIElements() = 0;
        virtual void Destroy() = 0;
        virtual void Print() = 0;
        virtual const GLchar* GetType() = 0;
    };

    class DirLight : public Lighting {
    public:
        Vector3 color;
        GLfloat ambient;
        GLfloat diffuse;
        GLfloat specular;
        GLfloat strength;

        // direction eq transform.front
        Vector3 direction;
        GLfloat shadowNear;
        GLfloat shadowFar;

        DirLight();
        void Awake() override;
        void Start() override;
        void Update() override;
        void Finish() override;
        void DrawUIElements() override;
        void Destroy() override;
        void Print() override;
        const GLchar* GetType() override;

    private:
        ~DirLight();
    };

    class PointLight : public Lighting {
    public:
        Vector3 color;
        GLfloat ambient;
        GLfloat diffuse;
        GLfloat specular;
        GLfloat strength;
        GLfloat radius;
        Attenuation attenuation;

        // position eq transform.worldPos
        Vector3 position;

        PointLight();
        void Awake() override;
        void Start() override;
        void Update() override;
        void Finish() override;
        void DrawUIElements() override;
        void Destroy() override;
        void Print() override;
        const GLchar* GetType() override;

    private:
        ~PointLight();
    };

    class SpotLight : public Lighting {
    public:
        Vector3 color;
        GLfloat ambient;
        GLfloat diffuse;
        GLfloat specular;
        GLfloat strength;
        GLfloat cutOff;
        GLfloat outerCutOff;
        Attenuation attenuation;

        // direction eq transform.front, position eq transform.worldPos
        Vector3 position;
        Vector3 direction;

        SpotLight();
        void Awake() override;
        void Start() override;
        void Update() override;
        void Finish() override;
        void DrawUIElements() override;
        void Destroy() override;
        void Print() override;
        const GLchar* GetType() override;

    private:
        ~SpotLight();
    };

    // 摄像机常量，用来快速恢复初始的状态
    const GLfloat YAW = -90.0f;
    const GLfloat PITCH = 0.0f;
    const GLfloat MOVE_SPEED = 2.5f;
    const GLfloat SENSITIVITY = 0.05f;
    const GLfloat FOV = 60.0f;
    const GLfloat MAX_FOV = 80.0f;
    const GLfloat MIN_FOV = 0.1f;
    const GLfloat ZOOM_SPEED = 8.0f;
    const GLfloat NEAR = 0.1f;
    const GLfloat FAR = 1000.0f;

    class Camera : public Component {
    public:

        // position eq transform.worldPos,
        // front eq = transform.front, right eq transform.right,
        // up = transform.up, worldUp eq transform.worldUp
        // yaw eq transform.roattion.y, pitch eq transform.rotation.x
        Vector3 position;
        Vector3 front;
        Vector3 right;
        Vector3 up;
        Vector3 worldUp;
        GLfloat yaw;
        GLfloat pitch;

        // 相机可配置选项
        GLfloat moveSpeed;
        GLfloat sensitivity;
        GLfloat fov;
        GLfloat zoomSpeed;
        GLfloat near;
        GLfloat far;
        GLuint operationMode;
        GLuint projection;
        bool constrainPitch;

        Camera(Vector3 position = Vector3(0.0f), Vector3 up = Vector3(0.0f, 1.0f, 0.0f),
            GLfloat yaw = YAW, GLfloat pitch = PITCH) : front(Vector3(0.0f, 0.0f, -1.0f)), moveSpeed(MOVE_SPEED),
            sensitivity(SENSITIVITY), fov(FOV), zoomSpeed(ZOOM_SPEED), near(NEAR), far(FAR)
        {
            this->name = "Camera";
            this->gameObject = nullptr;
            this->position = position;
            this->worldUp = up;
            this->yaw = yaw;
            this->pitch = pitch;
            this->operationMode = CAM_FIRST_PERSON;
            this->projection = CAM_PERSPECTIVE;
            this->constrainPitch = true;
            UpdateCameraVectors();

            std::cout << "Camera created ... " << std::endl;
        }

        Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
            front(Vector3(0.0f, 0.0f, -1.0f)), moveSpeed(MOVE_SPEED), sensitivity(SENSITIVITY), fov(FOV), zoomSpeed(ZOOM_SPEED),
            near(NEAR), far(FAR)
        {
            this->name = "Camera";
            this->gameObject = nullptr;
            this->position = Vector3(posX, posY, posZ);
            this->worldUp = Vector3(upX, upY, upZ);
            this->yaw = yaw;
            this->pitch = pitch;
            this->operationMode = CAM_FIRST_PERSON;
            this->projection = CAM_PERSPECTIVE;
            this->constrainPitch = true;
            UpdateCameraVectors();

            std::cout << "Camera created ... " << std::endl;
        }

        void Awake() override;
        void Start() override;
        void Update() override;
        void Finish() override;
        void DrawUIElements() override;
        void Destroy() override;
        void Print() override;
        void ProcessKeyboard(CameraMovement movement, GLfloat deltaTime);
        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);
        void FirstPersonMovement(GLfloat xoffset, GLfloat yoffset);
        void OrbitMovement(GLfloat xoffset, GLfloat yoffset);
        void ProcessMouseScroll(GLfloat yoffset, GLfloat deltaTime);
        Matrix4x4 lookAt(Vector3 cameraPos, Vector3 cameraTargetPos, Vector3 up);

    private:
        ~Camera();
        void UpdateCameraVectors();
    };

}

#endif // !COMPONENT_H