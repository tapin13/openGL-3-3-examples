#version 330 core

layout (location = 0) in vec3 Pos;

# !!! Both have the same type and name, the vertexColor in the fragment shader is linked to the vertexColor in the vertex shader.
out vec4 vertexColor; 

void main() {
    gl_Position = vec4(Pos.x, Pos.y, Pos.z, 1.0);
    vertexColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
