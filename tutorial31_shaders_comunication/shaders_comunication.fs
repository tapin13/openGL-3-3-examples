#version 330 core

# !!! Both have the same type and name, the vertexColor in the fragment shader is linked to the vertexColor in the vertex shader.

out vec4 outColor;
  
in vec4 vertexColor;

void main()
{
    outColor = vertexColor;
}
