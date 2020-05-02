#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec2 TexCoord;
	vec3 FragPos;
	vec3 Normal; 
	mat3 TBN;
} fs_in;

// ----------------------
struct Material {
	vec3 color;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_reflection1;
	sampler2D texture_displacement1;
	sampler2D texture_metallic1;
	sampler2D texture_roughness1;
	sampler2D texture_ao1;
	float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};

uniform vec3 viewPos;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform Material material;
uniform int lightNum;
uniform Light lights[4];
// -------------------------


const float PI = 3.14159265359;

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeomatrySchlickGGX(float NdotV, float roughness);
float GeomatrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 GetWorldSpaceNormal(mat3 TBN, vec3 norm);

void main()
{   
	vec3 albedo = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;
	vec3 normal = GetWorldSpaceNormal(fs_in.TBN, texture(material.texture_normal1, fs_in.TexCoord).rgb);
	float metallic = texture(material.texture_metallic1, fs_in.TexCoord).r;
	float roughness = texture(material.texture_roughness1, fs_in.TexCoord).r;
	float ao = texture(material.texture_ao1, fs_in.TexCoord).r;

	vec3 N = normalize(normal);
	vec3 V = normalize(viewPos - fs_in.FragPos);
	vec3 R = reflect(-V, N);

	vec3 F0 = vec3(0.04f);

	// PBR直接光部分
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------------------------
	vec3 Lo = vec3(0.0f);
	for (int i = 0; i < lightNum; i++)
	{
		vec3 L = normalize(lights[i].position - fs_in.FragPos);
		vec3 H = normalize(V + L);

		float light_distance = length(L);
		float light_attenuation = 1.0f / (light_distance * light_distance);
		vec3 light_radiance = lights[i].color * light_attenuation;

		
		F0 = mix(F0, albedo, metallic);
		vec3 F = FresnelSchlickRoughness(max(dot(H, V), 0.0f), F0, roughness);

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeomatrySmith(N, V, L, roughness);

		vec3 numerator = NDF * F * G;
		float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
		vec3 specular = numerator / max(denominator, 0.001f);

		vec3 kS = F;
		vec3 kD = vec3(1.0f) - kS;
		kD *= 1.0f - metallic;

		float NdotL = max(dot(N, L), 0.0f);
		Lo += (kD * albedo / PI + specular) * light_radiance * NdotL;
	}

	// PBR间接光部分
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// 计算IBL环境光
	vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse    = irradiance * albedo;
	
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;   
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	vec3 ambient = (kD * diffuse + specular) * ao; 

	vec3 color = ambient + Lo;
	FragColor = vec4(color, 1.0f);
}

// 将法线贴图的法向量从切线空间转到世界空间
vec3 GetWorldSpaceNormal(mat3 TBN, vec3 norm)
{
	vec3 normal = normalize(norm * 2.0f - 1.0f);
	normal = normalize(TBN * normal);
	return normal;
}

// 菲涅尔方程
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

// 正态分布函数
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return num / denom;
}

// 几何函数
float GeomatrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;

	float num = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return num / denom;

}

// 史密斯法
float GeomatrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float ggx1 = GeomatrySchlickGGX(NdotV, roughness);
	float ggx2 = GeomatrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

