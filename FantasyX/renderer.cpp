#include "renderer.h"
#include "component.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace fx;

extern Vector3 WORLD_UP;
extern Vector3 WORLD_RIGHT;
extern Vector3 WORLD_FRONT;

GLuint Renderer::vertices = 0;
GLuint Renderer::faces = 0;

Renderer::Renderer()
{
    this->bloomLevel = 6;
    this->bloomWidth = 1;
    this->IBL = false;
    this->shadowOn = false;
    this->softShadow = false;
    this->dirLightShadowMapWidth = 10.0f;
    this->dirLightViewNear = 0.1f;
    this->dirLightViewFar = 1000.0f;
    this->drawMode = DRAW_WITH_FACES;
    this->exposure = 1.0f;
    this->finalDisplay = DISPLAY_ALL;
    this->fxaa = 0;
    this->gamma = 2.2f;
    this->postProcessing = POST_NONE;
    this->windowWith = 1280;
    this->windowHeight = 720;
    this->canvaHeight = windowWith;
    this->canvaWidth = windowHeight;
    this->backGroundColor = Vector4(0.2f, 0.3f, 0.3f, 1.0f);
    this->publicHdrIBLTextures = nullptr;

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
    vertices = 0;
    faces = 0;
    BeforeRender();
    GeomatryMapping();
    ShadowMapCalculate();
    Lighting();
    ForwardRender();
    CatchBright();
    GaussianBlur();
    Bloom();
    Fxaa();
    Display();
    AfterRender();
}

void Renderer::InorderTraverse(BiTreeNode<GameObject *> *node, Shader *shader = nullptr)
{
    if (node ==  nullptr)
        return;

    if (shader == nullptr)
    {
        InorderTraverse(node->left);

        node->data->Draw();

        InorderTraverse(node->right);
    }
    else
    {
        InorderTraverse(node->left, shader);

        node->data->Draw(shader);

        InorderTraverse(node->right, shader);
    }
    
}

void Renderer::ReverseInorderTraverse(BiTreeNode<GameObject *> *node, Shader *shader = nullptr)
{
    if (node == nullptr)
        return;

    if (shader == nullptr)
    {
        ReverseInorderTraverse(node->right);

        node->data->Draw();

        ReverseInorderTraverse(node->left);
    }
    else
    {
        ReverseInorderTraverse(node->right, shader);

        node->data->Draw(shader);

        ReverseInorderTraverse(node->left, shader);
    }
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

    Camera *camera = (Camera *)sceneData.cameras->front()->GetComponent("Camera");
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

    GLuint i = 500;
    switch (camera->projection)
    {
    case CAM_PERSPECTIVE:
        projection = glm::perspective(glm::radians(camera->fov), (GLfloat)camera->width / (GLfloat)camera->height, camera->near, camera->far);
        break;
    case CAM_ORTHOGRAPHIC:
        projection = glm::ortho(-(GLfloat)(camera->width / i), (GLfloat)(camera->width / i),
            -(GLfloat)(camera->height / i), (GLfloat)(camera->height / i), camera->near, camera->far);
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

        // ！！！注意法线贴图缓冲必须为高动态范围，不然负值会被截断为0
        normal->internalFormat = GL_RGB16F;
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
    glClearColor(backGroundColor.x, backGroundColor.y, backGroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameObject *camera = sceneData.cameras->front();
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
#pragma region 初始化阴影贴图绘制缓冲

    if (buffer[SHADOWMAP_CALCULATE_BUFFER] == nullptr)
    {
        buffer[SHADOWMAP_CALCULATE_BUFFER] = new FrameBuffer();
        buffer[SHADOWMAP_CALCULATE_BUFFER]->CreateBuffer();

        Texture2D *shadowMap = new Texture2D();
        shadowMap->internalFormat = GL_DEPTH_COMPONENT;
        shadowMap->width = canvaWidth;
        shadowMap->height = canvaHeight;
        shadowMap->dataFormat = GL_DEPTH_COMPONENT;
        shadowMap->minFilter = GL_NEAREST;
        shadowMap->magFilter = GL_NEAREST;
        shadowMap->wrapS = GL_CLAMP_TO_BORDER;
        shadowMap->wrapT = GL_CLAMP_TO_BORDER;
        shadowMap->CreateBuffer();
        buffer[SHADOWMAP_CALCULATE_BUFFER]->AddTexture2D(shadowMap, DEPTH_ATTACHMENT);

        buffer[SHADOWMAP_CALCULATE_BUFFER]->DeclearBuffer();
        buffer[SHADOWMAP_CALCULATE_BUFFER]->CheckCompleteness();
    }

#pragma endregion

    // 如果满足条件才绘制
    if (sceneData.dirLights->size() > 0 && shadowOn && (finalDisplay == DISPLAY_ALL || finalDisplay == DISPLAY_ALL_NO_POST_EFFECT))
    {
        // 使用阴影贴图绘制缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, buffer[SHADOWMAP_CALCULATE_BUFFER]->id);
        glViewport(0, 0, canvaWidth, canvaHeight);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        // 计算定向光源位置的变换矩阵
        Matrix4x4 dirLightProjection = glm::ortho(-(dirLightShadowMapWidth / 2), dirLightShadowMapWidth / 2, -(dirLightShadowMapWidth * canvaHeight / canvaWidth), 
            dirLightShadowMapWidth * canvaHeight / canvaWidth, dirLightViewNear, dirLightViewFar);
        Matrix4x4 dirLightView = glm::lookAt(sceneData.dirLights->front()->transform->worldPos, Vector3(0.0f), WORLD_UP);

        DirLightShadowShader *shader = DirLightShadowShader::GetInstance();
        shader->Bind();
        shader->SetMat4("lightSpaceMatrix", dirLightProjection * dirLightView);

        // 确定遍历顺序
        GLfloat r = glm::dot(sceneData.dirLights->front()->transform->front, WORLD_FRONT);
        if (r > 0)
        {
            ReverseInorderTraverse(sceneData.opaqueTree->root, shader);
        }
        else
        {
            InorderTraverse(sceneData.opaqueTree->root, shader);
        }

        shader->Unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Renderer::Lighting()
{
#pragma region 初始化光照处理帧缓冲

    if (buffer[LIGHTING_BUFFER] == nullptr)
    {
        buffer[LIGHTING_BUFFER] = new FrameBuffer();
        buffer[LIGHTING_BUFFER]->CreateBuffer();

        Texture2D *lighting = new Texture2D();
        lighting->internalFormat = GL_RGB16F;
        lighting->width = canvaWidth;
        lighting->height = canvaHeight;
        lighting->dataFormat = GL_RGB;
        lighting->minFilter = GL_NEAREST;
        lighting->magFilter = GL_NEAREST;
        lighting->CreateBuffer();
        buffer[LIGHTING_BUFFER]->AddTexture2D(lighting, COLOR_ATTACHMENT);

        buffer[LIGHTING_BUFFER]->DeclearBuffer();
        buffer[LIGHTING_BUFFER]->CheckCompleteness();
    }

    if (buffer[IBL_BUFFER] == nullptr)
    {
        buffer[IBL_BUFFER] = new FrameBuffer();
        buffer[IBL_BUFFER]->CreateBuffer();

        RenderBuffer *rbo = new RenderBuffer();
        rbo->width = canvaWidth;
        rbo->height = canvaHeight;
        rbo->internalFormat = GL_DEPTH_COMPONENT24;
        rbo->CreateBuffer();
        buffer[IBL_BUFFER]->AddRenderBuffer(rbo, DEPTH_ATTACHMENT);

    }

#pragma endregion

    HdrIBLTextures *currentHdrIBLTextures = nullptr;
    // 如果功能开启而且当前显示的纹理不为空，则试图找到对应保存的IBL贴图，没找到则绘制
    if (IBL && hdrTexture.id != 0)
    {
        auto got = hdrIBLTextureStorage.find(hdrTexture.path.c_str());

        // 没找到就绘制
        if (got == hdrIBLTextureStorage.end())
        {
            HdrIBLTextures *newHdrIBLTextures = new HdrIBLTextures();
            newHdrIBLTextures->hdrTexture = this->hdrTexture;
            newHdrIBLTextures->CreateTextures();
            newHdrIBLTextures->DrawTextures(buffer[IBL_BUFFER], (RenderBuffer *)buffer[IBL_BUFFER]->depthAttachment);
            currentHdrIBLTextures = newHdrIBLTextures;

            std::pair<string, HdrIBLTextures *> p(hdrTexture.path.c_str(), newHdrIBLTextures);
            hdrIBLTextureStorage.insert(p);
        }
        else  // 找到则直接使用
        {
            currentHdrIBLTextures = got._Ptr->_Myval.second;
        }

        for (GLuint i = G_TEX_NUM, j = IBL_IRRADIANCE_MAP; j < IBL_MAP_NUM; ++i, ++j)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(currentHdrIBLTextures->IBLTextures[j]->type, currentHdrIBLTextures->IBLTextures[j]->id);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffer[LIGHTING_BUFFER]->id);
    glViewport(0, 0, canvaWidth, canvaHeight);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    for (GLuint i = 0; i < G_TEX_NUM; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments.at(i)->id);
    }

    PBRLightingShader *shader = PBRLightingShader::GetInstance();
    shader->Bind();
    shader->SetVec3("viewPos", sceneData.cameras->front()->transform->worldPos);
    shader->SetVec4("backColor", backGroundColor);
    if (IBL && currentHdrIBLTextures != nullptr && currentHdrIBLTextures->textureBeenFilled)
    {
        shader->SetBool("IBL", true);
        publicHdrIBLTextures = currentHdrIBLTextures;
    }   
    else
        shader->SetBool("IBL", false);

    if (sceneData.dirLights->size() > 0 && sceneData.dirLights->front()->visible)
    {
        // 设置定向光
        shader->SetVec3("dirLight.color", ((DirLight *)(sceneData.dirLights->front()->GetComponent("Light")))->color);
        shader->SetVec3("dirLight.direction", ((DirLight *)(sceneData.dirLights->front()->GetComponent("Light")))->direction);
        shader->SetFloat("dirLight.strength", ((DirLight *)(sceneData.dirLights->front()->GetComponent("Light")))->strength);
        shader->SetBool("dirLightOn", true);
    }
    else
    {
        shader->SetBool("dirLightOn", false);
    }

    if (sceneData.pointLights->size() > 0)
    {
        GLint pointLightNum = 0;
        auto iter = sceneData.pointLights->cbegin();
        for (; iter != sceneData.pointLights->cend(); iter++)
        {
            if ((*iter)->visible)
            {
                shader->SetVec3(("pointLights[" + std::to_string(pointLightNum) + "].color").c_str(), ((PointLight *)((*iter)->GetComponent("Light")))->color);
                shader->SetVec3(("pointLights[" + std::to_string(pointLightNum) + "].position").c_str(), ((PointLight *)((*iter)->GetComponent("Light")))->position);
                shader->SetFloat(("pointLights[" + std::to_string(pointLightNum) + "].strength").c_str(), ((PointLight *)((*iter)->GetComponent("Light")))->strength);
                pointLightNum++;
                if (pointLightNum >= 8)
                    break;
            }
        }
        shader->SetInt("pointLightNum", pointLightNum);
    }
    else
    {
        shader->SetInt("pointLightNum", 0);
    }
    

    RectangleMesh *mesh = RectangleMesh::GetInstance();
    glBindVertexArray(mesh->VAO);

    if (mesh->indices.size() > 0)
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

    shader->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::ForwardRender()
{
    if (buffer[FORWARDRENDER_BUFFER] == nullptr)
    {
        buffer[FORWARDRENDER_BUFFER] = new FrameBuffer();
        buffer[FORWARDRENDER_BUFFER]->CreateBuffer();

        Texture2D *color = new Texture2D();
        color->width = canvaWidth;
        color->height = canvaHeight;
        color->minFilter = GL_NEAREST;
        color->magFilter = GL_NEAREST;
        color->CreateBuffer();
        buffer[FORWARDRENDER_BUFFER]->AddTexture2D(color, COLOR_ATTACHMENT);

        RenderBuffer *rbo = new RenderBuffer();
        rbo->width = canvaWidth;
        rbo->height = canvaHeight;
        rbo->CreateBuffer();
        buffer[FORWARDRENDER_BUFFER]->AddRenderBuffer(rbo, DEPTH_STENCIL_ATTACHMENT);

        buffer[FORWARDRENDER_BUFFER]->DeclearBuffer();
        buffer[FORWARDRENDER_BUFFER]->CheckCompleteness();
    }

    // 传送数据
    glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer[LIGHTING_BUFFER]->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer[FORWARDRENDER_BUFFER]->id);
    glBlitFramebuffer(0, 0, canvaWidth, canvaHeight, 0, 0, canvaWidth, canvaHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer[G_BUFFER]->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer[FORWARDRENDER_BUFFER]->id);
    glBlitFramebuffer(0, 0, canvaWidth, canvaHeight, 0, 0, canvaWidth, canvaHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
    
    glBindFramebuffer(GL_FRAMEBUFFER, buffer[FORWARDRENDER_BUFFER]->id);
    glViewport(0, 0, canvaWidth, canvaHeight);
    glEnable(GL_DEPTH_TEST);

    // 是否绘制天空盒子
    if (((Camera *)sceneData.cameras->front()->GetComponent("Camera"))->projection == CAM_PERSPECTIVE)
    {
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        // 绑定需要显示的天空盒子
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, publicHdrIBLTextures != nullptr ? publicHdrIBLTextures->IBLTextures[IBL_CUBE_MAP]->id : skyboxTexture.id);

        SkyBoxShader *skyboxShader = SkyBoxShader::GetInstance();
        skyboxShader->Bind();

        CubeMesh *mesh = CubeMesh::GetInstance();
        glBindVertexArray(mesh->VAO);

        if (mesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

        skyboxShader->Unbind();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
    }
}

void Renderer::CatchBright()
{
    if (buffer[BRIGHT_BUFFER] == nullptr)
    {

        buffer[BRIGHT_BUFFER] = new FrameBuffer();
        buffer[BRIGHT_BUFFER]->CreateBuffer();

        Texture2D *bright = new Texture2D();
        bright->internalFormat = GL_RGB16F;
        bright->width = canvaWidth / bloomWidth;
        bright->height = canvaHeight / bloomWidth;
        bright->dataFormat = GL_RGB;
        bright->minFilter = GL_LINEAR;
        bright->magFilter = GL_LINEAR;
        bright->CreateBuffer();
        buffer[BRIGHT_BUFFER]->AddTexture2D(bright, COLOR_ATTACHMENT);

        buffer[BRIGHT_BUFFER]->DeclearBuffer();
        buffer[BRIGHT_BUFFER]->CheckCompleteness();

    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffer[BRIGHT_BUFFER]->id);
    glViewport(0, 0, canvaWidth / bloomWidth, canvaHeight / bloomWidth);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer[FORWARDRENDER_BUFFER]->colorAttachments.front()->id);

    BrightCatchShader *shader = BrightCatchShader::GetInstance();
    shader->Bind();

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

void Renderer::GaussianBlur()
{
    if (buffer[PING_BUFFER] == nullptr && buffer[PONG_BUFFER] == nullptr)
    {
        buffer[PING_BUFFER] = new FrameBuffer();
        buffer[PING_BUFFER]->CreateBuffer();

        Texture2D *ping = new Texture2D();
        ping->internalFormat = GL_RGB16F;
        ping->width = canvaWidth / bloomWidth;
        ping->height = canvaHeight / bloomWidth;
        ping->dataFormat = GL_RGB;
        ping->minFilter = GL_LINEAR;
        ping->magFilter = GL_LINEAR;
        ping->CreateBuffer();
        buffer[PING_BUFFER]->AddTexture2D(ping, COLOR_ATTACHMENT);
        buffer[PING_BUFFER]->DeclearBuffer();
        buffer[PING_BUFFER]->CheckCompleteness();

        buffer[PONG_BUFFER] = new FrameBuffer();
        buffer[PONG_BUFFER]->CreateBuffer();

        Texture2D *pong = new Texture2D();
        pong->internalFormat = GL_RGB16F;
        pong->width = canvaWidth / bloomWidth;
        pong->height = canvaHeight / bloomWidth;
        pong->dataFormat = GL_RGB;
        pong->minFilter = GL_LINEAR;
        pong->magFilter = GL_LINEAR;
        pong->CreateBuffer();
        buffer[PONG_BUFFER]->AddTexture2D(pong, COLOR_ATTACHMENT);
        buffer[PONG_BUFFER]->DeclearBuffer();
        buffer[PONG_BUFFER]->CheckCompleteness();
    }

    GaussianBlurShader *shader = GaussianBlurShader::GetInstance();
    RectangleMesh *mesh = RectangleMesh::GetInstance();
    bool horizontal = true, first_blur = true;
    shader->Bind();
    for (GLuint i = 0; i < bloomLevel; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, buffer[PING_BUFFER + horizontal]->id);
        glViewport(0, 0, canvaWidth / bloomWidth, canvaHeight / bloomWidth);
        glDisable(GL_DEPTH_TEST);
        shader->SetBool("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_blur ? buffer[BRIGHT_BUFFER]->colorAttachments.front()->id : buffer[PING_BUFFER + (!horizontal)]->colorAttachments.front()->id);
        glBindVertexArray(mesh->VAO);
        if (mesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());
        glBindVertexArray(0);
        horizontal = !horizontal;
        if (first_blur)
            first_blur = false;
    }
}

void Renderer::Bloom()
{
    if (buffer[BLOOM_BUFFER] == nullptr)
    {
        buffer[BLOOM_BUFFER] = new FrameBuffer();
        buffer[BLOOM_BUFFER]->CreateBuffer();

        Texture2D *bloomLevel = new Texture2D();
        bloomLevel->internalFormat = GL_RGB16F;
        bloomLevel->width = canvaWidth;
        bloomLevel->height = canvaHeight;
        bloomLevel->dataFormat = GL_RGB;
        bloomLevel->minFilter = GL_LINEAR;
        bloomLevel->magFilter = GL_LINEAR;
        bloomLevel->CreateBuffer();
        buffer[BLOOM_BUFFER]->AddTexture2D(bloomLevel, COLOR_ATTACHMENT);

        buffer[BLOOM_BUFFER]->DeclearBuffer();
        buffer[BLOOM_BUFFER]->CheckCompleteness();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffer[BLOOM_BUFFER]->id);
    glViewport(0, 0, canvaWidth, canvaHeight);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer[FORWARDRENDER_BUFFER]->colorAttachments.front()->id);
    glActiveTexture(GL_TEXTURE1);
    GLuint blurTexture;
    if (bloomLevel % 2 == 0)
    {
        blurTexture = buffer[PING_BUFFER]->colorAttachments.front()->id;
    }
    else
    {
        blurTexture = buffer[PONG_BUFFER]->colorAttachments.front()->id;
    }
    glBindTexture(GL_TEXTURE_2D, blurTexture);

    BloomShader *shader = BloomShader::GetInstance();
    shader->Bind();
    
    RectangleMesh *mesh = RectangleMesh::GetInstance();
    glBindVertexArray(mesh->VAO);

    if (mesh->indices.size() > 0)
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void Renderer::Fxaa()
{

}

void Renderer::Display()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWith, windowHeight);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    //buffer[BLOOM_BUFFER]->colorAttachments.front()->id
    glActiveTexture(GL_TEXTURE0);
    bool displaySingleColor = false;
    switch (finalDisplay)
    {
    case DISPLAY_ALL:
        glBindTexture(GL_TEXTURE_2D, buffer[BLOOM_BUFFER]->colorAttachments.front()->id);
        break;
    case DISPLAY_ALL_NO_POST_EFFECT:
        glBindTexture(GL_TEXTURE_2D, buffer[BLOOM_BUFFER]->colorAttachments.front()->id);
        break;
    case DISPLAY_ONLY_ALBEDO_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_ALBEDO]->id);
        break;
    case DISPLAY_ONLY_AO_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_AO]->id);
        displaySingleColor = true;
        break;
    case DISPLAY_ONLY_METALLIC_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_METALLIC]->id);
        displaySingleColor = true;
        break;
    case DISPLAY_ONLY_NORMAL_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_NORMAL]->id);
        break;
    case DISPLAY_ONLY_ROUGHNESS_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_ROUGHNESS]->id);
        displaySingleColor = true;
        break;
    case DISPLAY_ONLY_POS_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_POS]->id);
        break;
    case DISPLAY_ONLY_DEPTH_MAP:
        glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments[G_TEX_DEPTH]->id);
        displaySingleColor = true;
        break;
    default:
        break;
    }

    DisplayShader *shader = DisplayShader::GetInstance();
    shader->Bind();
    shader->SetFloat("gamma", this->gamma);
    shader->SetFloat("exposure", this->exposure);
    shader->SetBool("displaySingleColor", displaySingleColor);
    shader->SetInt("postType", postProcessing);

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