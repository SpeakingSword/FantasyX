#ifndef LOADMODEL_TEST_H
#define LOADMODEL_TEST_H

#include <iostream>

#include "scene.h"
#include "resource_manager.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

using namespace std;
using namespace fx;

//void InitEngine();

int main()
{
    //InitEngine();

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    ResourceManager *res = ResourceManager::GetInstance();
    GameObject *model = res->LoadModel("D:\\OpenGL3DModels\\nanosuit\\nanosuit.obj");
    string prefix = "-";
    GameObject::PrintAllName(model, prefix);
    
    while (!glfwWindowShouldClose(window))
    {
        // now do nothing !
    }
}

#endif // !LOADMODEL_TEST_H
