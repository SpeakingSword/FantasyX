#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
	vec2 TexCoord;
	vec3 FragPos;
	vec3 Normal; 
	mat3 TBN;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

// ---------------------------
uniform mat4 model;
uniform mat4 normalMat;
// ---------------------------

void main()
{
    gl_Position =  projection  * view * model * vec4(aPos, 1.0f);
	vs_out.TexCoord = aTexCoord;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.Normal = mat3(normalMat) * aNormal;
	// 计算切线空间变换矩阵
	vec3 T = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	vec3 N = normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 B = normalize(vec3(normalMat * vec4(aBitangent, 0.0f)));
	vs_out.TBN = mat3(T, B, N);
} 