#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"

#include "bmp_load.h"

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
    window = glfwCreateWindow(640, 480, "", NULL, NULL);
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

    // Texture load

    GLubyte *texture_data = NULL;
    GLuint texture_width;
    GLuint texture_height;
    
    GLuint textureID_1;
    glGenTextures(1, &textureID_1);
    glBindTexture(GL_TEXTURE_2D, textureID_1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    bmp_load("tex.bmp", &texture_data, &texture_width, &texture_height);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture_data);
    
    free(texture_data);
    
    GLuint textureID_2;
    glGenTextures(1, &textureID_2);
    glBindTexture(GL_TEXTURE_2D, textureID_2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    bmp_load("awesomeface.bmp", &texture_data, &texture_width, &texture_height);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, texture_data);
    
    free(texture_data);
    
    static const GLfloat gl_vertex_buffer_data[] = {
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
    };
    
    static const GLuint gl_intex_buffer_data[] = {
        0, 1, 2,
        2, 3, 0,
    };
    
    // Init Shaders, VAO, VBO, Link it
    
    GLuint programId;
    
    programId = loadShader("shader.vs", "shader.fs");
    if(programId == -1) {
        return (EXIT_FAILURE);
    }
    
    // Create VAO
    GLuint vertexArrayObjectID; // VAO
    glGenVertexArrays(1, &vertexArrayObjectID);
    
    // Create VBO
    glBindVertexArray(vertexArrayObjectID);
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);
    
    // Create IBO(EBO)
    glBindVertexArray(vertexArrayObjectID);
    GLuint indexBufferObject;
    glGenBuffers(1, &indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gl_intex_buffer_data), gl_intex_buffer_data, GL_STATIC_DRAW);
    
    // layout (location = 0) in vec3 Position;
    // [x,y,z,r,g,b,x,y,z,r....] -> 3 floats + 3 floats for stride
    glVertexAttribPointer(
        0, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
        (void*)0 // array buffer offset
    );
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(
        1, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
        (void*) (sizeof(GLfloat) * 3) // array buffer offset
    );
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(
        2, //  position vertex attribute in the vertex shader with layout (location = 0)
        2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
        (void*) (sizeof(GLfloat) * 6) // array buffer offset
    );
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);

    GLuint transformID = glGetUniformLocation(programId, "transform");
    
    float angle = 90.0f;
    float angle_cos = cosf(angle * (M_PI / 180.0f));
    float angle_sin = sinf(angle * (M_PI / 180.0f));

    float scale = 0.5f;
    
    float translate_x = 0.0f;
    float translate_y = 0.0f;
    
    GLfloat transformMatrix[4][4] = {
        { 1.0f * angle_cos * scale, -angle_sin * scale, 0, translate_x },
        { angle_sin * scale, 1.0f * angle_cos * scale, 0, translate_y },
        { 0, 0, 1.0f, 0 },
        { 0, 0, 0, 1.0f },
    };
    
    /*
    int ii, jj;
    for(jj = 0; jj < 4; jj++) {
        for(ii = 0; ii < 4; ii++) {
            printf("[%u][%u]: %f\t", jj, ii, transformMatrix[jj][ii]);
        }
        printf("\n");
    }
    */
    
    // Draw time
    
    glUseProgram(programId);
    glUniform1i(glGetUniformLocation(programId, "Texture1"), 0); // once
    glUniform1i(glGetUniformLocation(programId, "Texture2"), 1); // once
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID_2);
    glBindVertexArray(vertexArrayObjectID);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(transformID, 1, GL_TRUE, (const float *)transformMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
    }

    // Clean all
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    
    return (EXIT_SUCCESS);
}