#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 inTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPosition;
out vec3 Normal;
out vec2 TextureCoordinates;

void main() {
    FragPosition = vec3(model * vec4(Position, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TextureCoordinates = inTextureCoordinates;

    gl_Position = projection * view * model * vec4(Position, 1.0);
}
