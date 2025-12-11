#version 330 core

out vec4 FragColor;
  
in vec2 textureCoordinate;
in vec3 fragmentPosition;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform vec3 viewPosition;

void main()
{
    bool blinnLighting = true;

    vec3 objectColor = texture(diffuseMap, textureCoordinate).rgb;

    vec3 normal = texture(normalMap, textureCoordinate).rgb;
    vec3 rgbNormal = normalize(normal * 2.0 - 1.0);
    // vec3 rgbNormal = vec3(0,0,1);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDirection = (lightPosition - fragmentPosition);
    float distanceToLight = length(lightDirection);
    lightDirection = normalize(lightPosition - fragmentPosition);

    float diff = max(dot(lightDirection, rgbNormal), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    float specularPower = 0;

    if(blinnLighting == true) {
        vec3 halfWayDirection = normalize(lightDirection + viewDirection);
        specularPower = pow(max(dot(rgbNormal, halfWayDirection), 0.0), 16.0);
    } else { // Phong Lighting
        vec3 reflectDirection = reflect(-lightDirection, rgbNormal);
        specularPower = pow(max(dot(viewDirection, reflectDirection), 0.0), 8.0);
    }
    vec3 specular = specularStrength * specularPower * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}