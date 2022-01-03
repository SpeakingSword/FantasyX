#version 330 core
layout (location = 0) in vec3 Pos;

out VS_OUT {
    vec3 texCoord;
} vs_out;

layout (std140) uniform SharedMatrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    vs_out.texCoord = Pos;
    vec4 pos = projection * mat4(mat3(view)) * vec4(Pos, 1.0);
    gl_Position = pos.xyww;
}