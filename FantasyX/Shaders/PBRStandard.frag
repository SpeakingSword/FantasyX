#version 330 core
#define MAX_POINT_LIGHT 8
#define MAX_SPOT_LIGHT 8
out vec4 FragColor;

in VS_OUT {
	vec2 texCoord;
	vec3 fragPos;
	vec3 normal; 
	mat3 TBN;
} fs_in;

// ÿ����ɫ���Ĳ���ͨ����һ��
struct Material {
    sampler2D _AlbedoMap;
    sampler2D _MetallicMap;
    sampler2D _NormalMap;
    sampler2D _RoughnessMap;
    sampler2D _AoMap;
};

// ÿ����ɫ������һ��ƹ�����
struct DirLight {
    vec3 color;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
    float strength;
};

struct PointLight {
    vec3 color;
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
    float strength;
    float radius;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 color;
    vec3 position;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
    float strength;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

// Shader �����
layout (std140) uniform SharedDirLightt {
    DirLight dirLight;
};

layout (std140) uniform SharedPointLights {
    PointLight pointLights[MAX_POINT_LIGHT];
};

layout (std140) uniform SharedSpotLights {
    SpotLight spotLights[MAX_SPOT_LIGHT];
};

layout (std140) uniform SharedVariable {
    int lightNum;
    vec3 viewPos;
};

// ��ɫ�����е������ⲿ������ͨ��
uniform Material material;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// ���������Ҫ�Ĺ����㷨 ... 





void main()
{
    // ִ�й��ռ��㲢��ɫ
}

