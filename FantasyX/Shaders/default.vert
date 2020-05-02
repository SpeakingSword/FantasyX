#version 330 core
layout (location = 0) in vec3 aPos;			// ����λ��
layout (location = 1) in vec2 aTexCoord;	// ������������
layout (location = 2) in vec3 aNormal;		// ���㷨����
layout (location = 3) in vec3 aTangent;		// ��������
layout (location = 4) in vec3 aBitangent;	// ���㸱����
layout (location = 5) in mat4 aModel;		// ����ʵ�������Ƶ�ģ�;���
layout (location = 9) in mat4 aNormalMat;	// ����ʵ�������Ƶķ��߾���

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

uniform bool instancingDraw;		// �Ƿ�Ϊʵ��������
uniform mat4 dirlightSpaceMatrix;	// �����ռ�任����

void main()
{
	gl_Position =  projection  * view * (instancingDraw? aModel: model) * vec4(aPos, 1.0f);
    vs_out.TexCoord = aTexCoord;
	vs_out.Normal = mat3(instancingDraw? aNormalMat: normalMat) * aNormal;
	vs_out.FragPos = vec3((instancingDraw? aModel: model) * vec4(aPos, 1.0f));
	vs_out.FragPosLightSpace = dirlightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	// �������߿ռ�任����
	vec3 T = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	vec3 N = normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 B = normalize(vec3(normalMat * vec4(aBitangent, 0.0f)));
	vs_out.TBN = mat3(T, B, N);
}