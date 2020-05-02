#version 330 core
layout (location = 0) in vec3 aPos;			// 顶点位置
layout (location = 1) in vec2 aTexCoord;	// 顶点纹理坐标
layout (location = 2) in vec3 aNormal;		// 顶点法向量
layout (location = 3) in vec3 aTangent;		// 顶点切线
layout (location = 4) in vec3 aBitangent;	// 顶点副切线
layout (location = 5) in mat4 aModel;		// 用于实例化绘制的模型矩阵
layout (location = 9) in mat4 aNormalMat;	// 用于实例化绘制的法线矩阵

out VS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	vec4 FragPosLightSpace;
	mat3 TBN;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat4 normalMat;

uniform bool instancingDraw;		// 是否为实例化绘制
uniform mat4 dirlightSpaceMatrix;	// 定向光空间变换矩阵

void main()
{
	gl_Position =  projection  * view * (instancingDraw? aModel: model) * vec4(aPos, 1.0f);
    vs_out.TexCoord = aTexCoord;
	vs_out.Normal = mat3(instancingDraw? aNormalMat: normalMat) * aNormal;
	vs_out.FragPos = vec3((instancingDraw? aModel: model) * vec4(aPos, 1.0f));
	vs_out.FragPosLightSpace = dirlightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	// 计算切线空间变换矩阵
	vec3 T = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	vec3 N = normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 B = normalize(vec3(normalMat * vec4(aBitangent, 0.0f)));
	vs_out.TBN = mat3(T, B, N);
}