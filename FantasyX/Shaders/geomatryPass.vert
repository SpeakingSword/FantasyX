#version 330 core
layout (location = 0) in vec3 aPos;			// 顶点位置
layout (location = 1) in vec2 aTexCoord;	// 顶点纹理坐标
layout (location = 2) in vec3 aNormal;		// 顶点法向量
layout (location = 3) in vec3 aTangent;		// 顶点切线
layout (location = 4) in vec3 aBitangent;	// 顶点副切线

out VS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	mat3 TBN;
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
	gl_Position =  projection  * view * model * vec4(aPos, 1.0f);
    vs_out.TexCoord = aTexCoord;
	vs_out.Normal = mat3(normalMat) * aNormal;						// 观察空间法向量
	vs_out.FragPos = vec3(view * model * vec4(aPos, 1.0f));			// 观察空间片段位置
	// 计算切线空间变换矩阵
	vec3 T = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	vec3 N = normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 B = normalize(vec3(normalMat * vec4(aBitangent, 0.0f)));
	vs_out.TBN = mat3(T, B, N);
}