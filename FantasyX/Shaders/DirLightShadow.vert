#version 330 core
layout (location = 0) in vec3 Pos;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(Pos, 1.0f);
}