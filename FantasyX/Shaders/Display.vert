#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

out VS_OUT {
	vec2 texCoord;
} vs_out;

void main()
{
    vs_out.texCoord = TexCoord;
    gl_Position = vec4(Pos, 1.0f);
}