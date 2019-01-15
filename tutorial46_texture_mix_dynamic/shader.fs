#version 330 core

in vec3 vColor;
in vec2 TexturePosition;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

uniform float textureMixPercent;

out vec4 outColor;
  
void main()
{
    outColor = mix(texture(Texture1, TexturePosition), texture(Texture2, TexturePosition), textureMixPercent);
}
