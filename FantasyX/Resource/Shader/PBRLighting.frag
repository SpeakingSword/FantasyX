#version 330 core
#define MAX_POINT_LIGHT 8

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

struct DirLight {
    vec3 color;
    vec3 direction;
    float strength;
};

struct PointLight {
    vec3 color;
    vec3 position;
    float strength;
};

struct SpotLight {
    vec3 color;
    vec3 position;
    vec3 direction;
    float strength;
    float cutOff;
    float outerCutOff;
};

uniform sampler2D gPos;
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallic;
uniform sampler2D gNormal;
uniform sampler2D gRoughness;
uniform sampler2D gAo;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLutMap;

uniform vec3 viewPos;
uniform bool IBL;

uniform DirLight dirLight;
uniform bool dirLightOn;
uniform float dirLightShadowBias;
uniform bool dirLightShadowOn;
uniform mat4 dirLightSpaceMatrix;
uniform sampler2D dirLightShadowMap;

uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform int pointLightNum;

const float PI = 3.14159265359;

float DirLightShadowCalculation(vec4 fragPosLightSpace, float bias);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeomatrySchlickGGX(float NdotV, float roughness);
float GeomatrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main()
{
    vec3 albedo = texture(gAlbedo, fs_in.texCoord).rgb;
    vec3 normal = texture(gNormal, fs_in.texCoord).rgb;
    float metallic = texture(gMetallic, fs_in.texCoord).r;
    float roughness = texture(gRoughness, fs_in.texCoord).r;
    float ao = texture(gAo, fs_in.texCoord).r;
    vec3 fragPos = texture(gPos, fs_in.texCoord).rgb;
    
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - fragPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

    // 保存直接光照最终结果
    vec3 Lo = vec3(0.0f);
    // 保存间接光照最终结果
    vec3 ambient = vec3(0.0f);

    // 定向光光照计算
    if (dirLightOn)
    {
        vec3 L = normalize(-dirLight.direction);
        vec3 H = normalize(V + L);

        // BRDF
        vec3 F = FresnelSchlickRoughness(max(dot(H, V), 0.0f), F0, roughness);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeomatrySmith(N, V, L, roughness);

        vec3 numerator = NDF * F * G;
        float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
        vec3 specular = numerator / max(denominator, 0.001f);

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= (1.0f - metallic);

        float NdotL = max(dot(N, L), 0.0f);
        vec3 dirLightResult = (kD * albedo / PI + specular) * NdotL * dirLight.strength * dirLight.color;
        
        if (dirLightShadowOn)
        {
            //float bias = max(0.008 * (1.0 - dot(N, L)), 0.005);
            float shadow = DirLightShadowCalculation(dirLightSpaceMatrix * vec4(fragPos, 1.0f), dirLightShadowBias);
            dirLightResult = dirLightResult * (1.0f - shadow);
        }

        Lo += dirLightResult;
    }

    if (pointLightNum > 0)
    {
        for (int i = 0; i < pointLightNum; ++i)
        {
            vec3 L = normalize(pointLights[i].position - fragPos);
            vec3 H = normalize(V + L);
            float distance = length(pointLights[i].position - fragPos);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = pointLights[i].color * attenuation;

            vec3 F = FresnelSchlickRoughness(max(dot(H, V), 0.0f), F0, roughness);
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeomatrySmith(N, V, L, roughness);

            vec3 numerator = NDF * F * G;
            float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
            vec3 specular = numerator / max(denominator, 0.001f);

            vec3 kS = F;
            vec3 kD = vec3(1.0f) - kS;
            kD *= (1.0f - metallic);

            float NdotL = max(dot(N, L), 0.0f);
            Lo += (kD * albedo / PI + specular) * NdotL *  radiance * pointLights[i].strength;
        }
    }

    // 执行间接光照计算
    if (IBL)
    {
        vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        
        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse    = irradiance * albedo;
        
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;   
        vec2 envBRDF  = texture(brdfLutMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
        
        ambient = (kD * diffuse + specular) * ao;

    }

    FragColor = vec4(Lo + ambient, 1.0f);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeomatrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeomatrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = GeomatrySchlickGGX(NdotV, roughness);
    float ggx2 = GeomatrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float DirLightShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    float shadow;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec2 texelSize = 1.0f / textureSize(dirLightShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(dirLightShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    shadow /= 9.0f;

    if (projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}