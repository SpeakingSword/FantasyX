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

uniform float NEAR;		// 投影矩阵的近平面
uniform float FAR;		// 投影矩阵的远平面

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{    
    // 存储片段位置
    gPosition.rgb = fs_in.FragPos;
	// 存储片段的线性深度值
	gPosition.a = LinearizeDepth(gl_FragCoord.z);
    // 同样存储对每个逐片段法线到G缓冲中
	vec3 norm = normalize(fs_in.Normal);
	if (normalMapOn)
	{
		norm = texture(material.texture_normal1, fs_in.TexCoord).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(fs_in.TBN * norm);		// 将法线贴图提供的法向量转换至世界空间/观察空间
												// 此法向量将会作为后续的光照计算向量
	}
    gNormal = norm;

	// 和漫反射对每个逐片段颜色
	gAlbedoSpec.rgb = material.color;
	if (diffuseMapOn)
	{
		gAlbedoSpec.rgb = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
	}
    
	// 存储镜面强度到gAlbedoSpec的alpha分量
	gAlbedoSpec.a = 0.1f;
	if (specularMapOn)
	{
		gAlbedoSpec.a = texture(material.texture_specular1, fs_in.TexCoord).r;
	}
}  
