#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 inTexturePosition;

out vec3 vColor;
out vec2 TexturePosition;

void main() {
    gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);
    vColor = Color;
    TexturePosition = inTexturePosition;
}
