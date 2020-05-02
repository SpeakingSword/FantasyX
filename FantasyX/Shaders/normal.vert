#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat4 normalMat;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.normal = normalize(vec3(projection * view * vec4(mat3(normalMat) * aNormal, 0.0)));
}