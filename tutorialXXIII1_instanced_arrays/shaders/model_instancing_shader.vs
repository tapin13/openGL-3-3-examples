#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inNorals;
layout (location = 2) in vec2 inTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offsets[10 * 10 * 10];

out vec2 textureCoordinates;

void main() {
    textureCoordinates = inTextureCoordinates;
    vec3 offset = offsets[gl_InstanceID];
    gl_Position = projection * view * model * vec4(Position + offset, 1.0);
}
