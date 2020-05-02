#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material {
	vec3 color;
	sampler2D texture_diffuse1;
	sampler2D texture_normal1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_displacement1;
	float shininess;
};

in VS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	mat3 TBN;
} fs_in;


uniform Material material;

uniform bool diffuseMapOn;
uniform bool normalMapOn;
uniform bool specularMapOn;

uniform float NEAR;		// ͶӰ����Ľ�ƽ��
uniform float FAR;		// ͶӰ�����Զƽ��

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{    
    // �洢Ƭ��λ��
    gPosition.rgb = fs_in.FragPos;
	// �洢Ƭ�ε��������ֵ
	gPosition.a = LinearizeDepth(gl_FragCoord.z);
    // ͬ���洢��ÿ����Ƭ�η��ߵ�G������
	vec3 norm = normalize(fs_in.Normal);
	if (normalMapOn)
	{
		norm = texture(material.texture_normal1, fs_in.TexCoord).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(fs_in.TBN * norm);		// ��������ͼ�ṩ�ķ�����ת��������ռ�/�۲�ռ�
												// �˷�����������Ϊ�����Ĺ��ռ�������
	}
    gNormal = norm;

	// ���������ÿ����Ƭ����ɫ
	gAlbedoSpec.rgb = material.color;
	if (diffuseMapOn)
	{
		gAlbedoSpec.rgb = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
	}
    
	// �洢����ǿ�ȵ�gAlbedoSpec��alpha����
	gAlbedoSpec.a = 0.1f;
	if (specularMapOn)
	{
		gAlbedoSpec.a = texture(material.texture_specular1, fs_in.TexCoord).r;
	}
}  
