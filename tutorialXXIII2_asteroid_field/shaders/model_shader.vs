#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inNorals;
layout (location = 2) in vec2 inTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 textureCoordinates;

void main() {
    textureCoordinates = inTextureCoordinates;
    gl_Position = projection * view * model * vec4(Position, 1.0);
}
