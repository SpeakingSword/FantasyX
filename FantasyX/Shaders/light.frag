#version 330 core
layout (location = 0) out vec4 FragColor;		// ���Ƭ�ε���ɫ����ɫ���帽��
layout (location = 1) out vec4 BrightColor;		// ���Ƭ�ε���ɫ���������帽��

uniform vec3 lightColor;

void main()
{
	FragColor = vec4(lightColor, 1.0f);
	//float brightness = dot(vec3(0.2126f, 0.7152f, 0.0722f), FragColor.rgb);
	//if (brightness > 1.0)
	//{
	//	BrightColor = vec4(FragColor.rgb, 1.0f);
	//}
	//else
	//{
	//	BrightColor = vec4(vec3(0.0f), 1.0f);
	//}
	BrightColor = vec4(FragColor.rgb, 1.0f);
}