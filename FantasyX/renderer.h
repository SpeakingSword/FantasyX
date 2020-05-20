#ifndef RENDERER_H
#define RENDERER_H


#include <iostream>
#include <unordered_map>

#include "gameobject.h"
#include "texture.h"
#include "buffer.h"
#include "bitree.h"
#include "shader.h"
#include "types.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

using namespace std;

namespace fx {

    typedef struct {
        BiTree<GameObject *> *opaqueTree;
        BiTree<GameObject *> *transparentTree;
        list<GameObject *> *dirLights;
        list<GameObject *> *pointLights;
        list<GameObject *> *spotLights;
        list<GameObject *> *cameras;
    }SceneData;

    typedef enum {
        G_BUFFER,
        SHADOWMAP_CALCULATE_BUFFER,
        LIGHTING_BUFFER,
        IBL_BUFFER,
        FORWARDRENDER_BUFFER,
        BRIGHT_BUFFER,
        PING_BUFFER,
        PONG_BUFFER,
        BLOOM_BUFFER,
        FXAA_BUFFER,
        POSTPROCESSING_BUFFER,
        RENDERER_BUFFER_NUM
    }RendererBuffer;

    typedef enum {
        SHARED_MATRICES,
        SHARED_CAMERA_VALUES,
        SHADER_BUFFER_NUM
    }ShaderBuffer;

    typedef enum {
        G_TEX_POS,
        G_TEX_DEPTH,
        G_TEX_ALBEDO,
        G_TEX_METALLIC,
        G_TEX_NORMAL,
        G_TEX_ROUGHNESS,
        G_TEX_AO,
        G_TEX_NUM
    }GbufferTexture;
    
    class Renderer {
    private:
        SceneData sceneData;
        FrameBuffer *buffer[RENDERER_BUFFER_NUM];
        UniformBuffer *shaderBuffer[SHADER_BUFFER_NUM];
        unordered_map<string, HdrIBLTextures *> hdrIBLTextureStorage;
        HdrIBLTextures *publicHdrIBLTextures;

        ~Renderer();
        void BeforeRender();
        void GeomatryMapping();
        void ShadowMapCalculate();
        void Lighting();
        void ForwardRender();
        void CatchBright();
        void GaussianBlur();
        void Bloom();
        void Fxaa();
        void Display();
        void AfterRender();
        void InorderTraverse(BiTreeNode<GameObject *> *node, Shader *shader);
        void ReverseInorderTraverse(BiTreeNode<GameObject *> *node, Shader *shader);

    public:
        bool IBL;
        Texture hdrTexture;
        Texture skyboxTexture;
        GLuint bloomLevel;
        GLuint bloomWidth;
        GLuint fxaa;
        GLuint postProcessing;
        GLuint finalDisplay;

        GLfloat gamma;
        GLfloat exposure;

        GLuint drawMode;
        bool shadowOn;
        bool softShadow;
        GLfloat dirLightShadowMapWidth;
        GLfloat dirLightViewNear;
        GLfloat dirLightViewFar;

        GLuint canvaWidth;
        GLuint canvaHeight;
        GLuint windowWith;
        GLuint windowHeight;

        Vector4 backGroundColor;
        static GLuint vertices;
        static GLuint faces;

        Renderer();
        // 使用 SceneData 数据渲染
        void RenderScene(SceneData data);
        void Destroy();
    };
    
}

#endif // !RENDERER_H