#version 330 core

in vec2 textureCoordinates;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

out vec4 outColor;

void main()
{
    vec4 diffuse = texture(texture_diffuse, textureCoordinates);
    // vec4 specular = texture(texture_specular, textureCoordinates);
    // outColor = vec4(diffuse + specular);
    outColor = diffuse;
}
