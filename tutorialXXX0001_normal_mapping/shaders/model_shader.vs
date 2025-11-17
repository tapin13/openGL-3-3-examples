#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec2 inTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec2 textureCoordinates;
out vec3 outNormals;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main() {
    textureCoordinates = inTextureCoordinates;
    gl_Position = projection * view * model * vec4(Position, 1.0);
    outNormals = inNormals;
    FragPos = vec3(model * vec4(Position, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}
