#version 330 core

// in vec2 textureCoordinates;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_in;

uniform sampler2D texture1;
uniform sampler2D shadowMap;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

out vec4 outColor;

float ShadowCalculation(vec4 FragPosLightSpace) {
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(vs_in.Normal);
    vec3 lightDirection = normalize(lightPosition - vs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

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
    vec3 color = texture(texture1, vs_in.TexCoords).rgb;
    vec3 normal = normalize(vs_in.Normal);
    vec3 lightColor = vec3(0.3);

    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDirection = normalize(lightPosition - vs_in.FragPos);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDirection = normalize(viewPosition - vs_in.FragPos);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = 0.0;
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    spec = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // shadow calculate
    float shadow = ShadowCalculation(vs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    outColor = vec4(lighting, 1.0);
}
