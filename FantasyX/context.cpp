#include "context.h"

using namespace fx;

bool fxWindow::gladLoaded = false;
bool fxWindow::glfwInitiated = false;

fxWindow::~fxWindow()
{
    std::cout << "ENGIN CORE: A window was closed ... " << std::endl;
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