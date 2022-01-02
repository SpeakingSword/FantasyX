//#define SCENE_TEST_H
#ifdef SCENE_TEST_H

#include <iostream>
#include <iomanip>

#include "scene.h"
#include "gameobject.h"
#include "clocker.h"
#include "resource_manager.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

using namespace std;
using namespace fx;

int main()
{
#pragma region 必须的初始化部分
    // GLFW: 初始化与配置
    // ------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

    // GLFW: 创建窗口
    // -------------------------------------------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "FantasyX", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // GLAD: 加载所有的OpenGL函数
    // -----------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#pragma endregion

    Scene* scene = new Scene();
    ResourceManager* res = ResourceManager::GetInstance();

    GameObject* cube_1 = GameObject::Cube();
    cube_1->name = "Cube1";
    GameObject* cube_2 = GameObject::Cube();
    cube_2->name = "Cube2";
    GameObject* cube_3 = GameObject::Cube();
    cube_3->name = "Cube3";
    GameObject* cube_4 = GameObject::Cube();
    cube_4->name = "Cube4";

    GameObject* dirLight = GameObject::DirLight();
    dirLight->name = "DirLight";

    GameObject* pointLight_1 = GameObject::PointLight();
    pointLight_1->name = "PointLight1";
    pointLight_1->transform->position = Vector3(4.0f, 0.0f, 0.0f);
    GameObject* pointLight_2 = GameObject::PointLight();
    pointLight_2->name = "PointLight2";
    pointLight_2->transform->position = Vector3(-4.0f, 0.0f, 0.0f);

    cube_1->transform->position = Vector3(0.0f, 0.0f, 2.0f);
    cube_2->transform->position = Vector3(0.0f, 0.0f, 4.0f);
    cube_3->transform->position = Vector3(0.0f, 0.0f, -8.0f);
    cube_4->transform->position = Vector3(0.0f, 0.0f, -4.0f);

    scene->Add(cube_1);
    scene->Add(cube_2);
    scene->Add(cube_3);
    scene->Add(cube_4);
    scene->Add(dirLight);
    scene->Add(pointLight_1);
    scene->Add(pointLight_2);

    scene->Init();

    Scene::InorderPrintBSP(scene->GetOpaqueTree().root);

    GLfloat timer = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        Time::UpdateClocker();

        if (timer > 1.0f)
        {
            Vector3 dir_front = dirLight->transform->front;
            std::cout << "DirLight Front: " << dir_front.x << " " << dir_front.y << " " << dir_front.z << std::endl;
            timer = 0.0f;
        }
        else
            timer += Time::deltaTime;

        dirLight->transform->rotation.y += 20 * Time::deltaTime;

        // 渲染循环 ... 
        scene->Update();
        scene->Render();
    }
}

#endif // SCENE_TEST_H