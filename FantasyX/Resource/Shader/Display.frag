#version 330 core
#define POST_NONE 0
#define POST_REVERSAL 1
#define POST_GREY 2
#define POST_SHARPEN 3
#define POST_BLUR 4
#define POST_EDGE_DETECTION 5

#define POST_MAX_OFFSET 0.01f
#define POST_MIN_OFFSET 0.0005f

out vec4 FragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D display;
uniform int postType;
uniform float exposure;
uniform bool displaySingleColor;
uniform float gamma;
uniform float postStrength;

float sharpen_kernel[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float blur_kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float edge_detection_kernel[9] = float[](
    1,  1, 1,
    1, -8, 1,
    1,  1, 1
);

vec3 none();
vec3 reversal();
vec3 grey();
vec3 sharpen(float strength);
vec3 blur(float strength);
vec3 edge_detection(float strength);

void main()
{
    vec3 color = vec3(0.0f);

    switch(postType)
    {
        case POST_NONE:
            color = none();
            break;
        case POST_REVERSAL:
            color = reversal();
            break;
        case POST_GREY:
            color = grey();
            break;
        case POST_SHARPEN:
            color = sharpen(postStrength);
            break;
        case POST_BLUR:
            color = blur(postStrength);
            break;
        case POST_EDGE_DETECTION:
            color = edge_detection(postStrength);
            break;
        default:
            color = none();
            break;
    }

    FragColor = vec4(pow(color, vec3(1.0f / gamma)), 1.0f);
}

vec3 none()
{
    vec3 hdrColor = displaySingleColor ? vec3(texture(display, fs_in.texCoord).r) : texture(display, fs_in.texCoord).rgb;
    vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
    return mapped;
}

vec3 reversal()
{
    vec3 hdrColor = displaySingleColor ? vec3(texture(display, fs_in.texCoord).r) : texture(display, fs_in.texCoord).rgb;
    vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
    return (vec3(1.0f) - mapped);
}

vec3 grey()
{
    vec3 hdrColor = displaySingleColor ? vec3(texture(display, fs_in.texCoord).r) : texture(display, fs_in.texCoord).rgb;
    vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
    return vec3(dot(mapped, vec3(0.2126f, 0.7152f, 0.0722f)));
}

vec3 sharpen(float strength)
{
    float offset = POST_MIN_OFFSET + (POST_MAX_OFFSET - POST_MIN_OFFSET) * (strength / 100);
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        vec3 hdrColor = displaySingleColor ? vec3(texture(display, fs_in.texCoord + offsets[i]).r) : texture(display, fs_in.texCoord + offsets[i]).rgb;
        sampleTex[i] = vec3(1.0f) - exp(-hdrColor * exposure);
    }

    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * sharpen_kernel[i];

    return color;
}

vec3 blur(float strength)
{
    float offset = POST_MIN_OFFSET + (POST_MAX_OFFSET - POST_MIN_OFFSET) * (strength / 100);
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        vec3 hdrColor = displaySingleColor ? vec3(texture(display, fs_in.texCoord + offsets[i]).r) : texture(display, fs_in.texCoord + offsets[i]).rgb;
        sampleTex[i] = vec3(1.0f) - exp(-hdrColor * exposure);
    }

    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * blur_kernel[i];

    return color;
}

vec3 edge_detection(float strength)
{
    float offset = POST_MIN_OFFSET + (POST_MAX_OFFSET - POST_MIN_OFFSET) * (strength / 100);
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        vec3 hdrColor = displaySingleColor ? vec3(texture(display, fs_in.texCoord + offsets[i]).r) : texture(display, fs_in.texCoord + offsets[i]).rgb;
        sampleTex[i] = vec3(1.0f) - exp(-hdrColor * exposure);
    }

    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * edge_detection_kernel[i];

    return color;
}