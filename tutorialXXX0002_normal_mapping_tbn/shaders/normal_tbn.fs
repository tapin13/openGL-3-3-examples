#version 330 core

out vec4 FragColor;
  
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightColor;

uniform vec3 viewPosition;

in VS_OUT {
    vec3 fragmentPosition;
    vec2 textureCoordinate;
    vec3 TangentLightPosition;
    vec3 TangentViewPosition;
    vec3 TangentFragmentPosition;
} vs_in;

void main()
{
    bool blinnLighting = true;

    vec3 objectColor = texture(diffuseMap, vs_in.textureCoordinate).rgb;

    vec3 normal = texture(normalMap, vs_in.textureCoordinate).rgb;
    vec3 rgbNormal = normalize(normal * 2.0 - 1.0);
    // vec3 rgbNormal = vec3(0.0, 1.0 ,0.0);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * objectColor;

    vec3 lightDirection = normalize(vs_in.TangentLightPosition - vs_in.TangentFragmentPosition);
    float diff = max(dot(lightDirection, rgbNormal), 0.0);
    vec3 diffuse = diff * objectColor;

    float specularStrength = 0.5;
    vec3 viewDirection = normalize(vs_in.TangentViewPosition - vs_in.TangentFragmentPosition);
    float specularPower = 0;

    if(blinnLighting == true) {
        vec3 halfWayDirection = normalize(lightDirection + viewDirection);
        specularPower = pow(max(dot(rgbNormal, halfWayDirection), 0.0), 8.0);
    } else { // Phong Lighting
        vec3 reflectDirection = reflect(-lightDirection, rgbNormal);
        specularPower = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    }
    vec3 specular = specularStrength * specularPower * objectColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}