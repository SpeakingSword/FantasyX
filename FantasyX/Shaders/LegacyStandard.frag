#version 330 core

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
    vec3 normal;
} fs_in;

struct Material {
    vec3 _BaseColor;
    float _Metallic;
    float _Roughness;
    float _Ao;
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
    
}