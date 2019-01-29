#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPosition;
in vec3 Normal;

uniform vec3 viewPosition;

uniform Material material;
uniform Light light;

out vec4 outColor;
  
void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(light.position - FragPosition);

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDirection = normalize(viewPosition - FragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient  + diffuse + specular;
    outColor = vec4(result, 1.0f);
}
