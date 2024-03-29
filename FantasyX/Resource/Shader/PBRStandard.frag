﻿#version 330 core
layout (location = 0) out vec3 gPos;
layout (location = 1) out float gDepth;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out float gMetallic;
layout (location = 4) out vec3 gNormal;
layout (location = 5) out float gRoughness;
layout (location = 6) out float gAo;

in VS_OUT {
    vec2 texCoord;
    vec3 fragPos;
    mat3 TBN;
} fs_in;

struct Material {
    sampler2D _AlbedoMap;
    sampler2D _MetallicMap;
    sampler2D _NormalMap;
    sampler2D _RoughnessMap;
    sampler2D _AoMap;
};

uniform Material material;

layout (std140) uniform SharedCameraValues
{
    float NEAR;
    float FAR;	
};

float LinearizeDepth(float depth);

void main()
{
    gPos = fs_in.fragPos;
    gDepth = LinearizeDepth(gl_FragCoord.z);
    gAlbedo = texture(material._AlbedoMap, fs_in.texCoord).rgb;
    gMetallic = texture(material._MetallicMap, fs_in.texCoord).r;
    vec3 norm = normalize(texture(material._NormalMap, fs_in.texCoord).rgb);
    norm = normalize(norm * 2.0f - 1.0f);
    norm = normalize(fs_in.TBN * norm);
    gNormal = norm;
    gRoughness = texture(material._RoughnessMap, fs_in.texCoord).r;
    gAo = texture(material._AoMap, fs_in.texCoord).r;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}