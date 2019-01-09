#version 330 core

out vec4 outColor;
  
uniform vec4 vertexColor;

void main()
{
    outColor = vertexColor;
}
