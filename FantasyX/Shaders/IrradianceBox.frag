#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube hdr_cube_map;

const float PI = 3.14159265359;

void main()
{
    vec3 normal = normalize(localPos);
    vec3 irradiance = vec3(0.0f);

    vec3 up		= vec3(0.0f, 1.0f, 0.0f);
    vec3 right  = cross(up, normal);
    up			= cross(normal, right);

    float sampleDelta = 0.025f;
    float numSamples = 0.0f;

    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
        for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
        {
            
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(hdr_cube_map, sampleVec).rgb * cos(theta) * sin(theta);
            numSamples++;
        }

    irradiance = PI * irradiance * (1.0f / float(numSamples));

    FragColor = vec4(irradiance, 1.0f);
}