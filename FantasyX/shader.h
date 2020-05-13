#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "resource_manager.h"
#include "types.h"

using namespace std;

namespace fx {
    
    class Shader {
    public:
        string name;
        GLuint ID;
        void Bind();
        void Unbind();
        void SetBool(const GLchar *name, GLboolean value) const;
        void SetInt(const GLchar *name, GLint value) const;
        void SetFloat(const GLchar *name, GLfloat value) const;
        void SetMat4(const GLchar *name, const Matrix4x4 &value) const;
        void SetVec3(const GLchar *name, const Vector3 &value) const;
        void SetVec3(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) const;

        virtual void InitShader() = 0;
        GLuint CreateShaderProgram(const GLchar *, const GLchar *);
        GLuint CreateShaderProgram(const GLchar *, const GLchar *, const GLchar *);
    };
    
#pragma region LegacySimpleShader
    class LegacySimpleShader : public Shader {
    public:
        static LegacySimpleShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new LegacySimpleShader();
            }

            return _instance;
        }

    private:
        static LegacySimpleShader *_instance;

        LegacySimpleShader()
        {
            InitShader();
            cout << "LegacySimpleShader created ..." << endl;
        };

        class GC {
        public:
            GC() { cout << "LegacySimpleShader GC created ... " << endl; }
            ~GC()
            {
                cout << "LegacySimpleShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "LegacySimpleShader destroyed ... " << endl;
                }
            }
        };
        static GC gc;

        void InitShader() override;
    };
#pragma endregion

#pragma region LegacyStandardShader
    class LegacyStandardShader : public Shader {
    public:
        static LegacyStandardShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new LegacyStandardShader();
            }

            return _instance;
        }

    private:
        static LegacyStandardShader *_instance;

        LegacyStandardShader()
        {
            InitShader();
            std::cout << "LegacyStandardShader created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "LegacyStandardShader GC created ... " << endl; }
            ~GC()
            {
                cout << "LegacyStandardShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "LegacyStandardShader destroyed ... " << endl;
                }
            }
        };
        static GC gc;

        void InitShader() override;
    };
#pragma endregion

#pragma region PBRSimpleShader

    class PBRSimpleShader : public Shader {
    public:
        static PBRSimpleShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new PBRSimpleShader();
            }

            return _instance;
        }

    private:
        static PBRSimpleShader *_instance;

        PBRSimpleShader()
        {
            InitShader();
            std::cout << "PBRSimpleShader created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "PBRSimpleShader GC created ... " << endl; }
            ~GC()
            {
                cout << "PBRSimpleShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "PBRSimpleShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region DisplayShader

    class DisplayShader : public Shader {
    public:
        static DisplayShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new DisplayShader();
            }

            return _instance;
        }

    private:
        static DisplayShader *_instance;

        DisplayShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "DisplayShader GC created ... " << endl; }
            ~GC()
            {
                cout << "DisplayShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "DisplayShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

    // 添加更多的 Shader ...
}

#endif // !SHADER_H