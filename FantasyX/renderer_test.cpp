#define RENDERER_TEST_H
#ifdef RENDERER_TEST_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "context.h"
#include "scene.h"
#include "gameobject.h"
#include "clocker.h"
#include "resource_manager.h"
#include "material.h"


using namespace std;
using namespace fx;

const GLuint WINDOW_WIDTH = 1920;
const GLuint WINDOW_HEIGHT = 1080;

const GLuint CANVA_WIDTH = 1366;
const GLuint CANVA_HEIGHT = 768;

int main()
{
    fxWindow *window = new fxWindow();
    window->width = WINDOW_WIDTH;
    window->height = WINDOW_HEIGHT;
    window->Init(true, 4, 0);

#pragma region 初始化imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // 设置ImGui风格
    ImGui::StyleColorsDark();

    // 为ImGui绑定渲染平台
    const char *glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = false;
#pragma endregion

    vector<string> skyboxFaces
    {
        "D:\\OpenGLAssets\\Images\\Skybox\\mountain_and_lake\\right.jpg",
        "D:\\OpenGLAssets\\Images\\Skybox\\mountain_and_lake\\left.jpg",
        "D:\\OpenGLAssets\\Images\\Skybox\\mountain_and_lake\\top.jpg",
        "D:\\OpenGLAssets\\Images\\Skybox\\mountain_and_lake\\bottom.jpg",
        "D:\\OpenGLAssets\\Images\\Skybox\\mountain_and_lake\\front.jpg",
        "D:\\OpenGLAssets\\Images\\Skybox\\mountain_and_lake\\back.jpg"
    };

    Scene *scene = new Scene();
    ResourceManager *res = ResourceManager::GetInstance();
    Renderer *renderer = new Renderer();
    renderer->canvaWidth = CANVA_WIDTH;
    renderer->canvaHeight = CANVA_HEIGHT;
    renderer->drawMode = DRAW_WITH_FACES;
    renderer->finalDisplay = DISPLAY_ALL;
    renderer->bloomWidth = 2;
    renderer->IBL = true;
    renderer->shadowOn = true;
    renderer->exposure = 0.5f;
    renderer->dirLightShadowMapWidth = 2048;
    renderer->dirLightShadowBias = 0.001f;
    renderer->hdrTexture = res->LoadHdrTexture("D:\\OpenGLAssets\\Images\\HDR\\Brooklyn_Bridge_Planks\\Brooklyn_Bridge_Planks_2k.hdr");
    renderer->skyboxTexture = res->LoadCubeMap(skyboxFaces);
    scene->renderer = renderer;

    GameObject *zero = new GameObject();
    zero->name = "ZeroObject";
    zero->transform->position = Vector3(0.0f, 0.0f, 0.0f);

    GameObject *model = res->LoadModel("D:\\OpenGLAssets\\Models\\tiger-white\\Tiger_.fbx");
    //GameObject *model = res->LoadModel("D:\\OpenGLAssets\\Models\\Cerberus\\fire_gun.obj");
    //GameObject *model = GameObject::Cube();
    model->name = "model";
    model->transform->position = Vector3(0.0f, 0.0f, 0.0f);
    GLfloat model_scale = 0.0f;

    /*
    PBRStandardMaterial *model_standard_mat = new PBRStandardMaterial();
    model_standard_mat->textures2D.push_back(res->LoadTexture2D("D:\\OpenGLAssets\\Models\\Cerberus\\Textures\\Cerberus_A.tga", "_AlbedoMap", true));
    model_standard_mat->textures2D.push_back(res->LoadTexture2D("D:\\OpenGLAssets\\Models\\Cerberus\\Textures\\Cerberus_M.tga", "_MetallicMap"));
    model_standard_mat->textures2D.push_back(res->LoadTexture2D("D:\\OpenGLAssets\\Models\\Cerberus\\Textures\\Cerberus_N.tga", "_NormalMap"));
    model_standard_mat->textures2D.push_back(res->LoadTexture2D("D:\\OpenGLAssets\\Models\\Cerberus\\Textures\\Cerberus_R.tga", "_RoughnessMap"));
    model_standard_mat->textures2D.push_back(res->LoadTexture2D("D:\\OpenGLAssets\\Models\\Cerberus\\Textures\\Cerberus_AO.tga", "_AoMap"));
    model->SetMaterial(model_standard_mat);
    */
    
    // Tiger 模型相关设置
    model_scale = 0.005f;
    model->transform->scale = Vector3(model_scale);
    model->transform->position.y = -1.089f;
    model->transform->rotation.y = 90.0f;
    PBRSimpleMaterial *model_simple_mat = new PBRSimpleMaterial();
    model_simple_mat->metallic = 0.012f;
    model_simple_mat->roughness = 0.512f;
    model_simple_mat->baseColor = Vector3(1.0f);
    model->SetMaterial(model_simple_mat);
    

    GameObject *base = GameObject::Cube();
    base->name = "base";
    base->transform->position = Vector3(0.0f, -1.0f, 0.0f);
    base->transform->scale = Vector3(5.0f, 0.05f, 5.0f);
    PBRSimpleMaterial *base_simple_mat = new PBRSimpleMaterial();
    base_simple_mat->metallic = 0.01f;
    base_simple_mat->roughness = 0.95f;
    base->SetMaterial(base_simple_mat);

    GameObject *dirLight = GameObject::DirLight();
    dirLight->transform->position = Vector3(1.5f);
    dirLight->transform->rotation.x = -40.0f;
    dirLight->transform->rotation.y = 230.0f;
    ((DirLight *)dirLight->GetComponent("Light"))->strength = 5.0f;

    GameObject *pointLightZero = new GameObject();
    pointLightZero->name = "PointLightZero";
    pointLightZero->transform->position = Vector3(0.0f, 2.6f, 0.0f);
    bool p_light_rotate = false;
    GLfloat rotateSpeed = 1.0f;
    GLfloat p_light_radius = 3.0f;
    GameObject *pointLight1 = GameObject::PointLight();
    GameObject *pointLight2 = GameObject::PointLight();
    GameObject *pointLight3 = GameObject::PointLight();

    GameObject *camera_obj = GameObject::Camera();
    camera_obj->name = "Camera";
    camera_obj->transform->position = Vector3(0.0f, 0.5f, 2.0f);
    camera_obj->transform->rotation.x = 0.0f;
    camera_obj->transform->rotation.y = -90.0f;
    Camera *camera = (Camera *)camera_obj->GetComponent("Camera");
    
    zero->Add(model);
    zero->Add(base);

    pointLightZero->Add(pointLight1);
    pointLightZero->Add(pointLight2);
    pointLightZero->Add(pointLight3);

    scene->Add(zero);
    scene->Add(camera_obj);
    scene->Add(dirLight);
    scene->Add(pointLightZero);

    scene->Init();

    GLfloat timer = 0.0f;

    while (!window->ShouldClose())
    {
        Time::UpdateClocker();

        pointLight1->transform->position = Vector3(-p_light_radius, 0.0f, -p_light_radius);
        pointLight2->transform->position = Vector3(0.0f, 0.0f, p_light_radius);
        pointLight3->transform->position = Vector3(p_light_radius, 0.0f, -p_light_radius);
        if (p_light_rotate)
        {
            pointLightZero->transform->rotation.y += 30.0f * Time::deltaTime * rotateSpeed;
        }

        model->transform->scale = Vector3(model_scale);

        if (model->visible)
            model->SetVisible(true);
        else
            model->SetVisible(false);

        if (pointLightZero->visible)
            pointLightZero->SetVisible(true);
        else
            pointLightZero->SetVisible(false);

        // 渲染循环 ... 
        scene->Update();
        scene->Render();

        window->Clear(Vector3(0.2f));

#pragma region 绘制GUI

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

#pragma region 渲染设置
        {
            ImGui::Begin("Setting");
            ImGui::Checkbox("Help Window", &show_demo_window);
            ImGui::Text("Render Status --------------------------------");
            ImGui::Text("Aplication average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Vertices %d", Renderer::vertices);
            ImGui::Text("Faces %d", Renderer::faces);

            ImGui::Text("Render ---------------------------------------");
            ImGui::SliderFloat("Gamma", &renderer->gamma, 0.0f, 2.2f);
            ImGui::SliderFloat("Exposure", &renderer->exposure, 0.0f, 1.0f);
            ImGui::ColorEdit3("BackColor", (GLfloat*)&renderer->backGroundColor);
            ImGui::Text("Draw with: ");
            ImGui::SameLine();
            if (ImGui::Button("DOTS"))
                renderer->drawMode = DRAW_WITH_DOTS;
            ImGui::SameLine();
            if (ImGui::Button("LINES"))
                renderer->drawMode = DRAW_WITH_LINES;
            ImGui::SameLine();
            if (ImGui::Button("FACES"))
                renderer->drawMode = DRAW_WITH_FACES;

            ImGui::Text("Bloom: ");
            ImGui::SameLine();
            if (ImGui::Button("x0"))
                renderer->bloomLevel = 0;
            ImGui::SameLine();
            if (ImGui::Button("x2"))
                renderer->bloomLevel = 2;
            ImGui::SameLine();
            if (ImGui::Button("x4"))
                renderer->bloomLevel = 4;
            ImGui::SameLine();
            if (ImGui::Button("x6"))
                renderer->bloomLevel = 6;
            ImGui::SameLine();
            if (ImGui::Button("x8"))
                renderer->bloomLevel = 8;
            ImGui::SameLine();
            if (ImGui::Button("x16"))
                renderer->bloomLevel = 16;

            ImGui::Text("Display: ");
            ImGui::SameLine();
            if (ImGui::Button("ALL"))
                renderer->finalDisplay = DISPLAY_ALL;
            ImGui::SameLine();
            if (ImGui::Button("ALBEDO"))
                renderer->finalDisplay = DISPLAY_ONLY_ALBEDO_MAP;
            ImGui::SameLine();
            if (ImGui::Button("DEPTH"))
                renderer->finalDisplay = DISPLAY_ONLY_DEPTH_MAP;
            ImGui::SameLine();
            if (ImGui::Button("METTALLIC"))
                renderer->finalDisplay = DISPLAY_ONLY_METALLIC_MAP;
            if (ImGui::Button("NORMAL"))
                renderer->finalDisplay = DISPLAY_ONLY_NORMAL_MAP;
            ImGui::SameLine();
            if (ImGui::Button("ROUGHNESS"))
                renderer->finalDisplay = DISPLAY_ONLY_ROUGHNESS_MAP;
            ImGui::SameLine();
            if (ImGui::Button("POS"))
                renderer->finalDisplay = DISPLAY_ONLY_POS_MAP;
            ImGui::SameLine();
            if (ImGui::Button("AO"))
                renderer->finalDisplay = DISPLAY_ONLY_AO_MAP;

            ImGui::Text("PostType: ");
            ImGui::SameLine();
            if (ImGui::Button("NONE"))
                renderer->postProcessing = POST_NONE;
            ImGui::SameLine();
            if (ImGui::Button("REVERSAL"))
                renderer->postProcessing = POST_REVERSAL;
            ImGui::SameLine();
            if (ImGui::Button("GREY"))
                renderer->postProcessing = POST_GREY;
            ImGui::SameLine();
            if (ImGui::Button("SHARPEN"))
                renderer->postProcessing = POST_SHARPEN;
            if (ImGui::Button("BLUR"))
                renderer->postProcessing = POST_BLUR;
            ImGui::SameLine();
            if (ImGui::Button("EDGE_DETECTION"))
                renderer->postProcessing = POST_EDGE_DETECTION;
            ImGui::SliderFloat("PostStrength", &renderer->postStrength, 0.0f, 100.0f);

            ImGui::Text("Camera ---------------------------------------");
            ImGui::Text("Projection: ");
            ImGui::SameLine();
            if (ImGui::Button("PERSPECTIVE"))
                camera->projection = CAM_PERSPECTIVE;
            ImGui::SameLine();
            if (ImGui::Button("ORTHOGRAPHIC"))
                camera->projection = CAM_ORTHOGRAPHIC;


            ImGui::Text("Light ----------------------------------------");
            ImGui::Checkbox("IBL", &renderer->IBL);
            ImGui::Text("DirLight Setting: ");
            ImGui::Checkbox("DirLightOn", &dirLight->visible);
            ImGui::Checkbox("DirLightShadowOn", &renderer->shadowOn);
            ImGui::SliderFloat("DLightShadowBias", &renderer->dirLightShadowBias, 0.0f, 0.01f);
            ImGui::SliderFloat3("DLightPos", (GLfloat *)&dirLight->transform->position, -10.0f, 10.0f);
            ImGui::ColorEdit3("DLightColor", (GLfloat *)&(((DirLight *)(dirLight->GetComponent("Light")))->color));
            ImGui::SliderFloat3("DLightRotate", (GLfloat *)&dirLight->transform->rotation, -360.0f, 360.0f);
            ImGui::SliderFloat("DLightStrength", &(((DirLight *)(dirLight->GetComponent("Light")))->strength), 0.0f, 5.0f);

            ImGui::Text("PointLight Setting: ");
            ImGui::Checkbox("PointLightsOn", &pointLightZero->visible);
            ImGui::Checkbox("PLightRotate", &p_light_rotate);
            ImGui::SliderFloat("PLRotateSpeed", &rotateSpeed, 0.0f, 6.0f);
            ImGui::SliderFloat("PlightRadius", &p_light_radius, 1.0f, 10.0f);
            ImGui::SliderFloat("PLightHeight", &pointLightZero->transform->position.y, 0.0f, 10.0f);
            ImGui::ColorEdit3("P1Color", (GLfloat *)&(((PointLight *)(pointLight1->GetComponent("Light")))->color));
            ImGui::ColorEdit3("P2Color", (GLfloat *)&(((PointLight *)(pointLight2->GetComponent("Light")))->color));
            ImGui::ColorEdit3("P3Color", (GLfloat *)&(((PointLight *)(pointLight3->GetComponent("Light")))->color));

            ImGui::Text("Model ----------------------------------------");
            ImGui::Text("Model Setting: ");
            ImGui::Checkbox("ModelVisible", &model->visible);
            ImGui::SliderFloat("ModelPosY", &model->transform->position.y, -2.0f, 3.0f);
            ImGui::SliderFloat("ModelRoateY", &model->transform->rotation.y, 0.0f, 360.0f);
            ImGui::SliderFloat("ModelScale", &model_scale, 0.0f, 1.0f);

            ImGui::ColorEdit3("ModelColor", (GLfloat *)&model_simple_mat->baseColor);
            ImGui::SliderFloat("ModelMetallic", &model_simple_mat->metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("ModelRoughness", &model_simple_mat->roughness, 0.0f, 1.0f);

            ImGui::Text("Base Setting: ");
            ImGui::SliderFloat3("BaseRoate", (GLfloat *)&base->transform->rotation, 0.0f, 360.0f);
            ImGui::SliderFloat3("BaseScale", (GLfloat *)&base->transform->scale, 0.0f, 100.0f);
            ImGui::ColorEdit3("BaseColor", (GLfloat *)&base_simple_mat->baseColor);
            ImGui::SliderFloat("BaseMetallic", &base_simple_mat->metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("BaseRoughness", &base_simple_mat->roughness, 0.0f, 1.0f);

            ImGui::End();
        }
#pragma endregion

#pragma region 场景

        {
            ImGui::Begin("Scene");
            if (ImGui::IsWindowFocused())
                window->ProcessInput(zero, camera_obj);
            ImGui::SetWindowSize(ImVec2(renderer->canvaWidth, renderer->canvaHeight));
            ImVec2 scene_window_pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddImage((void *)renderer->buffer[DISPLAY_BUFFER]->colorAttachments.front()->id, scene_window_pos,
                ImVec2(scene_window_pos.x + renderer->canvaWidth, scene_window_pos.y + renderer->canvaHeight), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        }

#pragma endregion

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

        window->PollEvent();
        window->SwapBuffers();
    }

    window->Terminate();
    return 0;
}

#endif // RENDERER_TEST_SIMPLE_DRAW_H