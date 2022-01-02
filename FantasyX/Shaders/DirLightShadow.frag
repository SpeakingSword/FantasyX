#version 330 core

uniform float NEAR;
uniform float FAR;

float LinearizeDepth(float depth);

void main()
{             
    //gl_FragDepth = LinearizeDepth(gl_FragCoord.z);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}