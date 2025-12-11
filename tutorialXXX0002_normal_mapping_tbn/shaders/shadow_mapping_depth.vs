#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 textureDirection;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(inPosition, 1.0);
}
