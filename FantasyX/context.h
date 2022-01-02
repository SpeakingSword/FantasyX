#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include "gameobject.h"
#include "component.h"
#include "clocker.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>

using namespace std;

namespace fx {

    class fxWindow {
    public:
        GLuint width;
        GLuint height;
        string title;

        fxWindow(GLuint width = 1280, GLuint height = 720, const GLchar *title = "FantasyX");
        void Init(bool fullScreen, GLuint contextMajorVersion = 3, GLuint contextMinorVersion = 3);
        void Destroy();
        bool ShouldClose();
        void Clear(Vector3 color, bool depth = false, bool stencil = false);
        void Update();
        void SwapBuffers();
        void PollEvent();
        void Terminate();
        void Close();
        void Resize(GLuint width, GLuint height);
        void ProcessInput(GameObject *sceneRoot, GameObject *camera);
        GLFWwindow *GetGLFWwindow();

    private:
        static bool gladLoaded;
        static bool glfwInitiated;
        GLfloat lastX;
        GLfloat lastY;
        bool firstMove;
        GLFWwindow *window;
        ~fxWindow();
        void MouseMove(GLdouble xpos, GLdouble ypos, Camera *camera);
    };
}

#endif // !CONTEXT_H

