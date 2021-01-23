#version 330 core

in vec2 textureCoordinates;

uniform sampler2D texture1;

out vec4 outColor;

void main()
{

    vec4 textureColor = texture(texture1, textureCoordinates);
    if(textureColor.a < 0.1) {
        discard;
    }
    
    outColor = textureColor;
}
