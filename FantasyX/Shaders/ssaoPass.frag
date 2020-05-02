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

// 屏幕的平铺噪声纹理会根据屏幕分辨率除以噪声大小的值来决定
const vec2 noiseScale = vec2(1366.0/4.0, 768.0/4.0);

void main()
{
	vec3 fragPos = texture(gPositionDepth, TexCoord).xyz;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec3 randomVec = texture(texNoise, TexCoord * noiseScale).xyz;
	// 创建观察空间的TBN矩阵
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	// 对核心样本进行迭代
	float occlusion;
	for (int i = 0; i < kernelSize; ++i)
	{
		// 获取样本位置
		vec3 mSample = TBN * samples[i];	// 切线空间-> 观察空间
		mSample = fragPos + mSample * radius;
		// 将样本位置变换到屏幕空间
		vec4 offset = vec4(mSample, 1.0f);
		offset = projection * offset;
		offset.xyz /= offset.w;		//  透视除法
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		// 取样本对应位置存储的深度值
		float sampleDepth = -texture(gPositionDepth, offset.xy).a;
		// 将样本本身的深度值与其存储的深度值对比
		float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));	// 范围检测，当只有样本对应位置存储的深度值在raidus距离内
																							// 其贡献才会越大
		occlusion += (sampleDepth >= mSample.z ? 1.0f : 0.0f) * rangeCheck;
	}

	occlusion = 1.0f - (occlusion / kernelSize);
	FragColor = pow(occlusion, power);
}