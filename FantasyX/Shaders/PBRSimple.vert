#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 2) in vec3 Normal;

out VS_OUT {
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
    vs_out.fragPos = vec3(modelMatrix * vec4(Pos, 1.0f));
    vs_out.normal = mat3(normalMatrix) * Normal;
}