#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"

#define true  1
#define false 0

int main(int argc, char** argv) {
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.");
        return EXIT_FAILURE;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't use old openGL
    
    GLFWwindow *window;
    window = glfwCreateWindow(640, 480, "Tutorial 2.4 - Two fragment shader -> Two triangles", NULL, NULL);
    if(window == NULL) {
        fprintf(stderr, "Failed to open GLFW windows. If you have an Intel GPU, they are not 3.3 compatible.");
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW");
        return EXIT_FAILURE;
    }
    
    // Ensure we can capture Esc.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // OpenGL start

    // Data
    
    static const GLfloat gl_vertex_buffer_data[] = {
        -0.5f, 0.5f, 0.0f,
        -0.1f, -0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,
                
        0.5f, 0.5f, 0.0f,
        0.1f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    // Init Shaders, VAO, VBO, Link it
    
    GLuint programId[2];
    
    programId[0] = loadShader("two_triangles.vs", "two_triangles.fs");
    if(programId[0] == -1) {
        return (EXIT_FAILURE);
    }
    
    programId[1] = loadShader("two_triangles.vs", "two_triangles_2.fs");
    if(programId[1] == -1) {
        return (EXIT_FAILURE);
    }
    
    // Create Two VAO
    GLuint vertexArrayObjectID_1; // VAO
    glGenVertexArrays(1, &vertexArrayObjectID_1);
    
    GLuint vertexArrayObjectID_2; // VAO
    glGenVertexArrays(1, &vertexArrayObjectID_2);

    // Create Two VBO
    glBindVertexArray(vertexArrayObjectID_1);
    GLuint vertexBufferObject_1; // VBO
    glGenBuffers(1, &vertexBufferObject_1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject_1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(
        0, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        0, // stride - space between consecutive vertex attributes
        (void*)0 // array buffer offset
    );
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    glBindVertexArray(vertexArrayObjectID_2);
    GLuint vertexBufferObject_2; // VBO
    glGenBuffers(1, &vertexBufferObject_2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject_2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(
        0, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        3 * sizeof(GLfloat), // stride - space between consecutive vertex attributes
        (void*)(sizeof(GLfloat) * 3 * 3) // array buffer offset
            // 3 - x, y, z  and 3 point
    );
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    // Draw time
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(programId[0]);
    
    glBindVertexArray(vertexArrayObjectID_1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glUseProgram(programId[1]);
    glBindVertexArray(vertexArrayObjectID_2);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Clean all
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
    }
    
    return (EXIT_SUCCESS);
}

