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
	//hdrColor += blurColor;		// 混合模糊颜色
	// HDR 曝光色调映射
	vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
	// Gamma校正
	FragColor = vec4(pow(mapped, vec3(1.0f / gamma)), 1.0f);
}
		

// 锐化效果
vec3 Sharpen(const float offset)
{
	// 定义偏移
	vec2 offsets[9] = {
		vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
	};

	// 定义卷积矩阵
	float kernel[9] = {
		-1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
	};

	// 获取偏移后的纹理坐标处的颜色
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		//sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
	}

	// 计算当前纹理坐标处颜色最终的卷积值
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return color;
}

// 模糊效果
vec3 Blur(const float offset)
{
	// 定义偏移
	vec2 offsets[9] = {
		vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
	};

	// 定义卷积矩阵
	float kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	};

	// 获取偏移后的纹理坐标处的颜色
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		//sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
	}

	// 计算当前纹理坐标处颜色最终的卷积值
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return color;
}

// 边缘检测效果
vec3 EdgeDetection(const float offset)
{
	// 定义偏移
	vec2 offsets[9] = {
		vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
	};

	// 定义卷积矩阵
	float kernel[9] = {
		 1,  1,  1,
         1, -8,  1,
         1,  1,  1
	};

	// 获取偏移后的纹理坐标处的颜色
	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		//sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
	}

	// 计算当前纹理坐标处颜色最终的卷积值
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
	{
		color += sampleTex[i] * kernel[i];
	}

	return color;
}