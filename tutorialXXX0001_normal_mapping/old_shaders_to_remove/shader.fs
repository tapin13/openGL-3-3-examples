#version 330 core

in vec3 FragPosition;
in vec3 Normal;
in vec3 LightPosition;

uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 outColor;
  
void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(LightPosition - FragPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDirection = normalize(-FragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient  + diffuse + specular) * objectColor;
    outColor = vec4(result, 1.0f);
}
