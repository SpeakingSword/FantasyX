#version 330 core
layout (location = 0) in vec3 aPos;			// ����λ��
layout (location = 1) in vec2 aTexCoord;	// ������������
layout (location = 2) in vec3 aNormal;		// ���㷨����
layout (location = 3) in vec3 aTangent;		// ��������
layout (location = 4) in vec3 aBitangent;	// ���㸱����

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
	vs_out.Normal = mat3(normalMat) * aNormal;						// �۲�ռ䷨����
	vs_out.FragPos = vec3(view * model * vec4(aPos, 1.0f));			// �۲�ռ�Ƭ��λ��
	// �������߿ռ�任����
	vec3 T = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	vec3 N = normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 B = normalize(vec3(normalMat * vec4(aBitangent, 0.0f)));
	vs_out.TBN = mat3(T, B, N);
}