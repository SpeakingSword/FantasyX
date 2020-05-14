#define RENDERER_TEST_SIMPLE_DRAW_H
#ifdef RENDERER_TEST_SIMPLE_DRAW_H

#include <glad\glad.h>
#include <iostream>
#include "context.h"
#include "scene.h"
#include "gameobject.h"
#include "clocker.h"
#include "resource_manager.h"
#include "material.h"

using namespace std;
using namespace fx;

const GLuint SCR_WIDTH = 1360;
const GLuint SCR_HEIGHT = 768;

int main()
{
    fxWindow *window = new fxWindow();
    window->width = SCR_WIDTH;
    window->height = SCR_HEIGHT;
    window->Init();

    Scene *scene = new Scene();
    Renderer *renderer = new Renderer();
    renderer->canvaWidth = SCR_WIDTH;
    renderer->canvaHeight = SCR_HEIGHT;
    renderer->finalDisplay = DISPLAY_ONLY_NORMAL_MAP;
    renderer->backGroundColor = Vector3(0.0f, 0.0f, 0.0f);
    scene->renderer = renderer;

    // 零对象通常为一个空对象
    GameObject *zero = new GameObject();
    zero->name = "ZeroObject";
    zero->transform->position = Vector3(0.0f, 0.0f, 0.0f);

    ResourceManager *res = ResourceManager::GetInstance();
    GameObject *gril = res->LoadModel("D:\\OpenGLAssets\\Models\\matilda\\matilda.obj");
    gril->name = "Cube";
    gril->transform->position = Vector3(0.0f, 0.0f, 0.0f);
    gril->transform->scale = Vector3(0.01f);

    GameObject *camera_obj = GameObject::Camera();
    camera_obj->name = "Camera";
    camera_obj->transform->position = Vector3(0.0f, 0.0f, 4.0f);
    Camera *camera = (Camera *)camera_obj->GetComponent("Camera");
    camera->width = SCR_WIDTH;
    camera->height = SCR_HEIGHT;
    
    zero->Add(gril);
    scene->Add(zero);
    scene->Add(camera_obj);

    scene->Init();

    while (!window->ShouldClose())
    {
        Time::UpdateClocker();
        window->ProcessInput(zero, camera_obj);
        
        zero->transform->rotation.y += 30.0f * Time::deltaTime;

        // 渲染循环 ... 
        scene->Update();
        scene->Render(); 

        window->PollEvent();
        window->SwapBuffers();
    }

    window->Terminate();
    return 0;
}

#endif // RENDERER_TEST_SIMPLE_DRAW_H