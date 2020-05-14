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

    void InitGLFW(GLuint majorVersion, GLuint minorVersion);

    class fxWindow {
    public:
        GLuint width;
        GLuint height;
        string title;

        fxWindow(GLuint width = 1280, GLuint height = 720, const GLchar *title = "FantasyX")
        {
            window = nullptr;
            this->width = width;
            this->height = height;
            this->title = title;
            std::cout << "ENGIN CORE: A window was created ... " << std::endl;
        }
        void Init(GLuint contextMajorVersion = 3, GLuint contextMinorVersion = 3)
        {

            if (window == nullptr)
            {
                // 只需要初始化一次
                if (!glfwInitiated)
                {
                    glfwInit();
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
                    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
                    glfwInitiated = true;
                }

                this->window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

                if (window == nullptr)
                {
                    std::cout << "ENGIN CORE: Failed to created a glfw window ... " << std::endl;
                    glfwTerminate();
                    system("pause");
                    exit(0);
                }

                glfwMakeContextCurrent(window);

                // 只需要初始化一次
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
        void Destroy();
        bool ShouldClose();
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
        GLFWwindow *window;
        ~fxWindow();
    };
}

#endif // !CONTEXT_H

