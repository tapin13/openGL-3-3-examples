#version 330 core

in vec3 LightingColor;

uniform vec3 objectColor;

out vec4 outColor;
  
void main()
{
    outColor = vec4(LightingColor * objectColor, 1.0f);
}
