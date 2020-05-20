#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;

out VS_OUT {
	vec2 texCoord;
} vs_out;

void main()
{
    vs_out.texCoord = TexCoord;
    gl_Position = vec4(Pos, 1.0f);
}