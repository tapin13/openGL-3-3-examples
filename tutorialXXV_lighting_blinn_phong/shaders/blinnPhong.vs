#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec2 inTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentPosition;
out vec3 normals;
out vec2 textureCoordinates;

void main() {
    fragmentPosition = inPosition;
    normals = inNormals;
    textureCoordinates = inTextureCoordinates;

    gl_Position = projection * view * model * vec4(inPosition, 1.0);
}
