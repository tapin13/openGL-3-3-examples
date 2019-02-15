#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalculationDirectionLight(DirectionLight light, vec3 normal, vec3 viewDirection);

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalculationPointLight(PointLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection);

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalculationSpotLight(SpotLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection);

in vec3 FragPosition;
in vec3 Normal;
in vec2 TextureCoordinates;

uniform vec3 viewPosition;

uniform Material material;

uniform DirectionLight directionLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

out vec4 outColor;
  
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(viewPosition - FragPosition);

    vec3 result = CalculationDirectionLight(directionLight, norm, viewDirection);
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalculationPointLight(pointLights[i], norm, FragPosition, viewDirection);
    }
    result += CalculationSpotLight(spotLight, norm, FragPosition, viewDirection);
    outColor = vec4(result, 1.0f);
    //outColor = vec4(1.0f);
}

vec3 CalculationDirectionLight(DirectionLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(-light.direction);

    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuse, TextureCoordinates).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextureCoordinates).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TextureCoordinates).rgb;

    return (ambient + diffuse + specular);
}

vec3 CalculationPointLight(PointLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - FragPosition);

    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    float lightDistance = length(light.position - FragPosition);
    float lightAttenuation = 1.0 / ( light.constant + light.linear * lightDistance + light.quadratic * ( lightDistance * lightDistance ) );

    vec3 ambient = light.ambient * texture(material.diffuse, TextureCoordinates).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextureCoordinates).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TextureCoordinates).rgb;

    ambient *= lightAttenuation;
    diffuse *= lightAttenuation; 
    specular *= lightAttenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculationSpotLight(SpotLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - FragPosition);

    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDirection), 0.0);

    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    float lightDistance = length(light.position - FragPosition);
    float lightAttenuation = 1.0 / ( light.constant + light.linear * lightDistance + light.quadratic * ( lightDistance * lightDistance ) );

    vec3 ambient = light.ambient * texture(material.diffuse, TextureCoordinates).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextureCoordinates).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TextureCoordinates).rgb;

    diffuse *= lightAttenuation; 
    specular *= lightAttenuation;

    diffuse *= intensity; 
    specular *= intensity;

    return (ambient + diffuse + specular);
}