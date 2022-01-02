#include "renderer.h"
#include "component.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

namespace fx {

    GLuint Renderer::vertices = 0;
    GLuint Renderer::faces = 0;

    Renderer::Renderer()
    {
        this->bloomLevel = 6;
        this->bloomWidth = 1;
        this->bloomStrength = 0.02f;
        this->singleColor = false;
        this->IBL = false;
        this->shadowOn = false;
        this->dirLightShadowMapWidth = 1024;
        this->dirLightViewNear = 0.1f;
        this->dirLightViewFar = 10.0f;
        this->dirLightShadowBias = 0.0f;
        this->drawMode = DRAW_WITH_FACES;
        this->exposure = 1.0f;
        this->finalDisplay = DISPLAY_ALL;
        this->fxaa = 0;
        this->gamma = 2.2f;
        this->postProcessing = POST_NONE;
        this->postStrength = 0.0f;
        this->canvaHeight = 1366;
        this->canvaWidth = 768;
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
        switch (finalDisplay)
        {
        case DISPLAY_ALL:
            ShadowMapCalculate();
            Lighting();
            ForwardRender();
            if (bloomLevel > 0)
            {
                CatchBright();
                GaussianBlur();
                Bloom();
            }
            Fxaa();
            singleColor = false;
            break;

        case DISPLAY_ONLY_ALBEDO_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_ALBEDO, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = false;
            break;

        case DISPLAY_ONLY_AO_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_AO, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = true;
            break;

        case DISPLAY_ONLY_METALLIC_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_METALLIC, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = true;
            break;

        case DISPLAY_ONLY_NORMAL_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_NORMAL, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = false;
            break;

        case DISPLAY_ONLY_ROUGHNESS_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_ROUGHNESS, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = true;
            break;

        case DISPLAY_ONLY_POS_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_POS, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = false;
            break;

        case DISPLAY_ONLY_DEPTH_MAP:
            FrameBuffer::BlitColorBuffer(buffer[G_BUFFER]->id, G_TEX_DEPTH, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
            singleColor = true;
            break;
        default:
            break;
        }
        Display();
        AfterRender();
    }

    void Renderer::InorderTraverse(BiTreeNode<GameObject*>* node, Shader* shader = nullptr)
    {
        if (node == nullptr)
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

    void Renderer::ReverseInorderTraverse(BiTreeNode<GameObject*>* node, Shader* shader = nullptr)
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

#pragma region 初始化过渡缓冲

        if (buffer[TRANSMITION_BUFFER] == nullptr)
        {
            buffer[TRANSMITION_BUFFER] = new FrameBuffer();
            buffer[TRANSMITION_BUFFER]->CreateBuffer();

            Texture2D* colorTex = new Texture2D();
            colorTex->width = canvaWidth;
            colorTex->height = canvaHeight;
            //colorTex->minFilter = GL_NEAREST;
            //colorTex->magFilter = GL_NEAREST;
            colorTex->CreateBuffer();
            buffer[TRANSMITION_BUFFER]->AddTexture2D(colorTex, COLOR_ATTACHMENT);

            buffer[TRANSMITION_BUFFER]->DeclearBuffer();
            buffer[TRANSMITION_BUFFER]->CheckCompleteness();
        }

#pragma endregion

#pragma region 更新着色器共享变量

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

        Camera* camera = (Camera*)sceneData.cameras->front()->GetComponent("Camera");
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
            projection = glm::perspective(glm::radians(camera->fov), (GLfloat)canvaWidth / (GLfloat)canvaHeight, camera->near, camera->far);
            break;
        case CAM_ORTHOGRAPHIC:
            projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, camera->near, camera->far);
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

#pragma region 设置OpenGL的状态

        // 状态开关
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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

            Texture2D* pos = new Texture2D();
            pos->internalFormat = GL_RGB16F;
            pos->width = canvaWidth;
            pos->height = canvaHeight;
            pos->dataFormat = GL_RGB;
            pos->minFilter = GL_NEAREST;
            pos->magFilter = GL_NEAREST;
            pos->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(pos, COLOR_ATTACHMENT);

            Texture2D* depth = new Texture2D();
            depth->internalFormat = GL_R16F;
            depth->width = canvaWidth;
            depth->height = canvaHeight;
            depth->dataFormat = GL_RGB;
            depth->minFilter = GL_NEAREST;
            depth->magFilter = GL_NEAREST;
            depth->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(depth, COLOR_ATTACHMENT);

            Texture2D* albedo = new Texture2D();
            albedo->internalFormat = GL_RGB16F;
            albedo->width = canvaWidth;
            albedo->height = canvaHeight;
            albedo->dataFormat = GL_RGB;
            albedo->minFilter = GL_NEAREST;
            albedo->magFilter = GL_NEAREST;
            albedo->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(albedo, COLOR_ATTACHMENT);

            Texture2D* metallic = new Texture2D();
            metallic->internalFormat = GL_RED;
            metallic->width = canvaWidth;
            metallic->height = canvaHeight;
            metallic->dataFormat = GL_RGB;
            metallic->minFilter = GL_NEAREST;
            metallic->magFilter = GL_NEAREST;
            metallic->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(metallic, COLOR_ATTACHMENT);

            Texture2D* normal = new Texture2D();

            // ！！！注意法线贴图缓冲必须为高动态范围，不然负值会被截断为0
            normal->internalFormat = GL_RGB16F;
            normal->width = canvaWidth;
            normal->height = canvaHeight;
            normal->dataFormat = GL_RGB;
            normal->minFilter = GL_NEAREST;
            normal->magFilter = GL_NEAREST;
            normal->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(normal, COLOR_ATTACHMENT);

            Texture2D* roughness = new Texture2D();
            roughness->internalFormat = GL_RED;
            roughness->width = canvaWidth;
            roughness->height = canvaHeight;
            roughness->dataFormat = GL_RGB;
            roughness->minFilter = GL_NEAREST;
            roughness->magFilter = GL_NEAREST;
            roughness->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(roughness, COLOR_ATTACHMENT);

            Texture2D* ao = new Texture2D();
            ao->internalFormat = GL_RED;
            ao->width = canvaWidth;
            ao->height = canvaHeight;
            ao->dataFormat = GL_RGB;
            ao->minFilter = GL_NEAREST;
            ao->magFilter = GL_NEAREST;
            ao->CreateBuffer();
            buffer[G_BUFFER]->AddTexture2D(ao, COLOR_ATTACHMENT);

            RenderBuffer* rbo = new RenderBuffer();
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
        buffer[G_BUFFER]->Bind();
        glViewport(0, 0, canvaWidth, canvaHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(backGroundColor.x, backGroundColor.y, backGroundColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GameObject* camera = sceneData.cameras->front();
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

        buffer[G_BUFFER]->Unbind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::ShadowMapCalculate()
    {
#pragma region 初始化阴影贴图绘制缓冲

        if (buffer[SHADOWMAP_CALCULATE_BUFFER] == nullptr)
        {
            buffer[SHADOWMAP_CALCULATE_BUFFER] = new FrameBuffer();
            buffer[SHADOWMAP_CALCULATE_BUFFER]->CreateBuffer();

            Texture2D* shadowMap = new Texture2D();
            shadowMap->internalFormat = GL_DEPTH_COMPONENT24;
            shadowMap->width = dirLightShadowMapWidth;
            shadowMap->height = dirLightShadowMapWidth;
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
        if (sceneData.dirLights->size() > 0 && shadowOn)
        {
            // 使用阴影贴图绘制缓冲
            buffer[SHADOWMAP_CALCULATE_BUFFER]->Bind();
            glViewport(0, 0, dirLightShadowMapWidth, dirLightShadowMapWidth);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);


            // 计算定向光源位置的变换矩阵
            GLfloat near = 0.1f, far = 10.0f;
            Matrix4x4 dirLightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near, far);
            Matrix4x4 dirLightView = glm::lookAt(sceneData.dirLights->front()->transform->worldPos, Vector3(0.0f), WORLD_UP);
            Matrix4x4 dirLightSpaceMatrix = dirLightProjection * dirLightView;

            DirLightShadowShader* shader = DirLightShadowShader::GetInstance();
            shader->Bind();
            shader->SetMat4("lightSpaceMatrix", dirLightSpaceMatrix);
            shader->SetFloat("NEAR", near);
            shader->SetFloat("FAR", far);

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
            buffer[SHADOWMAP_CALCULATE_BUFFER]->Unbind();

            // 顺便设置定向光的空间投影矩阵
            PBRLightingShader* lighting_shader = PBRLightingShader::GetInstance();
            lighting_shader->Bind();
            lighting_shader->SetMat4("dirLightSpaceMatrix", dirLightSpaceMatrix);
            lighting_shader->Unbind();
        }
    }

    void Renderer::Lighting()
    {
#pragma region 初始化光照处理帧缓冲

        if (buffer[LIGHTING_BUFFER] == nullptr)
        {
            buffer[LIGHTING_BUFFER] = new FrameBuffer();
            buffer[LIGHTING_BUFFER]->CreateBuffer();

            Texture2D* lighting = new Texture2D();
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

            RenderBuffer* rbo = new RenderBuffer();
            rbo->width = canvaWidth;
            rbo->height = canvaHeight;
            rbo->internalFormat = GL_DEPTH_COMPONENT24;
            rbo->CreateBuffer();
            buffer[IBL_BUFFER]->AddRenderBuffer(rbo, DEPTH_ATTACHMENT);

        }

#pragma endregion

        HdrIBLTextures* currentHdrIBLTextures = nullptr;
        // 如果功能开启而且当前显示的纹理不为空，则试图找到对应保存的IBL贴图，没找到则绘制
        if (IBL && hdrTexture.id != 0)
        {
            auto got = hdrIBLTextureStorage.find(hdrTexture.path.c_str());

            // 没找到就绘制
            if (got == hdrIBLTextureStorage.end())
            {
                HdrIBLTextures* newHdrIBLTextures = new HdrIBLTextures();
                newHdrIBLTextures->hdrTexture = this->hdrTexture;
                newHdrIBLTextures->CreateTextures();
                newHdrIBLTextures->DrawTextures(buffer[IBL_BUFFER], (RenderBuffer*)buffer[IBL_BUFFER]->depthAttachment);
                currentHdrIBLTextures = newHdrIBLTextures;

                std::pair<string, HdrIBLTextures*> p(hdrTexture.path.c_str(), newHdrIBLTextures);
                hdrIBLTextureStorage.insert(p);
            }
            else  // 找到则直接使用
            {
                currentHdrIBLTextures = got._Ptr->_Myval.second;
            }

            for (GLuint i = G_TEX_NUM, j = IBL_IRRADIANCE_MAP; j < IBL_MAP_NUM; ++i, ++j)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                GLuint type = currentHdrIBLTextures->IBLTextures[j]->type;
                glBindTexture(type, currentHdrIBLTextures->IBLTextures[j]->id);
            }
        }

        buffer[LIGHTING_BUFFER]->Bind();
        glViewport(0, 0, canvaWidth, canvaHeight);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        for (GLuint i = 0; i < G_TEX_NUM; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, buffer[G_BUFFER]->colorAttachments.at(i)->id);
        }

        PBRLightingShader* shader = PBRLightingShader::GetInstance();
        shader->Bind();
        shader->SetVec3("viewPos", sceneData.cameras->front()->transform->worldPos);

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
            shader->SetVec3("dirLight.color", ((DirLight*)(sceneData.dirLights->front()->GetComponent("Light")))->color);
            shader->SetVec3("dirLight.direction", ((DirLight*)(sceneData.dirLights->front()->GetComponent("Light")))->direction);
            shader->SetFloat("dirLight.strength", ((DirLight*)(sceneData.dirLights->front()->GetComponent("Light")))->strength);
            shader->SetBool("dirLightOn", true);

            if (shadowOn)
            {
                // 设置需要绘制阴影的变量
                glActiveTexture(GL_TEXTURE0 + 12);
                glBindTexture(GL_TEXTURE_2D, buffer[SHADOWMAP_CALCULATE_BUFFER]->depthAttachment->id);
                shader->SetBool("dirLightShadowOn", true);
                shader->SetFloat("dirLightShadowBias", dirLightShadowBias);
            }
            else
            {
                shader->SetBool("dirLightShadowOn", false);
            }
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
                    shader->SetVec3(("pointLights[" + std::to_string(pointLightNum) + "].color").c_str(), ((PointLight*)((*iter)->GetComponent("Light")))->color);
                    shader->SetVec3(("pointLights[" + std::to_string(pointLightNum) + "].position").c_str(), ((PointLight*)((*iter)->GetComponent("Light")))->position);
                    shader->SetFloat(("pointLights[" + std::to_string(pointLightNum) + "].strength").c_str(), ((PointLight*)((*iter)->GetComponent("Light")))->strength);
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


        RectangleMesh* mesh = RectangleMesh::GetInstance();
        glBindVertexArray(mesh->VAO);

        if (mesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

        shader->Unbind();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        buffer[LIGHTING_BUFFER]->Unbind();

    }

    void Renderer::ForwardRender()
    {
        if (buffer[FORWARDRENDER_BUFFER] == nullptr)
        {
            buffer[FORWARDRENDER_BUFFER] = new FrameBuffer();
            buffer[FORWARDRENDER_BUFFER]->CreateBuffer();

            Texture2D* color = new Texture2D();
            color->width = canvaWidth;
            color->height = canvaHeight;
            color->minFilter = GL_NEAREST;
            color->magFilter = GL_NEAREST;
            color->CreateBuffer();
            buffer[FORWARDRENDER_BUFFER]->AddTexture2D(color, COLOR_ATTACHMENT);

            RenderBuffer* rbo = new RenderBuffer();
            rbo->width = canvaWidth;
            rbo->height = canvaHeight;
            rbo->CreateBuffer();
            buffer[FORWARDRENDER_BUFFER]->AddRenderBuffer(rbo, DEPTH_STENCIL_ATTACHMENT);

            buffer[FORWARDRENDER_BUFFER]->DeclearBuffer();
            buffer[FORWARDRENDER_BUFFER]->CheckCompleteness();
        }

        FrameBuffer::BlitColorBuffer(buffer[LIGHTING_BUFFER]->id, 0, buffer[FORWARDRENDER_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
        FrameBuffer::BlitDepthBuffer(buffer[G_BUFFER]->id, buffer[FORWARDRENDER_BUFFER]->id, canvaWidth, canvaHeight, canvaWidth, canvaHeight);

        buffer[FORWARDRENDER_BUFFER]->Bind();
        glViewport(0, 0, canvaWidth, canvaHeight);
        glEnable(GL_DEPTH_TEST);

        // 是否绘制天空盒子
        if (((Camera*)sceneData.cameras->front()->GetComponent("Camera"))->projection == CAM_PERSPECTIVE)
        {
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LEQUAL);
            // 绑定需要显示的天空盒子
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, publicHdrIBLTextures != nullptr ? publicHdrIBLTextures->IBLTextures[IBL_IRRADIANCE_MAP]->id : skyboxTexture.id);

            SkyBoxShader* skyboxShader = SkyBoxShader::GetInstance();
            skyboxShader->Bind();

            CubeMesh* mesh = CubeMesh::GetInstance();
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

        buffer[FORWARDRENDER_BUFFER]->Unbind();

        FrameBuffer::BlitColorBuffer(buffer[FORWARDRENDER_BUFFER]->id, 0, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
    }

    void Renderer::CatchBright()
    {
        if (buffer[BRIGHT_BUFFER] == nullptr)
        {

            buffer[BRIGHT_BUFFER] = new FrameBuffer();
            buffer[BRIGHT_BUFFER]->CreateBuffer();

            Texture2D* bright = new Texture2D();
            bright->internalFormat = GL_RGB16F;
            bright->width = canvaWidth / bloomWidth;
            bright->height = canvaHeight / bloomWidth;
            bright->dataFormat = GL_RGB;
            bright->CreateBuffer();
            buffer[BRIGHT_BUFFER]->AddTexture2D(bright, COLOR_ATTACHMENT);

            buffer[BRIGHT_BUFFER]->DeclearBuffer();
            buffer[BRIGHT_BUFFER]->CheckCompleteness();

        }

        buffer[BRIGHT_BUFFER]->Bind();
        glViewport(0, 0, canvaWidth / bloomWidth, canvaHeight / bloomWidth);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer[TRANSMITION_BUFFER]->colorAttachments.front()->id);

        BrightCatchShader* shader = BrightCatchShader::GetInstance();
        shader->Bind();
        shader->SetFloat("exposure", this->exposure);

        RectangleMesh* mesh = RectangleMesh::GetInstance();
        glBindVertexArray(mesh->VAO);

        if (mesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

        shader->Unbind();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        buffer[BRIGHT_BUFFER]->Unbind();
    }

    void Renderer::GaussianBlur()
    {
        if (buffer[PING_BUFFER] == nullptr && buffer[PONG_BUFFER] == nullptr)
        {
            buffer[PING_BUFFER] = new FrameBuffer();
            buffer[PING_BUFFER]->CreateBuffer();

            Texture2D* ping = new Texture2D();
            ping->internalFormat = GL_RGB16F;
            ping->width = canvaWidth / bloomWidth;
            ping->height = canvaHeight / bloomWidth;
            ping->dataFormat = GL_RGB;
            ping->CreateBuffer();
            buffer[PING_BUFFER]->AddTexture2D(ping, COLOR_ATTACHMENT);
            buffer[PING_BUFFER]->DeclearBuffer();
            buffer[PING_BUFFER]->CheckCompleteness();

            buffer[PONG_BUFFER] = new FrameBuffer();
            buffer[PONG_BUFFER]->CreateBuffer();

            Texture2D* pong = new Texture2D();
            pong->internalFormat = GL_RGB16F;
            pong->width = canvaWidth / bloomWidth;
            pong->height = canvaHeight / bloomWidth;
            pong->dataFormat = GL_RGB;
            pong->CreateBuffer();
            buffer[PONG_BUFFER]->AddTexture2D(pong, COLOR_ATTACHMENT);
            buffer[PONG_BUFFER]->DeclearBuffer();
            buffer[PONG_BUFFER]->CheckCompleteness();
        }

        GaussianBlurShader* shader = GaussianBlurShader::GetInstance();
        RectangleMesh* mesh = RectangleMesh::GetInstance();
        bool horizontal = true, first_blur = true;
        shader->Bind();
        for (GLuint i = 0; i < bloomLevel; i++)
        {
            buffer[PING_BUFFER + horizontal]->Bind();
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

        buffer[PING_BUFFER]->Unbind();
    }

    void Renderer::Bloom()
    {
        if (buffer[BLOOM_BUFFER] == nullptr)
        {
            buffer[BLOOM_BUFFER] = new FrameBuffer();
            buffer[BLOOM_BUFFER]->CreateBuffer();

            Texture2D* bloomLevel = new Texture2D();
            bloomLevel->internalFormat = GL_RGB16F;
            bloomLevel->width = canvaWidth;
            bloomLevel->height = canvaHeight;
            bloomLevel->dataFormat = GL_RGB;
            bloomLevel->CreateBuffer();
            buffer[BLOOM_BUFFER]->AddTexture2D(bloomLevel, COLOR_ATTACHMENT);

            buffer[BLOOM_BUFFER]->DeclearBuffer();
            buffer[BLOOM_BUFFER]->CheckCompleteness();
        }

        buffer[BLOOM_BUFFER]->Bind();
        glViewport(0, 0, canvaWidth, canvaHeight);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
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

        BloomShader* shader = BloomShader::GetInstance();
        shader->Bind();
        shader->SetFloat("bloomStrength", bloomStrength);

        RectangleMesh* mesh = RectangleMesh::GetInstance();
        glBindVertexArray(mesh->VAO);

        if (mesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        buffer[BLOOM_BUFFER]->Unbind();
        glDisable(GL_BLEND);

        FrameBuffer::BlitColorBuffer(buffer[BLOOM_BUFFER]->id, 0, buffer[TRANSMITION_BUFFER]->id, 0, canvaWidth, canvaHeight, canvaWidth, canvaHeight);
    }

    void Renderer::Fxaa()
    {

    }

    void Renderer::Display()
    {
        if (buffer[DISPLAY_BUFFER] == nullptr)
        {
            buffer[DISPLAY_BUFFER] = new FrameBuffer();
            buffer[DISPLAY_BUFFER]->CreateBuffer();

            Texture2D* color = new Texture2D();
            color->internalFormat = GL_RGB;
            color->width = canvaWidth;
            color->height = canvaHeight;
            color->CreateBuffer();
            buffer[DISPLAY_BUFFER]->AddTexture2D(color, COLOR_ATTACHMENT);

            buffer[DISPLAY_BUFFER]->DeclearBuffer();
            buffer[DISPLAY_BUFFER]->CheckCompleteness();
        }

        buffer[DISPLAY_BUFFER]->Bind();
        glViewport(0, 0, canvaWidth, canvaHeight);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer[TRANSMITION_BUFFER]->colorAttachments.front()->id);

        DisplayShader* shader = DisplayShader::GetInstance();
        shader->Bind();
        shader->SetFloat("gamma", gamma);
        shader->SetFloat("exposure", exposure);
        shader->SetBool("displaySingleColor", singleColor);
        shader->SetInt("postType", postProcessing);
        shader->SetFloat("postStrength", postStrength);

        RectangleMesh* mesh = RectangleMesh::GetInstance();
        glBindVertexArray(mesh->VAO);

        if (mesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

        shader->Unbind();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        buffer[DISPLAY_BUFFER]->Unbind();
    }

    void Renderer::AfterRender()
    {

    }

}