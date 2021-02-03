#version 330 core

layout (location = 0) in vec3 Position;

uniform mat4 view;
uniform mat4 projection;

out vec3 textureDirection;

void main() {
    textureDirection = Position;
    vec4 pos = projection * view * vec4(Position, 1.0);
    gl_Position = pos.xyww;
}
