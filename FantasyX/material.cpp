#include "material.h"
#include "shader.h"

namespace fx {

#pragma region LegacySimpleMaterial
    LegacySimpleMaterial::LegacySimpleMaterial()
    {
        InitMaterial();
        cout << "ENGIN CORE: " << name.c_str() << " created ... " << endl;
    }

    LegacySimpleMaterial::~LegacySimpleMaterial()
    {
        cout << "ENGIN CORE: " << name.c_str() << " destroyed ... " << endl;
    }

    void LegacySimpleMaterial::InitMaterial()
    {
        name = "LegacySimpleMaterial";
        specularStrength = 32;
        baseColor = Vector3(0.7f);
        shader = LegacySimpleShader::GetInstance();
    }

    void LegacySimpleMaterial::MappingProperty()
    {
        shader->Bind();
        shader->SetVec3("material._BaseColor", baseColor);
        shader->SetInt("material._SpecularStrength", specularStrength);
        shader->Unbind();
    }

    void LegacySimpleMaterial::Destroy()
    {
        this->~LegacySimpleMaterial();
    }

    void LegacySimpleMaterial::Print()
    {
        std::cout << "Material Name: " << this->name.c_str() << std::endl;
        std::cout << "BaseColor: " << baseColor.x << " " << baseColor.y << " " << baseColor.z << std::endl;
        std::cout << "SpecularStrength: " << this->specularStrength << std::endl;
        std::cout << "Shader ID: " << this->shader->ID << std::endl;
        std::cout << "Shader Name: " << this->shader->name.c_str() << std::endl;
    }

    void LegacySimpleMaterial::DrawUIElements()
    {
        ImGui::Text("Material: %s", name.c_str());
        ImGui::ColorEdit3("BaseColor", (GLfloat *)&baseColor);
        ImGui::SliderFloat("SpecStrength", &specularStrength, 0.0f, 1.0f);
        ImGui::Text("Shader: %s", shader->name.c_str());
    }

#pragma endregion

#pragma region LegacyStandardMaterial
    LegacyStandardMaterial::LegacyStandardMaterial()
    {
        InitMaterial();
        std::cout << "ENGIN CORE: " << name.c_str() << " created ... " << std::endl;
    }

    LegacyStandardMaterial::~LegacyStandardMaterial()
    {
        std::cout << "ENGIN CORE: " << name.c_str() << " destroyed ... " << std::endl;
    }

    void LegacyStandardMaterial::Destroy()
    {
        this->~LegacyStandardMaterial();
    }

    void LegacyStandardMaterial::InitMaterial()
    {
        name = "LegacyStandardMaterial";
        shader = LegacyStandardShader::GetInstance();
    }

    void LegacyStandardMaterial::MappingProperty()
    {
        shader->Bind();
        for (int i = 0; i < textures2D.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            shader->SetInt(("material." + textures2D[i].type).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures2D[i].id);
        }
        shader->Unbind();
    }

    void LegacyStandardMaterial::Print()
    {
        std::cout << "Material Name: " << this->name.c_str() << std::endl;
        for (int i = 0; i < this->textures2D.size(); i++)
        {
            std::cout << "Texture" << i << " Type: " << textures2D[i].type.c_str() << std::endl;
        }
        std::cout << "Shader ID: " << this->shader->ID << std::endl;
        std::cout << "Shader Name: " << this->shader->name.c_str() << std::endl;
    }

    void LegacyStandardMaterial::DrawUIElements()
    {
        ImGui::Text("Material: %s", name.c_str());
        for (auto iter = textures2D.cbegin(); iter != textures2D.cend(); iter++)
        {
            ImGui::Separator();
            ImGui::Text("%s", (*iter).type.c_str());
            ImGui::SameLine();
            ImGui::Text("%s", (*iter).path.c_str());
        }
        ImGui::Text("Shader: %s", shader->name.c_str());
    }
#pragma endregion

#pragma region PBRSimpleMaterial

    PBRSimpleMaterial::PBRSimpleMaterial()
    {
        InitMaterial();
        cout << "ENGIN CORE: " << name.c_str() << " created ... " << endl;
    }

    PBRSimpleMaterial::~PBRSimpleMaterial()
    {
        cout << "ENGIN CORE: " << name.c_str() << " destroyed ... " << endl;
    }

    void PBRSimpleMaterial::Destroy()
    {
        this->~PBRSimpleMaterial();
    }

    void PBRSimpleMaterial::InitMaterial()
    {
        name = "PBRSimpleMaterial";
        baseColor = Vector3(0.7f);
        metallic = 1.0f;
        roughness = 0.2f;
        ao = 1.0f;
        shader = PBRSimpleShader::GetInstance();
    }

    void PBRSimpleMaterial::MappingProperty()
    {
        shader->Bind();
        shader->SetVec3("material._BaseColor", baseColor);
        shader->SetFloat("material._Metallic", metallic);
        shader->SetFloat("material._Roughness", roughness);
        shader->SetFloat("material._Ao", ao);
        shader->Unbind();
    }

    void PBRSimpleMaterial::Print()
    {
        std::cout << "Material Name: " << this->name.c_str() << std::endl;
        std::cout << "BaseColor: " << baseColor.x << " " << baseColor.y << " " << baseColor.z << std::endl;
        std::cout << "Metallic: " << this->metallic << std::endl;
        std::cout << "Roughness: " << this->roughness << std::endl;
        std::cout << "Ao: " << this->ao << std::endl;
        std::cout << "Shader ID: " << this->shader->ID << std::endl;
        std::cout << "Shader Name: " << this->shader->name.c_str() << std::endl;
    }

    void PBRSimpleMaterial::DrawUIElements()
    {
        ImGui::Text("Material: %s", name.c_str());
        ImGui::ColorEdit3("BaseColor", (GLfloat *)&baseColor);
        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Ao", &ao, 0.0f, 1.0f);
        ImGui::Text("Shader: %s", shader->name.c_str());
    }

#pragma endregion

#pragma region PBRStandradMaterial

    PBRStandardMaterial::PBRStandardMaterial()
    {
        InitMaterial();
        std::cout << "ENGIN CORE:: " << name.c_str() << " created ... " << std::endl;
    }

    PBRStandardMaterial::~PBRStandardMaterial()
    {
        std::cout << "ENGIN CORE:: " << name.c_str() << " destoyed ... " << std::endl;
    }

    void PBRStandardMaterial::Destroy()
    {
        this->~PBRStandardMaterial();
    }

    void PBRStandardMaterial::InitMaterial()
    {
        name = "PBRStandardMaterial";
        shader = PBRStandardShader::GetInstance();
    }

    void PBRStandardMaterial::MappingProperty()
    {
        shader->Bind();
        for (GLuint i = 0; i < textures2D.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            shader->SetInt(("material." + textures2D[i].type).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures2D[i].id);
        }
        shader->Unbind();
    }

    void PBRStandardMaterial::Print()
    {
        // wait ...
    }

    void PBRStandardMaterial::DrawUIElements()
    {
        ImGui::Text("Material: %s", name.c_str());
        for (auto iter = textures2D.cbegin(); iter != textures2D.cend(); iter++)
        {
            ImGui::Separator();
            ImGui::Text("%s", (*iter).type.c_str());
            ImGui::SameLine();
            ImGui::Text("%s", (*iter).path.c_str());
        }
        ImGui::Text("Shader: %s", shader->name.c_str());
    }
#pragma endregion

}