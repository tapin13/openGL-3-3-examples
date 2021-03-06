#version 330 core

in vec3 vColor;
in vec2 TexturePosition;

uniform sampler2D Texture;

out vec4 outColor;
  
void main()
{
    outColor = texture(Texture, TexturePosition) * vec4(vColor, 1.0f);
}
