#include "context.h"

using namespace fx;

bool fxWindow::gladLoaded = false;
bool fxWindow::glfwInitiated = false;

fxWindow::~fxWindow()
{
    std::cout << "ENGIN CORE::A window was closed ... " << std::endl;
}

fx::fxWindow::fxWindow(GLuint width, GLuint height, const GLchar *title)
{
    window = nullptr;
    this->width = width;
    this->height = height;
    this->title = title;
    std::cout << "ENGIN CORE::A window was created ... " << std::endl;
}

void fx::fxWindow::Init(bool fullScreen, GLuint contextMajorVersion, GLuint contextMinorVersion)
{
    if (window == nullptr)
    {
        // ֻ��Ҫ��ʼ��һ��
        if (!glfwInitiated)
        {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
            glfwWindowHint(GLFW_DECORATED, !fullScreen);
            glfwInitiated = true;
        }

        this->window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        
        GLfloat offsetX = (1920 - width) / 2;
        GLfloat offsetY = (1080 - height) / 2;
        // 30px �Ǳ������ĸ߶�
        if (!fullScreen)
            offsetY += 30;

        glfwSetWindowPos(window, offsetX, offsetY);

        if (window == nullptr)
        {
            std::cout << "ENGIN CORE: Failed to created a glfw window ... " << std::endl;
            glfwTerminate();
            system("pause");
            exit(0);
        }

        glfwMakeContextCurrent(window);

        // ֻ��Ҫ��ʼ��һ��
        if (!gladLoaded)
        {
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cout << "ENGIN CORE: Failed to initialize GLAD" << std::endl;
                system("pause");
                exit(0);
            }

            gladLoaded = true;
        }
    }
}

void fxWindow::Destroy()
{
    this->~fxWindow();
}

bool fxWindow::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void fxWindow::Close()
{
    glfwSetWindowShouldClose(window, true);
}

void fxWindow::Resize(GLuint width, GLuint height)
{
    
}

void fxWindow::Clear(Vector3 color, bool depth, bool stencil)
{
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClearColor(color.x, color.y, color.y, 1.0f);
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (depth)
    {
        glEnable(GL_DEPTH_TEST);
        mask |= GL_DEPTH_BUFFER_BIT;
    }
        
    if (stencil)
    {
        glEnable(GL_STENCIL_TEST);
        mask |= GL_STENCIL_BUFFER_BIT;
    }    

    glClear(mask);
}

void fxWindow::SwapBuffers()
{
    glfwSwapBuffers(window);
}

void fxWindow::PollEvent()
{
    glfwPollEvents();
}

void fxWindow::Terminate()
{
    glfwTerminate();
}

GLFWwindow *fxWindow::GetGLFWwindow()
{
    return window;
}

void fxWindow::MouseMove(GLdouble xpos, GLdouble ypos, Camera *camera)
{
    if (firstMove)
    {
        lastX = xpos;
        lastY = ypos;
        firstMove = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void fxWindow::ProcessInput(GameObject *sceneRoot, GameObject *camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    GLdouble xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    MouseMove(xpos, ypos, (Camera *)camera->GetComponent("Camera"));

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera->transform->position += WORLD_UP * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera->transform->position -= WORLD_UP * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera->transform->position += camera->transform->front * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera->transform->position -= camera->transform->front * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera->transform->position -= camera->transform->right * Time::deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera->transform->position += camera->transform->right * Time::deltaTime;
    }
}

void fxWindow::Update()
{
    
}