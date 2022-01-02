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

// 每个着色器的材质通道不一样
struct Material {
    vec3 _BaseColor;
    int _SpecularStrength;
};

// 每个着色器共享一组灯光数据
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

// Shader 共享池
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
    int pointLightNum;
    int spotLightNum;
    vec3 viewPos;
};

// 着色器自有的连接外部的数据通道
uniform Material material;

// 定义必须的光照算法 ... 
vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);

    vec3 result = CalculateDirLight(dirLight, norm, viewDir);
    for (int i = 0; i < pointLightNum; i++)
    {
        result += CalculatePointLight(pointLights[i], norm, fs_in.fragPos, viewDir);
    }

    for (int i = 0; i < spotLightNum; i++)
    {
        result += CalculateSpotLight(spotLights[i], norm, fs_in.fragPos, viewDir);
    }

    FragColor = vec4(result, 1.0f);
}

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0f);

    // 布林-冯高光
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), material._SpecularStrength);


    vec3 ambient = light.color * light.ambient * material._BaseColor;
    vec3 diffuse = light.color * light.diffuse * diff * material._BaseColor;
    vec3 specular = light.color * light.specular * spec; // 问题：是否应该乘上物体颜色？

    ambient *= light.strength;
    diffuse *= light.strength;
    specular *= light.strength;

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0f);

    // 布林-冯高光
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), material._SpecularStrength);

    // 光衰减
    float lightDistance = length(light.position - fragPos);
    float lightAttenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));


    vec3 ambient = light.color * light.ambient * material._BaseColor;
    vec3 diffuse = light.color * light.diffuse * diff * material._BaseColor;
    vec3 specular = light.color * light.specular * spec;

    // 将光照结果乘上光照强度
    ambient *= light.strength;
    diffuse *= light.strength;
    specular *= light.strength;

    // 光照结果乘上光衰减系数
    ambient *= lightAttenuation;
    diffuse *= lightAttenuation;
    specular *= lightAttenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0f);

    // 布林-冯高光
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), material._SpecularStrength);

    // 聚光效果
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    // 光衰减
    float lightDistance = length(light.position - fragPos);
    float lightAttenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

    // 合并光照效果
    vec3 ambient = light.color * light.ambient * material._BaseColor;
    vec3 diffuse = light.color * light.diffuse * diff * material._BaseColor;
    vec3 specular = light.color * light.specular * spec;

    // 将光照结果乘上光照强度
    ambient *= light.strength;
    diffuse *= light.strength;
    specular *= light.strength;

    diffuse *= intensity;
    specular *= intensity;

    ambient *= lightAttenuation;
    diffuse *= lightAttenuation;
    specular *= lightAttenuation;

    return (ambient + diffuse + specular);
}