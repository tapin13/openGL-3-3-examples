#version 330 core

in vec2 textureCoordinates;
in vec3 outNormals;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec3 lightPosition;

uniform sampler2D shadowMap;

out vec4 outColor;

float ShadowCalculation(vec4 FragPosLightSpace) {
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(outNormals);
    vec3 lightDirection = normalize(lightPosition - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy +  vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main()
{
    vec3 lightColor = vec3(0.3);
    
    vec3 texture = texture(texture_diffuse, textureCoordinates).rgb;
    // vec4 specular = texture(texture_specular, textureCoordinates);
    // outColor = vec4(diffuse + specular);

    // ambient
    vec3 ambient = 0.3 * lightColor;

    // diffuse
    vec3 norm = normalize(outNormals);
    vec3 lightDirection = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // shadow calculate
    float shadow = ShadowCalculation(FragPosLightSpace);

    outColor = vec4((ambient + (1 - shadow) * diffuse) * texture, 1.0);
}
