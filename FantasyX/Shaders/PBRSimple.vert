#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

out VS_OUT {
	vec2 texCoord;
	vec3 fragPos;
	vec3 normal;
} vs_out;


layout (std140) uniform SharedMatrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

void main()
{
    gl_Position = projection * view * modelMatrix * vec4(Pos, 1.0f);
    vs_out.texCoord = TexCoord;
    vs_out.fragPos = vec3(modelMatrix * vec4(Pos, 1.0f));
    vs_out.normal = mat3(normalMatrix) * Normal;
}