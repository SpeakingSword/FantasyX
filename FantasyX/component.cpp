#include "component.h"
#include "gameobject.h"
#include "material.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace fx;

#pragma region ComponentSystem
ComponentSystem::ComponentSystem()
{
    std::cout << "ENGIN CORE::ComponentSystem created ... " << std::endl;
}

ComponentSystem::~ComponentSystem()
{
    for (auto iter = componentMap.cbegin(); iter != componentMap.cend(); iter++)
    {
        iter->second->Destroy();
    }
    std::cout << "ENGIN CORE::ComponentSystem destroyed ... " << std::endl;
}

void ComponentSystem::Destroy()
{
    this->~ComponentSystem();
}

void ComponentSystem::Start()
{
    for (auto iter = componentMap.cbegin(); iter != componentMap.cend(); iter++)
    {
        iter->second->Start();
    }
}

void ComponentSystem::Update()
{
    for (auto iter = componentMap.cbegin(); iter != componentMap.cend(); iter++)
    {
        iter->second->Update();
    }
}

bool ComponentSystem::Contain(const GLchar *name) const
{
    auto iter = componentMap.cbegin();
    for (; iter != componentMap.cend(); iter++)
    {
        if (strcmp(iter->first.c_str(), name) == 0)
            return true;
    }

    return false;
}

void ComponentSystem::DrawUIElements()
{
    for (auto iter = componentMap.cbegin(); iter != componentMap.cend(); iter++)
    {
        iter->second->DrawUIElements();
    }
}

Component *ComponentSystem::GetComponent(const GLchar *name) const
{
    auto iter = componentMap.cbegin();
    for (; iter != componentMap.cend(); iter++)
    {
        if (strcmp(iter->first.c_str(), name) == 0)
            return iter->second;
    }

    std::cout << "ENGIN CORE::COMPONENT SYSTEM::GetComponent()::Component " << name << " can not found !" << std::endl;
    return nullptr;
}

void ComponentSystem::AddComponent(Component *com)
{
    if (Contain(com->name.c_str()))
    {
        std::cout << "ENGIN CORE::COMPONENT SYSTEM::AddComponent()::Component " << com->name.c_str() << " already exist !" << std::endl;
    }
    else
    {
        componentMap.insert(pair<string, Component *>(com->name, com));
        com->gameObject = this->host;
    }
}

void ComponentSystem::RemoveComponent(const GLchar *name)
{
    auto n = componentMap.erase(name);
    if (n == 0)
        std::cout << "ENGIN CORE::COMPONENT SYSTEM::RemoveComponent()::Component " << name << " not found !" << std::endl;
}

void ComponentSystem::ShowAllComponents() const
{
    for (auto iter = componentMap.cbegin(); iter != componentMap.cend(); iter++)
    {
        iter->second->Print();
    }
}
#pragma endregion

#pragma region Transform

Transform::Transform()
{
    name = "Transform";
    gameObject = nullptr;
    position = Vector3(0.0f, 0.0f, 0.0f);
    rotation = Vector3(0.0f, 0.0f, 0.0f);
    scale = Vector3(1.0f, 1.0f, 1.0f);
    up = Vector3(0.0f, 1.0f, 0.0f);
    worldUp = up;
    UpdateVector();

    std::cout << "ENGIN CORE::Transform created .. " << std::endl;
}

Transform::Transform(GameObject *host)
{
    name = "Transform";
    gameObject = host;
    position = Vector3(0.0f, 0.0f, 0.0f);
    rotation = Vector3(0.0f, 0.0f, 0.0f);
    scale = Vector3(1.0f, 1.0f, 1.0f);
    up = Vector3(0.0f, 1.0f, 0.0f);
    worldUp = up;
    UpdateVector();

    std::cout << "ENGIN CORE::Transform created .. " << std::endl;
}

Transform::~Transform()
{
    std::cout << "ENGIN CORE::Transform destroyed ... " << std::endl;
}

void Transform::Destroy()
{
    this->~Transform();
}

void Transform::Awake()
{

}

void Transform::Start()
{

}

void Transform::Update()
{
    if (this->rotation.y > 360.0f)
        this->rotation.y -= 360.0f;
    if (this->rotation.x > 360.0f)
        this->rotation.x -= 360.0f;
    if (this->rotation.z > 360.0f)
        this->rotation.z -= 360.0f;

    worldPos = modelMatrix * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    UpdateVector();
}

void Transform::Finish()
{

}

void Transform::DrawUIElements()
{
    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::DragFloat3("position", (GLfloat *)&this->position, 0.1f);
    ImGui::DragFloat3("rotation", (GLfloat *)&this->rotation, 0.1f);
    ImGui::DragFloat3("scale", (GLfloat *)&this->scale, 0.1f);
}

void Transform::Translate(Vector3 dir, GLfloat far, Space mode)
{
    
}

void Transform::Rotate(Vector3 axis, GLfloat deg, Space mode)
{

}

void Transform::RotateAround(Vector3 pos, Vector3 axis, GLfloat deg, Space mode)
{

}

void Transform::UpdateVector()
{
    Vector3 temp_front;
    temp_front.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    temp_front.y = sin(glm::radians(rotation.x));
    temp_front.z = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    this->front = glm::normalize(temp_front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void Transform::Print()
{
    std::cout << "Component Name: " << this->name.c_str() << std::endl;
    const GLchar *host_name = (this->gameObject != nullptr) ? this->gameObject->name.c_str() : "NULL";
    std::cout << "Host Name: " << host_name << std::endl;
    std::cout << "Position: " << position.x << " " << position.y << " " << position.z << std::endl;
    std::cout << "Rotation: " << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;
    std::cout << "Scale: " << scale.x << " " << scale.y << " " << scale.z << std::endl;
    std::cout << "WorldPos: " << worldPos.x << " " << worldPos.y << " " << worldPos.z << std::endl;
    std::cout << "WorldUp: " << worldUp.x << " " << worldUp.y << " " << worldUp.z << std::endl;
    std::cout << "Up: " << up.x << " " << up.y << " " << up.z << std::endl;
    std::cout << "Right: " << right.x << " " << right.y << " " << right.z << std::endl;
    std::cout << "Front: " << front.x << " " << front.y << " " << front.z << std::endl;
    // 对于模型变换矩阵暂时不需要输出 ...
}
#pragma endregion

#pragma region Render
Render::Render()
{
    this->name = "Render";
    this->gameObject = nullptr;
    this->mesh = nullptr;
    this->material = nullptr;

    std::cout << "Render created ... " << std::endl;
}

Render::~Render()
{
    std::cout << "Render destroyed ... " << std::endl;
}

void Render::Destroy()
{
    this->~Render();
}

void Render::Awake()
{

}

void Render::Start()
{

}

void Render::Update()
{
    // this->material->MappingProperty();
}

void Render::DrawUIElements()
{
    ImGui::Separator();
    ImGui::Text("Render");
    if (material != nullptr)
    {
        material->DrawUIElements();
    }
    if (mesh != nullptr)
    {
        ImGui::Text("Mesh: %s", mesh->name.c_str());
        ImGui::Text("Path: %s", mesh->path.c_str());
    }


}

void Render::Finish()
{

}

void Render::Print()
{
    std::cout << "Component Name: " << this->name.c_str() << std::endl;
    const GLchar *host_name = (this->gameObject != nullptr) ? this->gameObject->name.c_str() : "NULL";
    std::cout << "Host Name: " << host_name << std::endl;
    if (this->mesh != nullptr)
    {
        this->mesh->Print();
    }
    else
        std::cout << "Mesh: NULL" << std::endl;

    if (this->material != nullptr)
    {
        this->material->Print();
    }
    else
        std::cout << "Material: NULL" << std::endl;
    
}
#pragma endregion

#pragma region DirLight
DirLight::DirLight()
{
    this->name = "Light";
    this->type = "DirLight";
    this->gameObject = nullptr;
    this->color = Vector3(1.0f);
    this->ambient = 0.1f;
    this->diffuse = 0.4f;
    this->specular = 0.7f;
    this->direction = Vector3(-1.0f, -1.0f, 0.0f);
    this->strength = 1.0f;

    std::cout << "DirLight created ... " << std::endl;
}

DirLight::~DirLight()
{
    std::cout << "DirLight destroyed ... " << std::endl;
}

void DirLight::Destroy()
{
    this->~DirLight();
}

void DirLight::Awake()
{

}

void DirLight::Start()
{

}

void DirLight::Update()
{
    this->direction = gameObject->transform->front;
}

void DirLight::Finish()
{

}

void DirLight::DrawUIElements()
{
    ImGui::Separator();
    ImGui::Text("Lighting");
    ImGui::Text("LightType: %s", name.c_str());
    ImGui::ColorEdit3("LightColor", (GLfloat *)&color);
    ImGui::SliderFloat("LightStrength", &strength, 0.0f, 5.0f);
}

const GLchar *DirLight::GetType()
{
    return this->type.c_str();
}

void DirLight::Print()
{
    std::cout << "Component Name: " << this->name.c_str() << std::endl;
    const GLchar *host_name = (this->gameObject != nullptr) ? this->gameObject->name.c_str() : "NULL";
    std::cout << "Host Name: " << host_name << std::endl;
    std::cout << "Light Type: " << this->type.c_str() << std::endl;
    std::cout << "Light Color: " << color.x << " " << color.y << " " << color.z << std::endl;
    std::cout << "Light Ambient: " << ambient << std::endl;
    std::cout << "Light Diffuse: " << diffuse << std::endl;
    std::cout << "Light Specular: " << specular << std::endl;
    std::cout << "Light Direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
    std::cout << "Light Strength: " << strength << std::endl;
}
#pragma endregion

#pragma region PointLight
PointLight::PointLight()
{
    this->name = "Light";
    this->type = "PointLight";
    this->gameObject = nullptr;
    this->color = Vector3(1.0f);
    this->position = Vector3(0.0f);
    this->ambient = 0.1f;
    this->diffuse = 0.8f;
    this->specular = 0.4f;
    this->strength = 1.0f;
    this->radius = 50.0f;
    this->attenuation.constant = 1.0f;
    this->attenuation.linear = 0.09f;
    this->attenuation.quadratic = 0.032f;

    std::cout << "PointLight created ... " << std::endl;
}

PointLight::~PointLight()
{
    std::cout << "PointLight destroyed ... " << std::endl;
}

void PointLight::Destroy()
{
    this->~PointLight();
}

void PointLight::Awake()
{

}

void PointLight::Start()
{

}

void PointLight::Update()
{
    this->position = gameObject->transform->worldPos;
}

void PointLight::Finish()
{

}

void PointLight::DrawUIElements()
{
    ImGui::Separator();
    ImGui::Text("Lighting");
    ImGui::Text("LightType: %s", name.c_str());
    ImGui::ColorEdit3("LightColor", (GLfloat *)&color);
    ImGui::SliderFloat("LightStrength", &strength, 0.0f, 5.0f);
}

const GLchar *PointLight::GetType()
{
    return this->type.c_str();
}

void PointLight::Print()
{
    std::cout << "Component Name: " << this->name.c_str() << std::endl;
    const GLchar *host_name = (this->gameObject != nullptr) ? this->gameObject->name.c_str() : "NULL";
    std::cout << "Host Name: " << host_name << std::endl;
    std::cout << "Light Type: " << this->type.c_str() << std::endl;
    std::cout << "Light Color: " << color.x << " " << color.y << " " << color.z << std::endl;
    std::cout << "Light Ambient: " << ambient << std::endl;
    std::cout << "Light Diffuse: " << diffuse << std::endl;
    std::cout << "Light Specular: " << specular << std::endl;
    std::cout << "Light Position: " << position.x << " " << position.y << " " << position.z << std::endl;
    std::cout << "Light Strength: " << strength << std::endl;
    std::cout << "Light Radius: " << radius << std::endl;
    std::cout << "Constant: " << attenuation.constant << std::endl;
    std::cout << "Linear: " << attenuation.linear << std::endl;
    std::cout << "Quadratic: " << attenuation.quadratic << std::endl;
}
#pragma endregion

#pragma region SpotLight
SpotLight::SpotLight()
{
    this->name = "Light";
    this->type = "SpotLight";
    this->gameObject = nullptr;
    this->color = Vector3(1.0f);
    this->position = Vector3(0.0f, 1.0f, 0.0f);
    this->direction = Vector3(0.0f, -1.0f, 0.0f);
    this->ambient = 0.1f;
    this->diffuse = 1.0f;
    this->specular = 0.4f;
    this->strength = 1.0f;
    this->cutOff = 12.5f;
    this->outerCutOff = 30.0f;
    this->attenuation.constant = 1.0f;
    this->attenuation.linear = 0.09f;
    this->attenuation.quadratic = 0.032f;

    std::cout << "SpotLight created ... " << std::endl;
}

SpotLight::~SpotLight()
{
    std::cout << "SpotLight destroyed ... " << std::endl;
}

void SpotLight::Destroy()
{
    this->~SpotLight();
}

void SpotLight::Awake()
{

}

void SpotLight::Start()
{

}

void SpotLight::Update()
{
    this->position = gameObject->transform->worldPos;
    this->direction = gameObject->transform->front;
}

void SpotLight::Finish()
{

}

void SpotLight::DrawUIElements()
{
    ImGui::Separator();
    ImGui::Text("Lighting");
    ImGui::Text("LightType: %s", name.c_str());
    ImGui::ColorEdit3("LightColor", (GLfloat *)&color);
    ImGui::SliderFloat("LightStrength", &strength, 0.0f, 5.0f);
}

const GLchar *SpotLight::GetType()
{
    return this->type.c_str();
}

void SpotLight::Print()
{
    std::cout << "Component Name: " << this->name.c_str() << std::endl;
    const GLchar *host_name = (this->gameObject != nullptr) ? this->gameObject->name.c_str() : "NULL";
    std::cout << "Host Name: " << host_name << std::endl;
    std::cout << "Light Type: " << this->type.c_str() << std::endl;
    std::cout << "Light Color: " << color.x << " " << color.y << " " << color.z << std::endl;
    std::cout << "Light Ambient: " << ambient << std::endl;
    std::cout << "Light Diffuse: " << diffuse << std::endl;
    std::cout << "Light Specular: " << specular << std::endl;
    std::cout << "Light Position: " << position.x << " " << position.y << " " << position.z << std::endl;
    std::cout << "Light Direction: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
    std::cout << "Light Strength: " << strength << std::endl;
    std::cout << "CutOff: " << cutOff << std::endl;
    std::cout << "OuterCutOff: " << outerCutOff << std::endl;
    std::cout << "Constant: " << attenuation.constant << std::endl;
    std::cout << "Linear: " << attenuation.linear << std::endl;
    std::cout << "Quadratic: " << attenuation.quadratic << std::endl;
}
#pragma endregion

#pragma region Camera
Camera::~Camera()
{
    std::cout << "Camera destroyed ... " << std::endl;
}

void Camera::Destroy()
{
    this->~Camera();
}

void Camera::ProcessKeyboard(CameraMovement movement, GLfloat deltaTime)
{
    switch (movement)
    {
    case CAM_FORWARD:
        this->gameObject->transform->position += this->gameObject->transform->front * moveSpeed * deltaTime;
        break;
    case CAM_BACKWARD:
        this->gameObject->transform->position -= this->gameObject->transform->front * moveSpeed * deltaTime;
        break;
    case CAM_RIGHT:
        this->gameObject->transform->position += this->gameObject->transform->right * moveSpeed * deltaTime;
        break;
    case CAM_LEFT:
        this->gameObject->transform->position -= this->gameObject->transform->right * moveSpeed * deltaTime;
        break;
    default:
        break;
    }
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{

    switch (this->operationMode)
    {
    case CAM_FIRST_PERSON:
        FirstPersonMovement(xoffset, yoffset);
        break;
    case CAM_ORBIT:
        OrbitMovement(xoffset, yoffset);
        break;
    default:
        break;
    }

    UpdateCameraVectors();
}

void Camera::FirstPersonMovement(GLfloat xoffset, GLfloat yoffset)
{
    xoffset *= this->sensitivity;
    yoffset *= this->sensitivity;

    this->gameObject->transform->rotation.y += xoffset;
    this->gameObject->transform->rotation.x += yoffset;

    if (this->constrainPitch)
    {
        if (this->gameObject->transform->rotation.x > 89.0f)
            this->gameObject->transform->rotation.x = 89.0f;
        if (this->gameObject->transform->rotation.x < -89.0f)
            this->gameObject->transform->rotation.x = -89.0f;
    }
}

void Camera::OrbitMovement(GLfloat xoffset, GLfloat yoffset)
{
    
}

void Camera::ProcessMouseScroll(GLfloat yoffset, GLfloat deltaTime)
{
    this->gameObject->transform->position += this->gameObject->transform->front * yoffset * this->zoomSpeed * deltaTime;
}

Matrix4x4 Camera::lookAt(Vector3 cameraPos, Vector3 cameraTargetPos, Vector3 up)
{
    Vector3 cameraDirection = glm::normalize(cameraPos - cameraTargetPos);
    Vector3 cameraRight = glm::normalize(glm::cross(glm::normalize(up), cameraDirection));
    Vector3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
    Matrix4x4 translation;
    translation[3][0] = -cameraPos.x;
    translation[3][1] = -cameraPos.y;
    translation[3][2] = -cameraPos.z;
    Matrix4x4 rotation;
    rotation[0][0] = cameraRight.x;
    rotation[1][0] = cameraRight.y;
    rotation[2][0] = cameraRight.z;
    rotation[0][1] = cameraUp.x;
    rotation[1][1] = cameraUp.y;
    rotation[2][1] = cameraUp.z;
    rotation[0][2] = cameraDirection.x;
    rotation[1][2] = cameraDirection.y;
    rotation[2][2] = cameraDirection.z;
    return rotation * translation;
}

void Camera::UpdateCameraVectors()
{
    Vector3 temp_front;
    temp_front.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
    temp_front.y = sin(glm::radians(this->pitch));
    temp_front.z = cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
    this->front = glm::normalize(temp_front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void Camera::Awake()
{

}

void Camera::Start()
{

}

void Camera::Update()
{
    this->position = gameObject->transform->worldPos;
    this->front = gameObject->transform->front;
    this->right = gameObject->transform->right;
    this->up = gameObject->transform->up;
    this->worldUp = gameObject->transform->worldUp;
    this->yaw = gameObject->transform->rotation.y;
    this->pitch = gameObject->transform->rotation.x;
}

void Camera::Finish()
{

}

void Camera::DrawUIElements()
{
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::SliderFloat("MoveSpeed", &moveSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat("Sensitivity", &sensitivity, 0.0f, 10.0f);
    ImGui::SliderFloat("Fov", &fov, 0.0f, 120.0f);
    ImGui::SliderFloat("ZoomSpeed", &zoomSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat("Near", &near, 0.0f, 10.0f);
    ImGui::SliderFloat("Far", &far, 10.0f, 10000.0f);
    ImGui::Text("OperationMode:");
    ImGui::SameLine();
    if (ImGui::Button("FIRST PERSON"))
        operationMode = CAM_FIRST_PERSON;
    ImGui::SameLine();
    if (ImGui::Button("ORBIT"))
        operationMode = CAM_ORBIT;
    ImGui::Text("Projection:");
    ImGui::SameLine();
    if (ImGui::Button("PERSPECTIVE"))
        projection = CAM_PERSPECTIVE;
    ImGui::SameLine();
    if (ImGui::Button("ORTHOGRAPHIC"))
        projection = CAM_ORTHOGRAPHIC;
    ImGui::Checkbox("ConstrainPitch", &constrainPitch);
}

void Camera::Print()
{
    std::cout << "Component Name: " << this->name.c_str() << std::endl;
    const GLchar *host_name = (this->gameObject != nullptr) ? this->gameObject->name.c_str() : "NULL";
    std::cout << "Host Name: " << host_name << std::endl;
    std::cout << "Position: " << this->position.x << " " << this->position.y << " " << this->position.z << std::endl;
    std::cout << "Front: " << this->front.x << " " << this->front.y << " " << this->front.z << std::endl;
    std::cout << "Right: " << this->right.x << " " << this->right.y << " " << this->right.z << std::endl;
    std::cout << "Up: " << this->up.x << " " << this->up.y << " " << this->up.z << std::endl;
    std::cout << "WorldUp: " << this->worldUp.x << " " << this->worldUp.y << " " << this->worldUp.z << std::endl;
    std::cout << "Yaw: " << this->yaw << std::endl;
    std::cout << "Pitch" << this->pitch << std::endl;
    std::cout << "MoveSpeed: " << this->moveSpeed << std::endl;
    std::cout << "Sensitivity: " << this->sensitivity << std::endl;
    std::cout << "Fov: " << this->fov << std::endl;
    std::cout << "ZoomSpeed: " << this->zoomSpeed << std::endl;
    std::cout << "Near: " << this->near << std::endl;
    std::cout << "Far: " << this->far << std::endl;
    std::cout << "OperationMode: " << this->operationMode << std::endl;
    std::cout << "Projection: " << this->projection << std::endl;
    std::cout << "ConstrainPitch: " << this->constrainPitch << std::endl;
}
#pragma endregion
