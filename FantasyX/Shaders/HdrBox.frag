#version 330 core
out vec4 FragColor;

in vec3 localPos;

uniform sampler2D hdrTexture;

const vec2 invAtan = vec2(0.1591f, 0.3183f);

vec2 SampleSphericalMap(vec3 v);

void main()
{
	vec2 uv = SampleSphericalMap(normalize(localPos));
	vec3 color = texture(hdrTexture, uv).rgb;

	FragColor = vec4(color, 1.0f);
}

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5f;
	return uv;
}