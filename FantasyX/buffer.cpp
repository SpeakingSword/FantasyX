#include "buffer.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace fx;

#pragma region FrameBuffer

FrameBuffer::FrameBuffer()
{
    id = 0;
    colorAttachmentNum = 0;
    depthAttachment = nullptr;
    stencilAttachment = nullptr;
    std::cout << "FrameBuffer created ... " << std::endl;
}

FrameBuffer::~FrameBuffer()
{
    if (id != 0)
    {
        for (GLuint i = 0; i < colorAttachmentNum; ++i)
        {
            colorAttachments[i]->Destroy();
        }

        if (depthAttachment != nullptr)
            depthAttachment->Destroy();

        if (stencilAttachment != nullptr)
            stencilAttachment->Destroy();

        glDeleteFramebuffers(1, &id);
        id = 0;
        std::cout << "FrameBufefr destroyed ... " << std::endl;
    }
}

void FrameBuffer::Destroy()
{
    this->~FrameBuffer();
}

void FrameBuffer::Bind()
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }
}

void FrameBuffer::Unbind()
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::CreateBuffer()
{
    if (id == 0)
    {
        glGenFramebuffers(1, &id);
    }
}

void FrameBuffer::ResetBuffer()
{
    if (id != 0)
    {
        ReleaseAllColorAttachment();
        ReleaseDepthAttachment();
        ReleaseStencilAttachment();
    }
}

void FrameBuffer::ClearBuffer()
{
    if (id != 0)
    {
        GLbitfield mask;
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        if (colorAttachmentNum > 0)
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            mask = mask | GL_COLOR_BUFFER_BIT;
        }
            
        if (depthAttachment != nullptr)
        {
            glClearDepth(1.0f);
            mask = mask | GL_DEPTH_BUFFER_BIT;
        }
            
        if (stencilAttachment != 0)
        {
            glClearStencil(0);
            mask = mask | GL_STENCIL_BUFFER_BIT;
        }
        
        glClear(mask);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
}

void FrameBuffer::DeclearBuffer()
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        if (colorAttachmentNum == 0)
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else if (colorAttachmentNum > 1)
        {
            GLuint *attachments = new GLuint[colorAttachmentNum];
            for (GLuint i = 0; i < colorAttachmentNum; ++i)
            {
                attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glDrawBuffers(colorAttachmentNum, attachments);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

bool FrameBuffer::CheckCompleteness()
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ENGIN CORE::ERROR::FRAMEBUFFER: Framebuffer " << &id << "is not complete!" << std::endl;
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    return false;
}

void FrameBuffer::AddTexture2D(TextureBuffer *tex, GLuint attachment)
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        switch (attachment)
        {
        case COLOR_ATTACHMENT:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNum, GL_TEXTURE_2D, tex->id, 0);
            colorAttachments.push_back(tex);
            colorAttachmentNum++;
            break;
        case DEPTH_ATTACHMENT:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);
            depthAttachment = tex;
            break;
        case STENCIL_ATTACHMENT:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);
            stencilAttachment = tex;
            break;
        default:
            break;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::AddTexture3D(TextureBuffer *tex, GLuint attachment)
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        switch (attachment)
        {
        case COLOR_ATTACHMENT:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNum, tex->id, 0);
            colorAttachments.push_back(tex);
            colorAttachmentNum++;
            break;
        case DEPTH_ATTACHMENT:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex->id, 0);
            depthAttachment = tex;
            break;
        case STENCIL_ATTACHMENT:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, tex->id, 0);
            stencilAttachment = tex;
            break;
        default:
            break;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::AddRenderBuffer(TextureBuffer *rbo, GLuint attachment)
{
    if (id != 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        switch (attachment)
        {
        case DEPTH_ATTACHMENT:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo->id);
            depthAttachment = rbo;
            break;
        case STENCIL_ATTACHMENT:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id);
            stencilAttachment = rbo;
            break;
        case DEPTH_STENCIL_ATTACHMENT:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id);
            depthAttachment = rbo;
            stencilAttachment = rbo;
            break;
        default:
            break;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::ReleaseAllColorAttachment()
{
    if (id != 0 && colorAttachmentNum > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        // 解绑所有颜色附件
        for (GLuint i = 0; i < colorAttachmentNum; ++i)
        {
            TextureBuffer *tex = colorAttachments[i];
            GLuint type = tex->type;
            switch (type)
            {
            case GL_TEXTURE_2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
                break;
            case GL_TEXTURE_2D_MULTISAMPLE:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
                break;
            case GL_TEXTURE_CUBE_MAP:
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0);
                break;
            default:
                break;
            }
        }
        colorAttachmentNum = 0;
        colorAttachments.clear();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::ReleaseDepthAttachment()
{
    if (id != 0 && depthAttachment != nullptr)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        GLuint type = depthAttachment->type;
        switch (type)
        {
        case GL_TEXTURE_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
            break;
        case GL_TEXTURE_2D_MULTISAMPLE:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
            break;
        case GL_TEXTURE_CUBE_MAP:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
            break;
        case GL_RENDERBUFFER:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
            break;
        default:
            break;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::ReleaseStencilAttachment()
{
    if (id != 0 && stencilAttachment != nullptr)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        GLuint type = stencilAttachment->type;
        switch (type)
        {
        case GL_TEXTURE_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
            break;
        case GL_TEXTURE_2D_MULTISAMPLE:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
            break;
        case GL_TEXTURE_CUBE_MAP:
            glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
            break;
        case GL_RENDERBUFFER:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
            break;
        default:
            break;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::BlitColorBuffer(GLuint src, GLuint srcAttach, GLuint dst, GLuint dstAttach, GLuint srcWid, GLuint srcHgt, GLuint dstWid, GLuint dstHgt)
{
    if (src != 0 && dst != 0 && src != dst)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + srcAttach);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + dstAttach);
        glBlitFramebuffer(0, 0, srcWid, srcHgt, 0, 0, dstWid, dstHgt, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

void FrameBuffer::BlitDepthBuffer(GLuint src, GLuint dst, GLuint srcWid, GLuint srcHgt, GLuint dstWid, GLuint dstHgt)
{
    if (src != 0 && dst != 0 && src != dst)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);
        glBlitFramebuffer(0, 0, srcWid, srcHgt, 0, 0, dstWid, dstHgt, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
}

#pragma endregion

#pragma region Texture2D

Texture2D::Texture2D()
{
    id = 0;
    width = 1280;
    height = 720;
    type = GL_TEXTURE_2D;
    internalFormat = GL_RGB16F;
    mipmapOn = false;
    dataFormat = GL_RGB;
    border = 0;
    dataType = GL_FLOAT;

    wrapS = GL_CLAMP_TO_EDGE;
    wrapT = GL_CLAMP_TO_EDGE;
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;
    borderColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    std::cout << "ENGIN CORE: Texture2D buffer created ... " << std::endl;
}

void Texture2D::Destroy()
{
    this->~Texture2D();
}

Texture2D::~Texture2D()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
        id = 0;
        std::cout << "ENGIN CORE: Texture2D buffer destroyed ... " << std::endl;
    }
}

void Texture2D::CreateBuffer()
{
    if (id == 0)
    {
        glGenTextures(1, &id);
        glBindTexture(type, id);
        glTexImage2D(type, 0, internalFormat, width, height, border, dataFormat, dataType, NULL);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);
            
        if (wrapS == GL_CLAMP_TO_BORDER || wrapT == GL_CLAMP_TO_BORDER)
        {
            GLfloat borderColor[] = { this->borderColor.x, this->borderColor.y, this->borderColor.z, this->borderColor.w };
            glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glBindTexture(type, 0);
    }
}

void Texture2D::ResetBuffer()
{
    if (id != 0)
    {
        glBindTexture(type, id);
        glTexImage2D(type, 0, internalFormat, width, height, border, dataFormat, dataType, NULL);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);

        if (wrapS == GL_CLAMP_TO_BORDER || wrapT == GL_CLAMP_TO_BORDER)
        {
            GLfloat borderColor[] = { this->borderColor.x, this->borderColor.y, this->borderColor.z, this->borderColor.w };
            glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glBindTexture(type, 0);
    }
    
}


void Texture2D::GenerateMipmap()
{
    if (!mipmapOn)
    {
        glBindTexture(type, id);
        glGenerateMipmap(type);
        glBindTexture(type, 0);

        mipmapOn = true;
    }
}

void Texture2D::ClearBuffer()
{
    if (id != 0)
    {
        glBindTexture(type, id);
        glTexSubImage2D(type, 0, 0, 0, width, height, dataFormat, dataType, NULL);
        glBindTexture(type, 0);
    }
}



#pragma endregion

#pragma region Texture2DMultisample

Texture2DMultisample::Texture2DMultisample()
{
    id = 0;
    width = 1280;
    height = 720;
    type = GL_TEXTURE_2D_MULTISAMPLE;
    internalFormat = GL_RGB16F;
    mipmapOn = false;
    samples = 4;
    fixedSampleLocations = GL_TRUE;

    wrapS = GL_CLAMP_TO_EDGE;
    wrapT = GL_CLAMP_TO_EDGE;
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;

    std::cout << "ENGIN CORE: Texture2DMultisample buffer created ... " << std::endl;
}

void Texture2DMultisample::Destroy()
{
    this->~Texture2DMultisample();
}

Texture2DMultisample::~Texture2DMultisample()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
        id = 0;
        std::cout << "ENGIN CORE: Texture2DMultisample buffer destroyed ... " << std::endl;
    }
}

void Texture2DMultisample::CreateBuffer()
{
    if (id == 0)
    {
        glGenTextures(1, &id);
        glBindTexture(type, id);
        glTexImage2DMultisample(type, samples, internalFormat, width, height, fixedSampleLocations);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);

        glBindTexture(type, 0);
    }
}

void Texture2DMultisample::ResetBuffer()
{
    if (id != 0)
    {
        glBindTexture(type, id);
        glTexImage2DMultisample(type, samples, internalFormat, width, height, fixedSampleLocations);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);

        glBindTexture(type, 0);
    }
    
}

void Texture2DMultisample::GenerateMipmap()
{
    if (!mipmapOn)
    {
        glBindTexture(type, id);
        glGenerateMipmap(type);
        glBindTexture(type, 0);

        mipmapOn = true;
    }
}

void Texture2DMultisample::ClearBuffer()
{
    // To be continued ... 
}

#pragma endregion

#pragma region Texture3D

Texture3D::Texture3D()
{
    id = 0;
    width = 1280;
    height = 720;
    type = GL_TEXTURE_CUBE_MAP;
    internalFormat = GL_RGB16F;
    mipmapOn = false;
    dataFormat = GL_RGB;
    border = 0;
    dataType = GL_FLOAT;

    wrapS = GL_CLAMP_TO_EDGE;
    wrapT = GL_CLAMP_TO_EDGE;
    wrapR = GL_CLAMP_TO_EDGE;
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;
    borderColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    
    std::cout << "Texture3D buffer created ... " << std::endl;
}

Texture3D::~Texture3D()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
        id = 0;
        std::cout << "Texture 3D buffer destroyed ... " << std::endl;
    }
}

void Texture3D::Destroy()
{
    this->~Texture3D();
}

void Texture3D::CreateBuffer()
{
    if (id == 0)
    {
        glGenTextures(1, &id);
        glBindTexture(type, id);
        
        for (GLuint i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, border, dataFormat, dataType, NULL);
        }
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(type, GL_TEXTURE_WRAP_R, wrapR);

        if (wrapS == GL_CLAMP_TO_BORDER || wrapT == GL_CLAMP_TO_BORDER || wrapR == GL_CLAMP_TO_BORDER)
        {
            GLfloat borderColor[] = { this->borderColor.x, this->borderColor.y, this->borderColor.z, this->borderColor.w };
            glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glBindTexture(type, 0);
    }

}

void Texture3D::ResetBuffer()
{
    if (id != 0)
    {
        glBindTexture(type, id);
        for (GLuint i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, border, dataFormat, dataType, NULL);
        }
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(type, GL_TEXTURE_WRAP_R, wrapR);

        if (wrapS == GL_CLAMP_TO_BORDER || wrapT == GL_CLAMP_TO_BORDER || wrapR == GL_CLAMP_TO_BORDER)
        {
            GLfloat borderColor[] = { this->borderColor.x, this->borderColor.y, this->borderColor.z, this->borderColor.w };
            glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glBindTexture(type, 0);
    }
    
}

void Texture3D::GenerateMipmap()
{
    if (!mipmapOn)
    {
        glBindTexture(type, id);
        glGenerateMipmap(type);
        glBindTexture(type, 0);

        mipmapOn = true;
    }
}

void Texture3D::ClearBuffer()
{
    if (id != 0)
    {
        glBindTexture(type, id);
        for (GLuint i = 0; i < 6; ++i)
        {
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, dataFormat, dataType, NULL);
        }
        glBindTexture(type, 0);
    }
    
}

#pragma endregion

#pragma region RenderBuffer

RenderBuffer::RenderBuffer()
{
    id = 0;
    width = 1280;
    height = 720;
    internalFormat = GL_DEPTH24_STENCIL8;
    type = GL_RENDERBUFFER;

    std::cout << "ENGIN CORE: Render buffer created ... " << std::endl;
}

RenderBuffer::~RenderBuffer()
{
    if (id != 0)
    {
        glDeleteRenderbuffers(1, &id);
        id = 0;
        std::cout << "ENGIN CORE: Render buffer destroyed ... " << std::endl;
    }
}

void RenderBuffer::Destroy()
{
    this->~RenderBuffer();
}

void RenderBuffer::CreateBuffer()
{
    if (id == 0)
    {
        glGenRenderbuffers(1, &id);
        glBindRenderbuffer(type, id);
        glRenderbufferStorage(type, internalFormat, width, height);
        glBindRenderbuffer(type, 0);
    }
}

void RenderBuffer::ResetBuffer()
{
    if (id != 0)
    {
        glBindRenderbuffer(type, id);
        glRenderbufferStorage(type, internalFormat, width, height);
        glBindRenderbuffer(type, 0);
    }
    
}

void RenderBuffer::ClearBuffer()
{
    // To be continued ...
}

#pragma endregion

#pragma region RenderBufferMultisample

RenderBufferMultisample::RenderBufferMultisample()
{
    id = 0;
    width = 1280;
    height = 720;
    internalFormat = GL_DEPTH24_STENCIL8;
    type = GL_RENDERBUFFER;
    samples = 4;
    std::cout << "ENGIN CORE: Multisample render buffer created ... " << std::endl;
}

RenderBufferMultisample::~RenderBufferMultisample()
{
    if (id != 0)
    {
        glDeleteRenderbuffers(1, &id);
        id = 0;
        std::cout << "ENGIN CORE: Multisample render buffer destroyed ... " << std::endl;
    }
}

void RenderBufferMultisample::Destroy()
{
    this->~RenderBufferMultisample();
}

void RenderBufferMultisample::CreateBuffer()
{
    if (id == 0)
    {
        glGenRenderbuffers(1, &id);
        glBindRenderbuffer(type, id);
        glRenderbufferStorageMultisample(type, samples, internalFormat, width, height);
        glBindRenderbuffer(type, 0);
    }
}

void RenderBufferMultisample::ResetBuffer()
{
    if (id != 0)
    {
        glBindRenderbuffer(type, id);
        glRenderbufferStorageMultisample(type, samples, internalFormat, width, height);
        glBindRenderbuffer(type, 0);
    }
}

void RenderBufferMultisample::ClearBuffer()
{
    // To be continued ...
}

#pragma endregion

#pragma region UniformBuffer

UniformBuffer::UniformBuffer()
{
    id = 0;
    size = 0;
    base = 0;
    std::cout << "ENGIN CORE: UniformBuffer created ... " << std::endl;
}

UniformBuffer::~UniformBuffer()
{
    if (id != 0)
    {
        glDeleteBuffers(1, &id);
        id = 0;
        std::cout << "ENGIN CORE: UniformBuffer destroyed ... " << std::endl;
    }
}

void UniformBuffer::Destroy()
{
    this->~UniformBuffer();
}

void UniformBuffer::CreateBuffer()
{
    if (id == 0)
    {
        glGenBuffers(1, &id);
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, this->size, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void UniformBuffer::ResetBuffer()
{
    if (id != 0)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, this->size, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void UniformBuffer::ClearBuffer()
{
    if (id != 0)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, this->size, NULL);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void UniformBuffer::BindBase()
{
    if (id != 0)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, base, id);
    }
}

#pragma endregion

#pragma region HdrIBLTextures
Matrix4x4 HdrIBLTextures::renderCubeMapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
Matrix4x4 *HdrIBLTextures::renderCubeMapViews = new Matrix4x4[6]
{
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

HdrIBLTextures::HdrIBLTextures()
{
    this->cube_map_width = 1024;
    this->irradiance_map_width = 128;
    this->prefilter_map_width = 128;
    this->brdf_lut_map_width = 512;
    this->texturesInitiated = false;
    this->textureBeenFilled = false;
    for (GLuint i = 0; i < IBL_MAP_NUM; ++i)
    {
        IBLTextures[i] == nullptr;
    }
}

HdrIBLTextures::~HdrIBLTextures()
{
    if (texturesInitiated)
    {
        for (GLuint i = 0; i < IBL_MAP_NUM; ++i)
        {
            IBLTextures[i]->Destroy();
        }

        glDeleteTextures(1, &hdrTexture.id);

        std::cout << "HdrIBLTextures destroyed ... " << std::endl;
    }
    
}

void HdrIBLTextures::Destroy()
{
    this->~HdrIBLTextures();
}

void HdrIBLTextures::CreateTextures()
{
    if (!texturesInitiated)
    {
        Texture3D *ibl_cube_map = new Texture3D();
        ibl_cube_map->internalFormat = GL_RGB16F;
        ibl_cube_map->width = cube_map_width;
        ibl_cube_map->height = ibl_cube_map->width;
        ibl_cube_map->dataFormat = GL_RGB;
        ibl_cube_map->minFilter = GL_LINEAR_MIPMAP_LINEAR;
        ibl_cube_map->magFilter = GL_LINEAR;
        ibl_cube_map->CreateBuffer();
        IBLTextures[IBL_CUBE_MAP] = ibl_cube_map;

        Texture3D *ibl_irradiance_map = new Texture3D();
        ibl_irradiance_map->internalFormat = GL_RGB16F;
        ibl_irradiance_map->width = irradiance_map_width;
        ibl_irradiance_map->height = ibl_irradiance_map->width;
        ibl_irradiance_map->minFilter = GL_LINEAR;
        ibl_irradiance_map->magFilter = GL_LINEAR;
        ibl_irradiance_map->CreateBuffer();
        IBLTextures[IBL_IRRADIANCE_MAP] = ibl_irradiance_map;

        Texture3D *ibl_prefilter_map = new Texture3D();
        ibl_prefilter_map->internalFormat = GL_RGB16F;
        ibl_prefilter_map->width = prefilter_map_width;
        ibl_prefilter_map->height = ibl_prefilter_map->width;
        ibl_prefilter_map->minFilter = GL_LINEAR_MIPMAP_LINEAR;
        ibl_prefilter_map->magFilter = GL_LINEAR;
        ibl_prefilter_map->CreateBuffer();
        // 如果使用了 GL_LINEAR_MIPMAP_LINEAR 等应该生成mipmap
        ibl_prefilter_map->GenerateMipmap();
        IBLTextures[IBL_PREFILTER_MAP] = ibl_prefilter_map;

        Texture2D *ibl_brdf_lut_map = new Texture2D();
        ibl_brdf_lut_map->internalFormat = GL_RG16F;
        ibl_brdf_lut_map->width = brdf_lut_map_width;
        ibl_brdf_lut_map->height = ibl_brdf_lut_map->width;
        ibl_brdf_lut_map->dataFormat = GL_RG;
        ibl_brdf_lut_map->minFilter = GL_LINEAR;
        ibl_brdf_lut_map->magFilter = GL_LINEAR;
        ibl_brdf_lut_map->CreateBuffer();
        IBLTextures[IBL_BRDF_LUT_MAP] = ibl_brdf_lut_map;

        texturesInitiated = true;
    }
}

void HdrIBLTextures::DrawTextures(const FrameBuffer *framebuffer, const RenderBuffer *rbo)
{
    if (hdrTexture.id != 0 && texturesInitiated)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo->id);
        
        // 非常重要的一行，不关闭面剔除，立方体贴图无法正常绘制
        // 只需一次
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        //glDepthFunc(GL_LEQUAL);

        // 绘制立方体贴图
        glRenderbufferStorage(GL_RENDERBUFFER, rbo->internalFormat, cube_map_width, cube_map_width);
        glViewport(0, 0, cube_map_width, cube_map_width);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture.id);

        HdrBoxShader *hdrBoxShader = HdrBoxShader::GetInstance();
        hdrBoxShader->Bind();
        hdrBoxShader->SetMat4("projection", HdrIBLTextures::renderCubeMapProjection);

        CubeMesh *cubeMesh = CubeMesh::GetInstance();
        // 绑定立方体网格，只需一次
        glBindVertexArray(cubeMesh->VAO);

        for (GLuint i = 0; i < 6; ++i)
        {
            hdrBoxShader->SetMat4("view", HdrIBLTextures::renderCubeMapViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBLTextures[IBL_CUBE_MAP]->id, NULL);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if (cubeMesh->indices.size() > 0)
                glDrawElements(GL_TRIANGLES, cubeMesh->indices.size(), GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, cubeMesh->vertices.size());
        }

        // 立方体贴图生成多级渐远纹理
        glBindTexture(GL_TEXTURE_CUBE_MAP, IBLTextures[IBL_CUBE_MAP]->id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


        glBindTexture(GL_TEXTURE_2D, 0);
        hdrBoxShader->Unbind();

        
        // 绘制立方体辐照度贴图
        glRenderbufferStorage(GL_RENDERBUFFER, rbo->internalFormat, irradiance_map_width, irradiance_map_width);
        glViewport(0, 0, irradiance_map_width, irradiance_map_width);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->IBLTextures[IBL_CUBE_MAP]->id);

        IrradianceBoxShader *irradianceBoxShader = IrradianceBoxShader::GetInstance();
        irradianceBoxShader->Bind();
        irradianceBoxShader->SetMat4("projection", HdrIBLTextures::renderCubeMapProjection);

        for (GLuint i = 0; i < 6; ++i)
        {
            irradianceBoxShader->SetMat4("view", HdrIBLTextures::renderCubeMapViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBLTextures[IBL_IRRADIANCE_MAP]->id, NULL);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if (cubeMesh->indices.size() > 0)
                glDrawElements(GL_TRIANGLES, cubeMesh->indices.size(), GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, cubeMesh->vertices.size());
        }

        irradianceBoxShader->Unbind();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // 绘制预滤波镜面立方贴图
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->IBLTextures[IBL_CUBE_MAP]->id);

        PrefilterBoxShader *prefilterBoxShader = PrefilterBoxShader::GetInstance();
        prefilterBoxShader->Bind();
        prefilterBoxShader->SetMat4("projection", HdrIBLTextures::renderCubeMapProjection);

        GLuint maxMipLevels = 5;
        for (GLuint mip = 0; mip < maxMipLevels; ++mip)
        {
            GLuint mipWidth = prefilter_map_width * std::pow(0.5f, mip);
            GLuint mipHeight = prefilter_map_width * std::pow(0.5f, mip);

            glRenderbufferStorage(GL_RENDERBUFFER, rbo->internalFormat, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            GLfloat roughness = (GLfloat)mip / (GLfloat)(maxMipLevels - 1);
            prefilterBoxShader->SetFloat("roughness", roughness);
            for (GLuint i = 0; i < 6; ++i)
            {
                prefilterBoxShader->SetMat4("view", HdrIBLTextures::renderCubeMapViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBLTextures[IBL_PREFILTER_MAP]->id, mip);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                if (cubeMesh->indices.size() > 0)
                    glDrawElements(GL_TRIANGLES, cubeMesh->indices.size(), GL_UNSIGNED_INT, 0);
                else
                    glDrawArrays(GL_TRIANGLES, 0, cubeMesh->vertices.size());
            }
        }

        prefilterBoxShader->Unbind();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // 绘制BRDF贴图
        glRenderbufferStorage(GL_RENDERBUFFER, rbo->internalFormat, brdf_lut_map_width, brdf_lut_map_width);
        glViewport(0, 0, brdf_lut_map_width, brdf_lut_map_width);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, IBLTextures[IBL_BRDF_LUT_MAP]->id, NULL);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        BRDFShader *brdfShader = BRDFShader::GetInstance();
        brdfShader->Bind();

        RectangleMesh *rectangleMesh = RectangleMesh::GetInstance();
        glBindVertexArray(rectangleMesh->VAO);
        if (rectangleMesh->indices.size() > 0)
            glDrawElements(GL_TRIANGLES, rectangleMesh->indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, rectangleMesh->vertices.size());

        glBindVertexArray(0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        brdfShader->Unbind();

        // 恢复面剔除
        glEnable(GL_CULL_FACE);
        //glDepthFunc(GL_LESS);

        textureBeenFilled = true;
    }
}

#pragma endregion



