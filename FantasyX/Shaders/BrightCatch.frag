#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D lightMap;
uniform float exposure;

void main()
{
    vec3 color = texture(lightMap, fs_in.texCoord).rgb;
    float grey = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // É«µ÷Ó³Éä
    float brightness = 1.0f - exp(-grey * exposure);

    if (brightness > 0.8f)
        FragColor = vec4(color, 1.0f);
    else
        FragColor = vec4(vec3(0.0f), 1.0f);
}