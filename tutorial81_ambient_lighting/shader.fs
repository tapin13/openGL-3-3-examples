#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 outColor;
  
void main()
{
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * objectColor;
    outColor = vec4(result, 1.0f);
}
