#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec2 texCoord;
} fs_in;

uniform sampler2D lightMap;
uniform sampler2D blurMap;

void main()
{             
     vec3 color = texture(lightMap, fs_in.texCoord).rgb;
     vec3 blur = texture(blurMap, fs_in.texCoord).rgb;
     FragColor = vec4(color + blur, 1.0f);
}