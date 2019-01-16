#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 inTexturePosition;

out vec3 vColor;
out vec2 TexturePosition;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(Position, 1.0);
    vColor = Color;
    TexturePosition = inTexturePosition;
}
