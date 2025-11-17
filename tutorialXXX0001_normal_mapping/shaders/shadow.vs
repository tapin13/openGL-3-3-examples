#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec2 inTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

// out vec2 textureCoordinates;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main() {
    vs_out.FragPos = vec3(model * vec4(inPosition, 1.0));
    vs_out.Normal =  inNormals;
    vs_out.TexCoords = inTextureCoordinates;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    // textureCoordinates = inTextureCoordinates;
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
}
