#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTextureCoordinate;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 textureCoordinate;
out vec3 fragmentPosition;

void main()
{
    gl_Position = projection * view * model * vec4(inPosition, 1.0);

    textureCoordinate = inTextureCoordinate;
    fragmentPosition = vec3(model * vec4(inPosition, 1.0));
    
}