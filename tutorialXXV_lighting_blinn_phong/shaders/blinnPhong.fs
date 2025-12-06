#version 330 core

in vec3 fragmentPosition;
in vec3 normals;
in vec2 textureCoordinates;

uniform sampler2D myTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 outColor;

void main()
{
    vec3 color = texture(myTexture, textureCoordinates).rgb;

    vec3 ambient = 0.05 * color;

    vec3 lightDirection = normalize(lightPos - fragmentPosition);
    vec3 normal = normalize(normals);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * color;

    vec3 viewDir = normalize(viewPos - fragmentPosition);
    float spec = 0.0;

    // blinn
    vec3 halfWayDir = normalize(lightDirection + viewDir);
    spec = pow(max(dot(normal, halfWayDir), 0.0), 32.0);

    // phong
    // vec3 reflectDir = reflect(-lightPos, normal);
    // spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

    vec3 specular = vec3(0.3) * spec;

    outColor = vec4(ambient + diffuse + specular, 1.0);
}
