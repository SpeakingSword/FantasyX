#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D lightMap;

void main()
{
    vec3 color = texture(lightMap, fs_in.texCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        FragColor = vec4(color, 1.0f);
    else
        FragColor = vec4(vec3(0.0f), 1.0f);
}