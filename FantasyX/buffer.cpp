#include "buffer.h"

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

/*
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
*/

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


