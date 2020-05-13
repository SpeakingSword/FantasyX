#define RENDERER_TEST_SIMPLE_DRAW_H
#ifdef RENDERER_TEST_SIMPLE_DRAW_H

#include <iostream>
#include "scene.h"
#include "gameobject.h"
#include "clocker.h"
#include "resource_manager.h"
#include "material.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

using namespace std;
using namespace fx;

const GLuint SCR_WIDTH = 1280;
const GLuint SCR_HEIGHT = 720;

GameObject *global_cube_p;
GameObject *global_cam_p;

void processInput(GLFWwindow *window);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FantasyX", NULL, NULL);
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

    Scene *scene = new Scene();
    Renderer *renderer = new Renderer();
    renderer->canvaWidth = SCR_WIDTH;
    renderer->canvaHeight = SCR_HEIGHT;
    renderer->finalDisplay = DISPLAY_ONLY_POS_MAP;
    renderer->backGroundColor = Vector3(1.0f, 1.0f, 1.0f);

    GameObject *cube = GameObject::Cube();
    PBRSimpleMaterial *cube_mat = (PBRSimpleMaterial *)((Render *)cube->GetComponent("Render"))->material;
    cube_mat->baseColor = Vector3(0.0f, 1.0f, 0.0f);
    global_cube_p = cube;
    cube->name = "Cube1";
    cube->transform->position = Vector3(0.0f, 0.0f, 0.0f);

    GameObject *camera_obj = GameObject::Camera();
    camera_obj->name = "Camera1";
    global_cam_p = camera_obj;
    camera_obj->transform->position = Vector3(0.0f, 0.0f, 4.0f);
    Camera *camera = (Camera *)camera_obj->GetComponent("Camera");
    camera->width = SCR_WIDTH;
    camera->height = SCR_HEIGHT;
    
    scene->renderer = renderer;
    scene->Add(cube);
    scene->Add(camera_obj);

    scene->Init();

    GLfloat timer = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        Time::UpdateClocker();

        if (timer > 1.0f)
        {
            std::cout << 1 / Time::deltaTime << std::endl;
            timer = 0.0f;
        }
        else
            timer += Time::deltaTime;

        processInput(window);

        cube->transform->rotation.y += 30.0f * Time::deltaTime;

        // 渲染循环 ... 
        scene->Update();
        scene->Render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        global_cube_p->transform->position -= global_cube_p->transform->right * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        global_cube_p->transform->position += global_cube_p->transform->right * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        global_cube_p->transform->position += global_cube_p->transform->up * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        global_cube_p->transform->position -= global_cube_p->transform->up * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        global_cam_p->transform->position += global_cam_p->transform->front * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        global_cam_p->transform->position -= global_cam_p->transform->front * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        global_cam_p->transform->position -= global_cam_p->transform->right * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        global_cam_p->transform->position += global_cam_p->transform->right * Time::deltaTime;
    }
}

#endif // RENDERER_TEST_SIMPLE_DRAW_H