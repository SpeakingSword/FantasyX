#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

struct Light {
    vec3 position;
    vec3 color;
	float radius;
	float constant;
	float linear;
	float quadratic;
};

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

const int NR_LIGHTS = 8;
uniform Light lights[NR_LIGHTS];

uniform bool ssaoOn;

void main()
{             
    // 从G缓冲中获取数据
	vec3 result;

    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = texture(gNormal, TexCoord).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoord).rgb;
    float Specular = texture(gAlbedoSpec, TexCoord).a;

	float AmbientOcclusion = 1.0f;
	if (ssaoOn)
		 AmbientOcclusion = texture(ssao, TexCoord).r;

	vec3 viewDir = normalize(-FragPos);
	vec3 lightDir;

    // 和往常一样地计算光照
    vec3 ambient = vec3(0.3f * Albedo * AmbientOcclusion);  // 环境光
	result += ambient;
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
		// 光衰减
		float lightDistance = length(vec3(view * vec4(lights[i].position, 1.0f)) - FragPos);
		if (lightDistance < lights[i].radius)		// 如果在光体积内才光照
		{
			float lightAttenuation = 1.0f / (lights[i].constant + lights[i].linear * lightDistance + lights[i].quadratic * (lightDistance * lightDistance));
		
			// 漫反射
			lightDir = normalize(vec3(view * vec4(lights[i].position, 1.0f)) - FragPos);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].color;
			diffuse *= lightAttenuation;
			// 高光
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(Normal, halfwayDir), 0.0f), 128.0f);
			vec3 specular = spec * Specular * lights[i].color;
			specular *= lightAttenuation;

			result += (diffuse + specular);
		}
		
    }
    FragColor = vec4(result, 1.0);
}  