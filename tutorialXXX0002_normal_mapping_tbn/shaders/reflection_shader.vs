#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 outPosition;
out vec3 outNormal;

void main() {
    outNormal = mat3(transpose(inverse(model))) * inNormal;
    outPosition = vec3(model * vec4(inPosition, 1.0));
    gl_Position = projection * view * vec4(outPosition, 1.0);
}
