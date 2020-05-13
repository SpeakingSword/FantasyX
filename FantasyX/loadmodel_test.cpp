//#define LOADMODEL_TEST_H
#ifdef LOADMODEL_TEST_H

#include <iostream>

#include "scene.h"
#include "resource_manager.h"
#include "gameobject.h"

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

    ResourceManager *res = ResourceManager::GetInstance();
    GameObject *solder = res->LoadModel("D:\\OpenGL3DModels\\nanosuit\\nanosuit.obj");
    GameObject *gril = res->LoadModel("D:\\OpenGL3DModels\\matilda\\matilda.obj");
    string prefix = "-";
    GameObject::PrintAllName(solder, prefix);
    GameObject::PrintAllName(gril, prefix);

    while (!glfwWindowShouldClose(window))
    {
        // 渲染循环 ... 
    }
}
#endif // LOADMODEL_TEST_H