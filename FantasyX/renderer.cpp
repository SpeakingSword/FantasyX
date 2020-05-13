#include "renderer.h"
#include "component.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace fx;

extern Vector3 WORLD_UP;
extern Vector3 WORLD_RIGHT;
extern Vector3 WORLD_FRONT;

Renderer::Renderer()
{
    this->bloom = 0;
    this->dirLightShadow = false;
    this->drawMode = DRAW_WITH_FACES;
    this->exposure = 1.0f;
    this->finalDisplay = DISPLAY_ALL;
    this->fxaa = 0;
    this->gamma = 2.2f;
    this->postProcessing = POST_NONE;
    this->canvaHeight = 720;
    this->canvaWidth = 1280;
    this->backGroundColor = Vector3(0.2f, 0.3f, 0.3f);

    for (GLuint i = 0; i < RENDERER_BUFFER_NUM; ++i)
    {
        buffer[i] = nullptr;
    }

    for (GLuint i = 0; i < SHADER_BUFFER_NUM; ++i)
    {
        shaderBuffer[i] = nullptr;
    }

    std::cout << "ENGIN CORE: Renderer created ... " << std::endl;
}

Renderer::~Renderer()
{
    for (GLuint i = 0; i < RENDERER_BUFFER_NUM; ++i)
    {
        if (buffer[i] != nullptr)
            buffer[i]->Destroy();
    }
    std::cout << "ENGIN CORE: Renderer destryed ... " << std::endl;
}

void Renderer::Destroy()
{
    this->~Renderer();
}

void Renderer::RenderScene(SceneData data)
{
    sceneData = data;
    BeforeRender();
    GeomatryMapping();
    ShadowMapCalculate();
    Lighting();
    ForwardRender();
    Bloom();
    Fxaa();
    this->PostProcessing();
    Display();
    AfterRender();
}

void Renderer::InorderTraverse(BiTreeNode<GameObject *> *node)
{
    if (node ==  nullptr)
        return;

    InorderTraverse(node->left);

    node->data->Draw();

    InorderTraverse(node->right);
}

void Renderer::ReverseInorderTraverse(BiTreeNode<GameObject *> *node)
{
    if (node->data == nullptr)
        return;

    ReverseInorderTraverse(node->right);

    node->data->Draw();

    ReverseInorderTraverse(node->left);
}

void Renderer::BeforeRender()
{
    // 状态开关
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // 更新着色器共享内存变量值
#pragma region UpdateShaderdMatrices

    if (shaderBuffer[SHARED_MATRICES] == nullptr)
    {
        shaderBuffer[SHARED_MATRICES] = new UniformBuffer();
        shaderBuffer[SHARED_MATRICES]->base = 0;
        shaderBuffer[SHARED_MATRICES]->size = sizeof(Matrix4x4) * 2;
        shaderBuffer[SHARED_MATRICES]->CreateBuffer();
        shaderBuffer[SHARED_MATRICES]->BindBase();
    }

    if (shaderBuffer[SHARED_CAMERA_VALUES] == nullptr)
    {
        shaderBuffer[SHARED_CAMERA_VALUES] = new UniformBuffer();
        shaderBuffer[SHARED_CAMERA_VALUES]->base = 1;
        shaderBuffer[SHARED_CAMERA_VALUES]->size = sizeof(GLfloat) * 2;
        shaderBuffer[SHARED_CAMERA_VALUES]->CreateBuffer();
        shaderBuffer[SHARED_CAMERA_VALUES]->BindBase();
    }

    Camera *camera = (Camera *)(*(sceneData.cameras->begin()))->GetComponent("Camera");
    Matrix4x4 view, projection;
    switch (camera->operationMode)
    {
    case CAM_FIRST_PERSON:
        view = camera->lookAt(camera->position, camera->position + camera->front, camera->up);
        break;
    case CAM_ORBIT:
        view = camera->lookAt(camera->position, Vector3(0.0f), camera->up);
        break;
    default:
        break;
    }

    switch (camera->projection)
    {
    case CAM_PERSPECTIVE:
        projection = glm::perspective(glm::radians(camera->fov), (GLfloat)camera->width / (GLfloat)camera->height, camera->near, camera->far);
        break;
    case CAM_ORTHOGRAPHIC:
        projection = glm::ortho(-(GLfloat)(camera->width / 2), (GLfloat)(camera->width / 2), -(GLfloat)(camera->height / 2), (GLfloat)(camera->height / 2));
        break;
    default:
        break;
    }


    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[SHARED_MATRICES]->id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4), glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4), sizeof(Matrix4x4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[SHARED_CAMERA_VALUES]->id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat), &(camera->near));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat), sizeof(GLfloat), &(camera->far));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

#pragma endregion
}

void Renderer::GeomatryMapping()
{
    // 初始化G缓冲
#pragma region Gbuffer Initiate
    if (buffer[G_BUFFER] == nullptr)
    {
        buffer[G_BUFFER] = new FrameBuffer();
        buffer[G_BUFFER]->CreateBuffer();

        Texture2D *pos = new Texture2D();
        pos->internalFormat = GL_RGB16F;
        pos->width = canvaWidth;
        pos->height = canvaHeight;
        pos->dataFormat = GL_RGB;
        pos->minFilter = GL_NEAREST;
        pos->magFilter = GL_NEAREST;
        pos->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(pos, COLOR_ATTACHMENT);

        Texture2D *depth = new Texture2D();
        depth->internalFormat = GL_R16F;
        depth->width = canvaWidth;
        depth->height = canvaHeight;
        depth->dataFormat = GL_RGB;
        depth->minFilter = GL_NEAREST;
        depth->magFilter = GL_NEAREST;
        depth->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(depth, COLOR_ATTACHMENT);
        
        Texture2D *albedo = new Texture2D();
        albedo->internalFormat = GL_RGB16F;
        albedo->width = canvaWidth;
        albedo->height = canvaHeight;
        albedo->dataFormat = GL_RGB;
        albedo->minFilter = GL_NEAREST;
        albedo->magFilter = GL_NEAREST;
        albedo->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(albedo, COLOR_ATTACHMENT);

        Texture2D *metallic = new Texture2D();
        metallic->internalFormat = GL_RED;
        metallic->width = canvaWidth;
        metallic->height = canvaHeight;
        metallic->dataFormat = GL_RGB;
        metallic->minFilter = GL_NEAREST;
        metallic->magFilter = GL_NEAREST;
        metallic->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(metallic, COLOR_ATTACHMENT);

        Texture2D *normal = new Texture2D();
        normal->internalFormat = GL_RGB;
        normal->width = canvaWidth;
        normal->height = canvaHeight;
        normal->dataFormat = GL_RGB;
        normal->minFilter = GL_NEAREST;
        normal->magFilter = GL_NEAREST;
        normal->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(normal, COLOR_ATTACHMENT);

        Texture2D *roughness = new Texture2D();
        roughness->internalFormat = GL_RED;
        roughness->width = canvaWidth;
        roughness->height = canvaHeight;
        roughness->dataFormat = GL_RGB;
        roughness->minFilter = GL_NEAREST;
        roughness->magFilter = GL_NEAREST;
        roughness->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(roughness, COLOR_ATTACHMENT);

        Texture2D *ao = new Texture2D();
        ao->internalFormat = GL_RED;
        ao->width = canvaWidth;
        ao->height = canvaHeight;
        ao->dataFormat = GL_RGB;
        ao->minFilter = GL_NEAREST;
        ao->magFilter = GL_NEAREST;
        ao->CreateBuffer();
        buffer[G_BUFFER]->AddTexture2D(ao, COLOR_ATTACHMENT);

        RenderBuffer *rbo = new RenderBuffer();
        rbo->width = canvaWidth;
        rbo->height = canvaHeight;
        rbo->CreateBuffer();
        buffer[G_BUFFER]->AddRenderBuffer(rbo, DEPTH_STENCIL_ATTACHMENT);

        buffer[G_BUFFER]->DeclearBuffer();
        buffer[G_BUFFER]->CheckCompleteness();
    }
#pragma endregion

    switch (drawMode)
    {
    case DRAW_WITH_DOTS:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case DRAW_WITH_LINES:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case DRAW_WITH_FACES:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }

    // 使用G缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, buffer[G_BUFFER]->id);

    glViewport(0, 0, canvaWidth, canvaHeight);
    glEnable(GL_DEPTH_TEST);
    glClearColor(backGroundColor.x,backGroundColor.y, backGroundColor.y, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto cam_p = sceneData.cameras->begin();
    GameObject *camera = *(cam_p);
    GLfloat r = glm::dot(camera->transform->front, WORLD_FRONT);

    // 如果摄像机与正z轴同向，则反中序遍历,否则反向，中序遍历
    if (r > 0)
    {
        ReverseInorderTraverse(sceneData.opaqueTree->root);
    }
    else
    {
        InorderTraverse(sceneData.opaqueTree->root);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::ShadowMapCalculate()
{

}

void Renderer::Lighting()
{

}

void Renderer::ForwardRender()
{

}

void Renderer::Bloom()
{

}

void Renderer::Fxaa()
{

}

void Renderer::PostProcessing()
{

}

void Renderer::Display()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, canvaWidth, canvaHeight);
    glDisable(GL_DEPTH_TEST);
    glClearColor(backGroundColor.x, backGroundColor.y, backGroundColor.y, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE0);
    switch (finalDisplay)
    {
    case DISPLAY_ALL:
        break;
    case DISPLAY_ALL_NO_POST_EFFECT:
        break;
    case DISPLAY_ONLY_ALBEDO_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_ALBEDO]->id);
        break;
    case DISPLAY_ONLY_AO_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_AO]->id);
        break;
    case DISPLAY_ONLY_METALLIC_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_METALLIC]->id);
        break;
    case DISPLAY_ONLY_NORMAL_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_NORMAL]->id);
        break;
    case DISPLAY_ONLY_ROUGHNESS_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_ROUGHNESS]->id);
        break;
    case DISPLAY_ONLY_POS_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_POS]->id);
        break;
    case DISPLAY_ONLY_DEPTH_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_DEPTH]->id);
        break;
    default:
        break;
    }

    DisplayShader *shader = DisplayShader::GetInstance();
    shader->Bind();
    shader->SetFloat("gamma", this->gamma);
    shader->SetFloat("exposure", this->exposure);

    RectangleMesh *mesh = RectangleMesh::GetInstance();
    glBindVertexArray(mesh->VAO);

    if (mesh->indices.size() > 0)
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

    shader->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void Renderer::AfterRender()
{
    
}