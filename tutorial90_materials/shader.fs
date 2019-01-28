#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 FragPosition;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform Material material;

out vec4 outColor;
  
void main()
{
    vec3 ambient = lightColor * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - FragPosition);

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = lightColor * (diff * material.diffuse);

    vec3 viewDirection = normalize(viewPosition - FragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);

    vec3 result = ambient  + diffuse + specular;
    outColor = vec4(result, 1.0f);
}
