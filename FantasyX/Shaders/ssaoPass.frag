#version 330 core
out float FragColor;
in vec2 TexCoord;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[24];
uniform mat4 projection;
uniform int kernelSize;
uniform float radius;
uniform int power;

// ��Ļ��ƽ����������������Ļ�ֱ��ʳ���������С��ֵ������
const vec2 noiseScale = vec2(1366.0/4.0, 768.0/4.0);

void main()
{
	vec3 fragPos = texture(gPositionDepth, TexCoord).xyz;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec3 randomVec = texture(texNoise, TexCoord * noiseScale).xyz;
	// �����۲�ռ��TBN����
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	// �Ժ����������е���
	float occlusion;
	for (int i = 0; i < kernelSize; ++i)
	{
		// ��ȡ����λ��
		vec3 mSample = TBN * samples[i];	// ���߿ռ�-> �۲�ռ�
		mSample = fragPos + mSample * radius;
		// ������λ�ñ任����Ļ�ռ�
		vec4 offset = vec4(mSample, 1.0f);
		offset = projection * offset;
		offset.xyz /= offset.w;		//  ͸�ӳ���
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		// ȡ������Ӧλ�ô洢�����ֵ
		float sampleDepth = -texture(gPositionDepth, offset.xy).a;
		// ��������������ֵ����洢�����ֵ�Ա�
		float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));	// ��Χ��⣬��ֻ��������Ӧλ�ô洢�����ֵ��raidus������
																							// �乱�ײŻ�Խ��
		occlusion += (sampleDepth >= mSample.z ? 1.0f : 0.0f) * rangeCheck;
	}

	occlusion = 1.0f - (occlusion / kernelSize);
	FragColor = pow(occlusion, power);
}