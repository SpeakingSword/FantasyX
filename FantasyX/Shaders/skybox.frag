#version 330 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skybox;

void main()
{   
	//vec3 boxColor = texture(skybox, TexCoord).rgb;
	vec3 boxColor = texture(skybox, TexCoord).rgb;
    FragColor = vec4(boxColor, 1.0f);
}