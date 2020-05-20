#version 330 core
out vec4 FragColor;
#define POST_NONE 0
#define POST_REVERSAL 1

in VS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D display;
uniform int postType;
uniform float exposure;
uniform bool displaySingleColor;
uniform float gamma;

void main()
{
    vec3 hdrColor;
    if (displaySingleColor)
        hdrColor = vec3(texture(display, fs_in.texCoord).r);
    else
        hdrColor = texture(display, fs_in.texCoord).rgb;

    vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);

    switch(postType)
    {
        case POST_NONE:
            break;
        case POST_REVERSAL:
            mapped = vec3(1.0f) - mapped;
            break;
        default:
            break;
    }

	// GammaÐ£Õý
	FragColor = vec4(pow(mapped, vec3(1.0f / gamma)), 1.0f);
}