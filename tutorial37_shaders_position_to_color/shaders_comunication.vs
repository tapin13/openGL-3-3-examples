#version 330 core

layout (location = 0) in vec3 Position;

out vec3 vColor;

uniform float positionOffset;

void main() {
    gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);
    vColor = Position;
}
