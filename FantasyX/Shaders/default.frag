#version 330 core
#define NUM_POINT_LIGHTS 4

// ���ʣ�����������ͨ����
struct Material {
	vec3 color;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_reflection1;
	sampler2D texture_displacement1;
	float shininess;
};

// �۹�
struct SpotLight {
	float strength;

	vec3 position;	
	vec3 direction;
	float cutOff;
	float outerCutOff;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

// �����
struct DirLight {
	float strength;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// ���
struct PointLight {
	float strength;

	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// ������ɫ��������������
in VS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	vec4 FragPosLightSpace;		// ���㶨�������Ӱʱʹ��
	mat3 TBN;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// ��ɫ����ʼ������
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

// ͨ����ͼ����ͬʱӦ�ø����ģ��
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform samplerCube skybox;							// ������ͼ
uniform samplerCube pointLightShadowMap;			// ���Դ��Ӱ��ͼ
uniform sampler2D dirLightShadowMap;			    // �������Ӱ��ͼ

// ÿһ֡��������µĲ���
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform vec3 lightPos;								// ���Դλ��
uniform vec3 viewPos;								// �����λ��
vec2 offsetTexCoord;								// ���û����������ͼλ��

// �ɵ��ڵĲ���
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform float height_scale;							// �û���ǿ��
uniform float bias;									// ��Ӱƫ��
uniform float pointLight_far_plane;					// �����ƾ���
vec3 sampleOffsetDirections[8] = vec3[]				// ���Դ����Ӱ������
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1)
   //vec3( 1,  1,  0), vec3( 1, -1,  0)
);

// ״̬����
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform bool containTexture;						// ��������
uniform bool specularOn;							// �Ƿ����߹�
uniform bool diffuseOn;								// �Ƿ���������
uniform bool normalMapOn;							// �Ƿ�ʹ�÷�����ͼ
uniform bool displacementMapOn;						// �Ƿ�ʹ���û���ͼ
uniform bool discardEdgeOn;							// �Ƿ񶪵���Ե
uniform bool Reflection;							// �Ƿ����������䣨ʹ�������ͼ��
uniform bool Refraction;							// �Ƿ����������䣨ʹ�������ͼ��
uniform bool blinnLightOn;							// �Ƿ��л�ΪBlinn-Phong����
uniform bool dirLightShadowOn;						// �Ƿ����������Ӱ 
uniform bool pointLightShadowOn;					// �Ƿ������Դ��Ӱ 


vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);
vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateReflection(vec3 normal, vec3 viewDir);
float LinearizeDepth(float depth);
float dirLightShadowCalculation(vec4 fragPosLightSpace);
float pointLightShadowCalculation(vec3 fragPos);

void main()
{
	// ��ʼ��ƫ�ƺ����������
	offsetTexCoord = fs_in.TexCoord;
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	if (displacementMapOn)
	{
		// just get offsetTextCoord
		vec3 tempViewDir = normalize(fs_in.TBN * (viewPos - fs_in.FragPos));
		offsetTexCoord = ParallaxMapping(fs_in.TexCoord, tempViewDir);
		if((offsetTexCoord.x > 1.0 || offsetTexCoord.y > 1.0 || offsetTexCoord.x < 0.0 || offsetTexCoord.y < 0.0) && discardEdgeOn)
			discard;
	}
	
	if (normalMapOn)		// ���ʹ�÷�����ͼ
	{
		norm = texture(material.texture_normal1, offsetTexCoord).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(fs_in.TBN * norm);		// ��������ͼ�ṩ�ķ�����ת��������ռ�
												// �˷�����������Ϊ�����Ĺ��ռ�������
	}

	// �����
	vec3 result = CalculateDirLight(dirLight, norm, viewDir);
	// ���
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
		result += CalculatePointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
	// �۹�
	result += CalculateSpotLight(spotLight, norm, fs_in.FragPos, viewDir);

	//���������ɫ
	FragColor = vec4(result, 1.0f);

	if (Reflection)
	{
		FragColor = vec4(result + CalculateReflection(norm, viewDir) * texture(material.texture_reflection1, fs_in.TexCoord).rgb, 1.0f);
	}

	if (Refraction)
	{
		float radio = 1.0f / 1.33f;
		vec3 refractDir = refract(-viewDir, norm, radio);
		FragColor = vec4(texture(skybox, refractDir).rgb, 1.0f);
	}

	// ��ȡ�������֣����ȴ���1��
	float brightness = dot(vec3(0.2126f, 0.7152f, 0.0722f), FragColor.rgb);
	if (brightness > 1.0)
	{
		BrightColor = vec4(FragColor.rgb, 1.0f);
	}
	else
	{
		BrightColor = vec4(vec3(0.0f), 1.0f);
	}
}

// ������Դ��Ӱ
// --------------------------------------------------------------------------------------------------------------------------------------------
float pointLightShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    // ��ȡ��ǳ���ֵ    
    float closestDepth = texture(pointLightShadowMap, fragToLight).r;
    // ��ȡ��ǰƬ�ε����ֵ
    float currentDepth = length(fragToLight);
    // ������Ӱ
	float shadow = 0.0f;
	int samples = 10;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0f + viewDistance / pointLight_far_plane) / 25.0f; // [0.04, 0.08]
	for(int i = 0; i < samples; ++i)
	{
	    float closestDepth = texture(pointLightShadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
	    closestDepth *= pointLight_far_plane;
	    if(currentDepth - bias > closestDepth)
	        shadow += 1.0;
	}
	shadow /= float(samples);

    return shadow;
}

// ���㶨�����Ӱ
// --------------------------------------------------------------------------------------------------------------------------------------------
float dirLightShadowCalculation(vec4 fragPosLightSpace)
{
	float shadow;
	// ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �任��[0,1]�ķ�Χ
    projCoords = projCoords * 0.5 + 0.5;
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // PCF(��Χ���������ȡƽ��ֵ)
	// ��ȡ�����ͼÿ�����صĿ��
	vec2 texelSize = 1.0f / textureSize(dirLightShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
		for (int y = -1; y <= 1; ++y)
		{
			// ������Χ������ֵ
			float pcfDepth = texture(dirLightShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	// ��9����Χ�����ֵȡƽ��ֵ
    shadow /= 9.0f;

	// ��Ƭ�γ�����Ӱ��׶��Զƽ��ʱ��û����Ӱ
	if (projCoords.z > 1.0f)
		shadow = 0.0f;

    return shadow;
}

// ���㷴����ɫ
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculateReflection(vec3 normal, vec3 viewDir)
{
	vec3 refelctDir = reflect(-viewDir, normal);
	return texture(skybox, refelctDir).rgb;
}

// �����������ֵ
// --------------------------------------------------------------------------------------------------------------------------------------------
float LinearizeDepth(float depth)
{
	float near = 0.1f;
	float far = 100.0f;
	float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));  
}

// ����Ӧ���û���ͼ��ı仯����������
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{
	//float minLayers = 8.0f;
	//float maxLayers = 32.0f;
	float numLayers = 10;//mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));

	float layerDepth = 1.0f / numLayers;// ÿ�����ȣ������֮��ľ��룩
	float currentLayerDepth = 0.0f;		// ��¼��ǰ���ڲ㣨�����������)

	vec2 p = viewDir.xy * height_scale;	// ��ȡP����
	vec2 deltaTexCoord = p / numLayers;	// ��P������Ϊ������������һ���ࣩ������
	vec2 currentTexCoord = texCoord;	// �����������
	// ��ǰ���Ӧ�����ֵ���������ͼ����������
	float currentDepthMapValue = texture(material.texture_displacement1, currentTexCoord).r;	
	// ���㱾������ֵС���ڱ�������������ֵʱ
	while (currentLayerDepth < currentDepthMapValue)
	{
		// ִ����������ƫ�ƣ���P�����ƶ� P / numLayers ��������һ��������
		currentTexCoord -= deltaTexCoord;
		// ��ȡƫ�ƺ�����������ֵ
		currentDepthMapValue = texture(material.texture_displacement1, currentTexCoord).r;
		// �Ƶ���һ��
		currentLayerDepth += layerDepth;
	}

	// �õ���һ���Ӧ����������
	vec2 prevTexCoord = currentTexCoord + deltaTexCoord;
	// ��������ֵ��������ֵ֮��Ĳ�
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(material.texture_displacement1, prevTexCoord).r - (currentLayerDepth - layerDepth);
	// ����������֮���Ȩ��
	float weight = afterDepth / (afterDepth - beforeDepth);
	// ��ȡ��ֵ�����������
	vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0f - weight);

	return finalTexCoord;
}

// ���㶨���
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// ������
	float diff = max(dot(normal, lightDir), 0.0f);
	// �����
	float spec;
	if (blinnLightOn == false)
	{
		// ���ϸ߹�
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}
	else
	{
		// ����-��߹�
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess * 2);
	}

	// ���㶨�����Ӱ
	float shadow = 0.0f;
	if (dirLightShadowOn)
		shadow = dirLightShadowCalculation(fs_in.FragPosLightSpace);

	// �ϲ�����Ч��
	if (containTexture)
	{
		vec3 ambient = light.ambient * texture(material.texture_diffuse1, offsetTexCoord).rgb;
		vec3 diffuse = diffuseOn? light.diffuse * diff * texture(material.texture_diffuse1, offsetTexCoord).rgb: vec3(0.0f);
		vec3 specular = specularOn? light.specular * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb: vec3(0.0f);
		ambient *= light.strength;
		diffuse *= light.strength;
		specular *= light.strength;
		return (ambient + (diffuse + specular) * (1.0f - shadow));
	}
	else
	{
		vec3 ambient = light.ambient * material.color;
		vec3 diffuse = diffuseOn? light.diffuse * diff * material.color: vec3(0.0f);
		ambient *= light.strength;
		diffuse *= light.strength;
		return (ambient + diffuse * (1.0f - shadow));
	}
	
}

// ������
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// ������
	float diff = max(dot(normal, lightDir), 0.0f);
	// �����
	float spec;
	if (blinnLightOn == false)
	{
		// ���ϸ߹�
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}
	else
	{
		// ����-��߹�
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess * 2);
	}
	// ��˥��
	float lightDistance = length(light.position - fragPos);
	float lightAttenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

	// ������Դ��Ӱ
	float shadow = 0.0f;
	if (pointLightShadowOn && lightPos == pointLights[1].position)
	{
		shadow = pointLightShadowCalculation(fs_in.FragPos);
	}

	// �ϲ����ս��
	if (containTexture)
	{
		vec3 ambient = light.ambient * texture(material.texture_diffuse1, offsetTexCoord).rgb;
		vec3 diffuse = diffuseOn? light.diffuse * diff * texture(material.texture_diffuse1, offsetTexCoord).rgb: vec3(0.0f);
		vec3 specular = specularOn? light.specular * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb: vec3(0.0f);
		// �����ս�����Ϲ���ǿ��
		ambient *= light.strength;
		diffuse *= light.strength;
		specular *= light.strength;
		// ���ս�����Ϲ�˥��ϵ��
		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;
		specular *= lightAttenuation;
		return (ambient + (diffuse + specular) * (1.0f - shadow));
	}
	else 
	{
		vec3 ambient = light.ambient * material.color;
		vec3 diffuse = diffuseOn? light.diffuse * diff * material.color: vec3(0.0f);
		// �����ս�����Ϲ���ǿ��
		ambient *= light.strength;
		diffuse *= light.strength;

		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;
		return (ambient + diffuse * (1.0f - shadow));
	}
	
}

// ����۹�
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// ������
	float diff = max(dot(normal, lightDir), 0.0f);
	// �����
	float spec;
	if (blinnLightOn == false)
	{
		// ���ϸ߹�
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}
	else
	{
		// ����-��߹�
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess * 2);
	}
	// �۹�Ч��
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	// ��˥��
	float lightDistance = length(light.position - fragPos);
	float lightAttenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
	// �ϲ�����Ч��
	if (containTexture)
	{
		vec3 ambient = light.ambient * texture(material.texture_diffuse1, offsetTexCoord).rgb;
		vec3 diffuse = diffuseOn? light.diffuse * diff * texture(material.texture_diffuse1, offsetTexCoord).rgb: vec3(0.0f);
		vec3 specular = specularOn? light.specular * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb: vec3(0.0f);

		// �����ս�����Ϲ���ǿ��
		ambient *= light.strength;
		diffuse *= light.strength;
		specular *= light.strength;

		diffuse *= intensity;
		specular *= intensity;

		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;
		specular *= lightAttenuation;

		return (ambient + diffuse + specular);
	}
	else
	{
		vec3 ambient = light.ambient * material.color;
		vec3 diffuse = diffuseOn? light.diffuse * diff * material.color: vec3(0.0f);

		// �����ս�����Ϲ���ǿ��
		ambient *= light.strength;
		diffuse *= light.strength;
	
		diffuse *= intensity;

		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;

		return (ambient + diffuse);
	}
	
}
