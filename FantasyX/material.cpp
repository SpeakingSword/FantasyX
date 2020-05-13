#include "material.h"
#include "shader.h"

using namespace fx;

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
    // ��ͼ����ͨ��Ĭ��ͼƬ��ʼ��
    // ����ȫ��ɫ��������ͼ��ȫ��ɫ������ͼ��ȫ��ɫ�߹���ͼ
    name = "LegacyStandardMaterial";
    // ��ʼ����ͼ ...
    ResourceManager *res = ResourceManager::GetInstance();
    Texture diffuseMap = res->LoadTexture2D((string(res->GetAppDir()) + string(res->GetImageDir()) + "default_diff.png").c_str(), "_DiffuseMap", true);
    Texture normalMap = res->LoadTexture2D((string(res->GetAppDir()) + string(res->GetImageDir()) + "default_normal.png").c_str(), "_NormalMap", false);
    Texture specularMap = res->LoadTexture2D((string(res->GetAppDir()) + string(res->GetImageDir()) + "default_spec.png").c_str(), "_SpecularMap", false);
    textures2D.push_back(diffuseMap);
    textures2D.push_back(normalMap);
    textures2D.push_back(specularMap);
    specularStrength = 32;
    shader = LegacyStandardShader::GetInstance();
}

void LegacyStandardMaterial::MappingProperty()
{
    shader->Bind();
    shader->SetInt("material._SpecularStrength", specularStrength);
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
    std::cout << "SpecularStrength: " << this->specularStrength << std::endl;
    std::cout << "Shader ID: " << this->shader->ID << std::endl;
    std::cout << "Shader Name: " << this->shader->name.c_str() << std::endl;
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
    metallic = 0.5f;
    roughness = 0.5f;
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

#pragma endregion
