#version 330 core

in vec2 textureCoordinates;

uniform sampler2D texture1;

out vec4 outColor;

void main()
{
    outColor = texture(texture1, textureCoordinates);
}
