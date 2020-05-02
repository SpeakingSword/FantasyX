#version 330 core
#define NUM_POINT_LIGHTS 4

// 材质（不包含发光通道）
struct Material {
	vec3 color;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_reflection1;
	sampler2D texture_displacement1;
	float shininess;
};

// 聚光
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

// 定向光
struct DirLight {
	float strength;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// 点光
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

// 顶点着色器传过来的数据
in VS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	vec4 FragPosLightSpace;		// 计算定向光照阴影时使用
	mat3 TBN;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// 着色器初始化参数
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

// 通用贴图，可同时应用给多个模型
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform samplerCube skybox;							// 环境贴图
uniform samplerCube pointLightShadowMap;			// 点光源阴影贴图
uniform sampler2D dirLightShadowMap;			    // 定向光阴影贴图

// 每一帧都必须更新的参数
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform vec3 lightPos;								// 点光源位置
uniform vec3 viewPos;								// 摄像机位置
vec2 offsetTexCoord;								// 被置换后的纹理贴图位置

// 可调节的参数
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform float height_scale;							// 置换的强度
uniform float bias;									// 阴影偏移
uniform float pointLight_far_plane;					// 点光绘制距离
vec3 sampleOffsetDirections[8] = vec3[]				// 点光源软阴影采样点
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1)
   //vec3( 1,  1,  0), vec3( 1, -1,  0)
);

// 状态参数
// --------------------------------------------------------------------------------------------------------------------------------------------
uniform bool containTexture;						// 包含纹理
uniform bool specularOn;							// 是否开启高光
uniform bool diffuseOn;								// 是否开启漫反射
uniform bool normalMapOn;							// 是否使用法线贴图
uniform bool displacementMapOn;						// 是否使用置换贴图
uniform bool discardEdgeOn;							// 是否丢掉边缘
uniform bool Reflection;							// 是否开启环境反射（使用天空贴图）
uniform bool Refraction;							// 是否开启环境折射（使用天空贴图）
uniform bool blinnLightOn;							// 是否切换为Blinn-Phong光照
uniform bool dirLightShadowOn;						// 是否开启定向光阴影 
uniform bool pointLightShadowOn;					// 是否开启点光源阴影 


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
	// 初始化偏移后的纹理坐标
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
	
	if (normalMapOn)		// 如果使用法线贴图
	{
		norm = texture(material.texture_normal1, offsetTexCoord).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(fs_in.TBN * norm);		// 将法线贴图提供的法向量转换至世界空间
												// 此法向量将会作为后续的光照计算向量
	}

	// 定向光
	vec3 result = CalculateDirLight(dirLight, norm, viewDir);
	// 点光
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
		result += CalculatePointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
	// 聚光
	result += CalculateSpotLight(spotLight, norm, fs_in.FragPos, viewDir);

	//输出最终颜色
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

	// 获取高亮部分（亮度大于1）
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

// 计算点光源阴影
// --------------------------------------------------------------------------------------------------------------------------------------------
float pointLightShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    // 获取最浅深度值    
    float closestDepth = texture(pointLightShadowMap, fragToLight).r;
    // 获取当前片段的深度值
    float currentDepth = length(fragToLight);
    // 计算阴影
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

// 计算定向光阴影
// --------------------------------------------------------------------------------------------------------------------------------------------
float dirLightShadowCalculation(vec4 fragPosLightSpace)
{
	float shadow;
	// 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // PCF(周围采样，最后取平均值)
	// 获取深度贴图每个纹素的宽高
	vec2 texelSize = 1.0f / textureSize(dirLightShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
		for (int y = -1; y <= 1; ++y)
		{
			// 采样周围点的深度值
			float pcfDepth = texture(dirLightShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	// 对9个周围的深度值取平均值
    shadow /= 9.0f;

	// 当片段超过阴影视锥体远平面时，没有阴影
	if (projCoords.z > 1.0f)
		shadow = 0.0f;

    return shadow;
}

// 计算反射颜色
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculateReflection(vec3 normal, vec3 viewDir)
{
	vec3 refelctDir = reflect(-viewDir, normal);
	return texture(skybox, refelctDir).rgb;
}

// 计算线性深度值
// --------------------------------------------------------------------------------------------------------------------------------------------
float LinearizeDepth(float depth)
{
	float near = 0.1f;
	float far = 100.0f;
	float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));  
}

// 计算应用置换贴图后的变化的纹理坐标
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{
	//float minLayers = 8.0f;
	//float maxLayers = 32.0f;
	float numLayers = 10;//mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));

	float layerDepth = 1.0f / numLayers;// 每层的深度（层与层之间的距离）
	float currentLayerDepth = 0.0f;		// 记录当前所在层（用深度来代表)

	vec2 p = viewDir.xy * height_scale;	// 获取P向量
	vec2 deltaTexCoord = p / numLayers;	// 将P向量分为层数（跟层数一样多）个向量
	vec2 currentTexCoord = texCoord;	// 纹理坐标起点
	// 当前层对应的深度值（从深度贴图采样而来）
	float currentDepthMapValue = texture(material.texture_displacement1, currentTexCoord).r;	
	// 当层本身的深度值小于在本层采样到的深度值时
	while (currentLayerDepth < currentDepthMapValue)
	{
		// 执行纹理坐标偏移，朝P方向移动 P / numLayers 距离获得下一个采样点
		currentTexCoord -= deltaTexCoord;
		// 获取偏移后采样到的深度值
		currentDepthMapValue = texture(material.texture_displacement1, currentTexCoord).r;
		// 移到下一层
		currentLayerDepth += layerDepth;
	}

	// 得到上一层对应的纹理坐标
	vec2 prevTexCoord = currentTexCoord + deltaTexCoord;
	// 计算层深度值与采样深度值之间的差
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(material.texture_displacement1, prevTexCoord).r - (currentLayerDepth - layerDepth);
	// 计算两个差之间的权重
	float weight = afterDepth / (afterDepth - beforeDepth);
	// 获取插值后的纹理坐标
	vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0f - weight);

	return finalTexCoord;
}

// 计算定向光
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// 漫反射
	float diff = max(dot(normal, lightDir), 0.0f);
	// 镜面光
	float spec;
	if (blinnLightOn == false)
	{
		// 冯氏高光
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}
	else
	{
		// 布林-冯高光
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess * 2);
	}

	// 计算定向光阴影
	float shadow = 0.0f;
	if (dirLightShadowOn)
		shadow = dirLightShadowCalculation(fs_in.FragPosLightSpace);

	// 合并光照效果
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

// 计算点光
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// 漫反射
	float diff = max(dot(normal, lightDir), 0.0f);
	// 镜面光
	float spec;
	if (blinnLightOn == false)
	{
		// 冯氏高光
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}
	else
	{
		// 布林-冯高光
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess * 2);
	}
	// 光衰减
	float lightDistance = length(light.position - fragPos);
	float lightAttenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

	// 计算点光源阴影
	float shadow = 0.0f;
	if (pointLightShadowOn && lightPos == pointLights[1].position)
	{
		shadow = pointLightShadowCalculation(fs_in.FragPos);
	}

	// 合并光照结果
	if (containTexture)
	{
		vec3 ambient = light.ambient * texture(material.texture_diffuse1, offsetTexCoord).rgb;
		vec3 diffuse = diffuseOn? light.diffuse * diff * texture(material.texture_diffuse1, offsetTexCoord).rgb: vec3(0.0f);
		vec3 specular = specularOn? light.specular * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb: vec3(0.0f);
		// 将光照结果乘上光照强度
		ambient *= light.strength;
		diffuse *= light.strength;
		specular *= light.strength;
		// 光照结果乘上光衰减系数
		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;
		specular *= lightAttenuation;
		return (ambient + (diffuse + specular) * (1.0f - shadow));
	}
	else 
	{
		vec3 ambient = light.ambient * material.color;
		vec3 diffuse = diffuseOn? light.diffuse * diff * material.color: vec3(0.0f);
		// 将光照结果乘上光照强度
		ambient *= light.strength;
		diffuse *= light.strength;

		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;
		return (ambient + diffuse * (1.0f - shadow));
	}
	
}

// 计算聚光
// --------------------------------------------------------------------------------------------------------------------------------------------
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// 漫反射
	float diff = max(dot(normal, lightDir), 0.0f);
	// 镜面光
	float spec;
	if (blinnLightOn == false)
	{
		// 冯氏高光
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	}
	else
	{
		// 布林-冯高光
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess * 2);
	}
	// 聚光效果
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	// 光衰减
	float lightDistance = length(light.position - fragPos);
	float lightAttenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
	// 合并光照效果
	if (containTexture)
	{
		vec3 ambient = light.ambient * texture(material.texture_diffuse1, offsetTexCoord).rgb;
		vec3 diffuse = diffuseOn? light.diffuse * diff * texture(material.texture_diffuse1, offsetTexCoord).rgb: vec3(0.0f);
		vec3 specular = specularOn? light.specular * spec * texture(material.texture_specular1, fs_in.TexCoord).rgb: vec3(0.0f);

		// 将光照结果乘上光照强度
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

		// 将光照结果乘上光照强度
		ambient *= light.strength;
		diffuse *= light.strength;
	
		diffuse *= intensity;

		ambient *= lightAttenuation;
		diffuse *= lightAttenuation;

		return (ambient + diffuse);
	}
	
}
