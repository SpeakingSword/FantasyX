#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D display;

uniform float gamma;
uniform float exposure;
uniform bool displaySingleColor;

void main()
{
    // 获取高动态范围颜色值
    vec3 hdrColor;
    if (displaySingleColor)
        hdrColor = vec3(texture(display, fs_in.texCoord).r);
    else
        hdrColor = texture(display, fs_in.texCoord).rgb;
	
	// HDR 曝光色调映射
	vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);

	// Gamma校正
	FragColor = vec4(pow(mapped, vec3(1.0f / gamma)), 1.0f);
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}