#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

uniform float gamma;
uniform float exposure;

vec3 Sharpen(const float offset);
vec3 Blur(const float offset);
vec3 EdgeDetection(const float offset);

void main()
{
	vec3 hdrColor = texture(screenTexture, TexCoord).rgb;
	//vec3 blurColor = texture(blurTexture, TexCoord).rgb;
	//hdrColor += blurColor;		// ���ģ����ɫ
	// HDR �ع�ɫ��ӳ��
	vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
	// GammaУ��
	FragColor = vec4(pow(mapped, vec3(1.0f / gamma)), 1.0f);
}
		

// ��Ч��
vec3 Sharpen(const float offset)
{
	// ����ƫ��
	vec2 offsets[9] = {
		vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
	};

	// ����������
	float kernel[9] = {
		-1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
	};

	// ��ȡƫ�ƺ���������괦����ɫ
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		//sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
	}

	// ���㵱ǰ�������괦��ɫ���յľ��ֵ
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return color;
}

// ģ��Ч��
vec3 Blur(const float offset)
{
	// ����ƫ��
	vec2 offsets[9] = {
		vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
	};

	// ����������
	float kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	};

	// ��ȡƫ�ƺ���������괦����ɫ
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		//sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
	}

	// ���㵱ǰ�������괦��ɫ���յľ��ֵ
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return color;
}

// ��Ե���Ч��
vec3 EdgeDetection(const float offset)
{
	// ����ƫ��
	vec2 offsets[9] = {
		vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
	};

	// ����������
	float kernel[9] = {
		 1,  1,  1,
         1, -8,  1,
         1,  1,  1
	};

	// ��ȡƫ�ƺ���������괦����ɫ
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		//sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
	}

	// ���㵱ǰ�������괦��ɫ���յľ��ֵ
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return color;
}