#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inColor;

out vec3 vColor;

uniform float positionOffset;

void main() {
    gl_Position = vec4(Position.x + positionOffset, Position.y, Position.z, 1.0);
    vColor = inColor;
}