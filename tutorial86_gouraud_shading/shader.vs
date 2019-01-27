#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 aNormal;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 LightingColor;

void main() {
    gl_Position = projection * view * model * vec4(Position, 1.0);

    vec3 Position = vec3(model * vec4(Position, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

    float ambientStrength = 0.1;
    vec3 ambient  = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - Position);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 1.0;
    vec3 viewDirection = normalize(viewPosition - Position);
    vec3 reclectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reclectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    LightingColor = ambient + diffuse + specular;
}
