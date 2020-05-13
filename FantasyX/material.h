#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include <glad\glad.h>

#include "types.h"
#include "texture.h"
#include "resource_manager.h"

using namespace std;

namespace fx {
    class Shader;
    
    class Material {
    public:
        string name;
        Shader *shader;

        virtual void InitMaterial() = 0;
        virtual void MappingProperty() = 0;
        virtual void Print() = 0;
        virtual void Destroy() = 0;
    };
    
    class LegacySimpleMaterial : public Material {
    public:
        Vector3 baseColor;
        GLuint specularStrength;

        LegacySimpleMaterial();
        void Destroy() override;
        void MappingProperty() override;
        void Print() override;

    private:
        ~LegacySimpleMaterial();
        void InitMaterial() override;
    };

    class LegacyStandardMaterial : public Material {
    public:
        GLuint specularStrength;
        vector<Texture> textures2D;

        LegacyStandardMaterial();
        void Destroy() override;
        void MappingProperty() override;
        void Print() override;

    private:
        ~LegacyStandardMaterial();
        void InitMaterial() override;
    };

    class PBRSimpleMaterial : public Material {
    public:
        Vector3 baseColor;
        GLfloat metallic;
        GLfloat roughness;
        GLfloat ao;

        PBRSimpleMaterial();
        void Destroy() override;
        void MappingProperty() override;
        void Print() override;

    private:
        ~PBRSimpleMaterial();
        void InitMaterial() override;
    };

    // Ìí¼Ó¸ü¶à Material ... 
    
}

#endif // !MATERIAL_H