#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <vector>
#include "types.h"
#include <glad\glad.h>
using namespace std;

namespace fx {

    typedef enum {
        COLOR_ATTACHMENT,
        DEPTH_ATTACHMENT,
        STENCIL_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT
    };

    class TextureBuffer {
    public:
        GLuint id;
        GLuint width;
        GLuint height;
        GLuint internalFormat;
        GLuint type;
        
        virtual void CreateBuffer() = 0;
        virtual void ResetBuffer() = 0;
        virtual void ClearBuffer() = 0;
        virtual void Destroy() = 0;
    };

    class Texture2D : public TextureBuffer {
    public:
        bool mipmapOn;

        GLuint wrapS;
        GLuint wrapT;
        GLuint minFilter;
        GLuint magFilter;

        GLuint dataFormat;
        GLuint border;
        GLuint dataType;
        Vector4 borderColor;

        Texture2D();
        void CreateBuffer() override;
        void ResetBuffer() override;
        void ClearBuffer() override;
        void GenerateMipmap();
        void Destroy() override;
        
    private:
        ~Texture2D();
    };

    class Texture2DMultisample : public TextureBuffer {
    public:
        bool mipmapOn;

        GLuint wrapS;
        GLuint wrapT;
        GLuint minFilter;
        GLuint magFilter;

        GLuint samples;
        GLboolean fixedSampleLocations;

        Texture2DMultisample();
        void CreateBuffer() override;
        void ResetBuffer() override;
        void ClearBuffer() override;
        void GenerateMipmap();
        void Destroy() override;

    private:
        ~Texture2DMultisample();
    };

    class Texture3D : public TextureBuffer {
    public:
        bool mipmapOn;

        GLuint wrapS;
        GLuint wrapT;
        GLuint wrapR;
        GLuint minFilter;
        GLuint magFilter;

        GLuint dataFormat;
        GLuint border;
        GLuint dataType;
        Vector4 borderColor;
        
        Texture3D();
        void CreateBuffer() override;
        void ResetBuffer() override;
        void ClearBuffer() override;
        void GenerateMipmap();
        void Destroy() override;

    private:
        ~Texture3D();
    };

    class RenderBuffer : public TextureBuffer{
    public:
        RenderBuffer();
        void CreateBuffer() override;
        void ResetBuffer() override;
        void ClearBuffer() override;
        void Destroy() override;

    private:
        ~RenderBuffer();
    };

    class RenderBufferMultisample : public TextureBuffer {
    public:
        GLuint samples;
        RenderBufferMultisample();
        void CreateBuffer() override;
        void ResetBuffer() override;
        void ClearBuffer() override;
        void Destroy() override;

    private:
        ~RenderBufferMultisample();
    };

    class FrameBuffer {
    public:
        GLuint id;
        vector<TextureBuffer *> colorAttachments;
        GLuint colorAttachmentNum;
        TextureBuffer *depthAttachment;
        TextureBuffer *stencilAttachment;

        FrameBuffer();
        bool CheckCompleteness();
        void DeclearBuffer();
        // void Bind();
        // void Unbind();
        void CreateBuffer();
        void ClearBuffer();
        void ResetBuffer();
        void AddTexture2D(TextureBuffer *tex, GLuint attachment);
        void AddTexture3D(TextureBuffer *tex, GLuint attachment);
        void AddRenderBuffer(TextureBuffer *rbo, GLuint attachment);
        void ReleaseAllColorAttachment();
        void ReleaseDepthAttachment();
        void ReleaseStencilAttachment();
        void Destroy();

    private:
        ~FrameBuffer();
    };

    class UniformBuffer {
    public:
        GLuint id;
        GLuint size;
        GLuint base;

        UniformBuffer();
        void CreateBuffer();
        void ClearBuffer();
        void ResetBuffer();
        void BindBase();
        void Destroy();

    private:
        ~UniformBuffer();
    };
}

#endif // !FRAMEBUFFER_H