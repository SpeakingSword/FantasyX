#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 texCoord;
} fs_in;

uniform samplerCube skybox;

void main()
{   
	vec3 color = texture(skybox, fs_in.texCoord).rgb;
    FragColor = vec4(color, 1.0f);
}