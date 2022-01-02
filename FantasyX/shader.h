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
        void SetVec4(const GLchar *name, const Vector4 &value) const;
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

#pragma region PBRStandardShader

    class PBRStandardShader : public Shader {
    public:
        static PBRStandardShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new PBRStandardShader();
            }

            return _instance;
        }

    private:
        static PBRStandardShader *_instance;

        PBRStandardShader()
        {
            InitShader();
            std::cout << "PBRStandardShader created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "PBRStandardShader GC created ... " << endl; }
            ~GC()
            {
                cout << "PBRStandardShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "PBRStandardShader destroyed ... " << endl;
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

#pragma region DirLightShadowShader

    class DirLightShadowShader : public Shader {
    public:
        static DirLightShadowShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new DirLightShadowShader();
            }

            return _instance;
        }

    private:
        static DirLightShadowShader *_instance;

        DirLightShadowShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "DirLightShadowShader GC created ... " << endl; }
            ~GC()
            {
                cout << "DirLightShadowShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "DirLightShadowShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region PBRLightingShader

    class PBRLightingShader : public Shader {
    public:
        static PBRLightingShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new PBRLightingShader();
            }

            return _instance;
        }

    private:
        static PBRLightingShader *_instance;

        PBRLightingShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "PBRLightingShader GC created ... " << endl; }
            ~GC()
            {
                cout << "PBRLightingShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "PBRLightingShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region BrightCatchShader

    class BrightCatchShader : public Shader {
    public:
        static BrightCatchShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new BrightCatchShader();
            }

            return _instance;
        }

    private:
        static BrightCatchShader *_instance;

        BrightCatchShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "BrightCatchShader GC created ... " << endl; }
            ~GC()
            {
                cout << "BrightCatchShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "BrightCatchShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region GaussianBlurShader

    class GaussianBlurShader : public Shader {
    public:
        static GaussianBlurShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new GaussianBlurShader();
            }

            return _instance;
        }

    private:
        static GaussianBlurShader *_instance;

        GaussianBlurShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "GaussianBlurShader GC created ... " << endl; }
            ~GC()
            {
                cout << "GaussianBlurShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "GaussianBlurShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region BloomShader

    class BloomShader : public Shader {
    public:
        static BloomShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new BloomShader();
            }

            return _instance;
        }

    private:
        static BloomShader *_instance;

        BloomShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "BloomShader GC created ... " << endl; }
            ~GC()
            {
                cout << "BloomShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "BloomShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region SkyBoxShader

    class SkyBoxShader : public Shader {
    public:
        static SkyBoxShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new SkyBoxShader();
            }

            return _instance;
        }

    private:
        static SkyBoxShader *_instance;

        SkyBoxShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "SkyBoxShader GC created ... " << endl; }
            ~GC()
            {
                cout << "SkyBoxShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "SkyBoxShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region HdrBoxShader

    class HdrBoxShader : public Shader {
    public:
        static HdrBoxShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new HdrBoxShader();
            }

            return _instance;
        }

    private:
        static HdrBoxShader *_instance;

        HdrBoxShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "HdrBoxShader GC created ... " << endl; }
            ~GC()
            {
                cout << "HdrBoxShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "HdrBoxShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region IrradianceBoxShader

    class IrradianceBoxShader : public Shader {
    public:
        static IrradianceBoxShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new IrradianceBoxShader();
            }

            return _instance;
        }

    private:
        static IrradianceBoxShader *_instance;

        IrradianceBoxShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "IrradianceBoxShader GC created ... " << endl; }
            ~GC()
            {
                cout << "IrradianceBoxShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "IrradianceBoxShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region PrefilterBoxShader

    class PrefilterBoxShader : public Shader {
    public:
        static PrefilterBoxShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new PrefilterBoxShader();
            }

            return _instance;
        }

    private:
        static PrefilterBoxShader *_instance;

        PrefilterBoxShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "PrefilterBoxShader GC created ... " << endl; }
            ~GC()
            {
                cout << "PrefilterBoxShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "PrefilterBoxShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion

#pragma region BRDFShader

    class BRDFShader : public Shader {
    public:
        static BRDFShader *GetInstance()
        {
            if (_instance == nullptr)
            {
                _instance = new BRDFShader();
            }

            return _instance;
        }

    private:
        static BRDFShader *_instance;

        BRDFShader()
        {
            InitShader();
            std::cout << name.c_str() << " created ... " << std::endl;
        }

        class GC {
        public:
            GC() { cout << "BRDFShader GC created ... " << endl; }
            ~GC()
            {
                cout << "BRDFShader GC destroyed ..." << endl;
                if (_instance != nullptr)
                {
                    delete _instance;
                    _instance = nullptr;
                    cout << "BRDFShader destroyed ... " << endl;
                }
            }
        };

        static GC gc;

        void InitShader() override;
    };

#pragma endregion


    // 更多 Shader ...
}

#endif // !SHADER_H