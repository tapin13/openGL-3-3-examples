#version 330 core

in vec3 FragPosition;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;

out vec4 outColor;
  
void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - FragPosition);

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 deffuse = diff * lightColor;

    vec3 result = (ambient  + deffuse) * objectColor;
    outColor = vec4(result, 1.0f);
}
