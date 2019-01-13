#version 330 core

in vec3 vColor;
in vec2 TexturePosition;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

out vec4 outColor;
  
void main()
{
    outColor = mix(texture(Texture1, TexturePosition), texture(Texture2, vec2(1.0 - TexturePosition.x, TexturePosition.y)), 0.2);
}
