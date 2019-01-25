#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 outColor;
  
void main()
{
    outColor = vec4(lightColor * objectColor, 1.0f);
}
