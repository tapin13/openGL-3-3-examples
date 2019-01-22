#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"

#include "bmp_load.h"

#include "matrix.h"

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

    glEnable(GL_DEPTH_TEST);  
    //glEnable(GL_BLEND);  
    //glEnable(GL_CULL_FACE);  

    // Ensure we can capture Esc.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint windowWidth = 0;
    GLuint windowHeight = 0;
    glfwGetWindowSize(window, (int *)&windowWidth, (int *)&windowHeight);
    //glViewport(0, 0, windowWidth, windowHeight);
    
    // OpenGL start

    // Data

    // Texture load

    GLubyte *texture_data = NULL;
    GLuint texture_width;
    GLuint texture_height;
    
    GLuint textureID_1;
    glGenTextures(1, &textureID_1);
    glBindTexture(GL_TEXTURE_2D, textureID_1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bmp_load("tex.bmp", &texture_data, &texture_width, &texture_height);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture_data);
    
    free(texture_data);
    
    GLuint textureID_2;
    glGenTextures(1, &textureID_2);
    glBindTexture(GL_TEXTURE_2D, textureID_2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    bmp_load("awesomeface.bmp", &texture_data, &texture_width, &texture_height);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, texture_data);
    
    free(texture_data);

// side indexed    
//    static const GLfloat gl_vertex_buffer_data[] = {
//        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
//        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
//        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
//        -0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
//    };
//    static const GLuint gl_intex_buffer_data[] = {
//        0, 1, 2,
//        2, 3, 0,
//    };
    
// color box sides
//static const GLfloat box[] = {
//    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
//     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
//     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
//     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
//    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
//    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
//
//    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
//     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
//     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
//     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
//    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
//    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
//
//    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
//    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
//    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
//    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//};

    static const GLfloat box[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLfloat cubePositions[10][3] = {
        { 0.0f,  0.0f,  0.0f },
        { 2.0f,  5.0f, -10.0f },
        { -1.5f, -2.2f, -2.5f },
        { -3.8f, -2.0f, -19.3f },
        { 2.4f, -0.4f, -3.5f },
        { -1.7f,  3.0f, -7.5f },
        { 1.3f, -2.0f, -2.5f },
        { 1.5f,  2.0f, -2.5f },
        { 1.5f,  0.2f, -1.5f },
        { -1.3f,  1.0f, -1.5f },
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
    //glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    
    // Create IBO(EBO)
//    glBindVertexArray(vertexArrayObjectID);
//    GLuint indexBufferObject;
//    glGenBuffers(1, &indexBufferObject);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gl_intex_buffer_data), gl_intex_buffer_data, GL_STATIC_DRAW);
    
    // layout (location = 0) in vec3 Position;
    // [x,y,z,r,g,b,x,y,z,r....] -> 3 floats + 3 floats for stride
    glVertexAttribPointer(
        0, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 5, // stride - space between consecutive vertex attributes
        (void*)0 // array buffer offset
    );
    glEnableVertexAttribArray(0);
    
//    glVertexAttribPointer(
//        1, //  position vertex attribute in the vertex shader with layout (location = 0)
//        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
//        GL_FLOAT, // type of the data which is GL_FLOAT
//        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
//        sizeof(GLfloat) * 6, // stride - space between consecutive vertex attributes
//        (void*) (sizeof(GLfloat) * 3) // array buffer offset
//    );
//    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(
        2, //  position vertex attribute in the vertex shader with layout (location = 0)
        2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 5, // stride - space between consecutive vertex attributes
        (void*) (sizeof(GLfloat) * 3) // array buffer offset
    );
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    GLuint modelID = glGetUniformLocation(programId, "model");
    GLuint viewID = glGetUniformLocation(programId, "view");
    GLuint projectionID = glGetUniformLocation(programId, "projection");
    
    GLfloat modelMatrix[4][4] = {};
    GLfloat viewMatrix[4][4] = {};
    GLfloat projectionMatrix[4][4] = {};
    
    matrix_identity((GLfloat *)modelMatrix);
    matrix_identity((GLfloat *)viewMatrix);
    matrix_identity((GLfloat *)projectionMatrix);

    GLfloat axes[3] = { 0.5f, 1.0f, 0.0f };

    // camera
    GLfloat cameraPosition[3] = { 0.0f, 0.0f, 3.0f };
    GLfloat cameraTarget[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat cameraDirection[3] = { 
        cameraPosition[0] - cameraTarget[0], 
        cameraPosition[1] - cameraTarget[1], 
        cameraPosition[2] - cameraTarget[2], 
    };
    vector_normalize(cameraDirection);
    
    GLfloat up[3] = { 0.0f, 1.0f, 0.0f };
    GLfloat cameraRight[3] = { up[0], up[1], up[2] };
    vector_cross(cameraRight, cameraDirection);
    vector_normalize(cameraRight);
    
    GLfloat cameraUp[3] = { cameraDirection[0], cameraDirection[1], cameraDirection[2] };
    vector_cross(cameraUp, cameraRight);
    
    float radius = 10.0f;
    float camera_x = sin(glfwGetTime()) * radius;
    float camera_z = cos(glfwGetTime()) * radius;
    
    // Draw time
    
    glUseProgram(programId);
    glUniform1i(glGetUniformLocation(programId, "Texture1"), 0); // once
    glUniform1i(glGetUniformLocation(programId, "Texture2"), 1); // once
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID_2);
    glBindVertexArray(vertexArrayObjectID);

    
//    matrix_rotate(*modelMatrix, 50.0f, axes);
//    matrix_print(*modelMatrix);
    
    //matrix_translate((GLfloat *)viewMatrix, (GLfloat *)cubePositions[1]);
    //matrix_print(*viewMatrix);

    matrix_perspective(55.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    int cubePositions_i = 0;
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera_x = sin(glfwGetTime()) * radius;
        camera_z = cos(glfwGetTime()) * radius;

        // camera calculations
        cameraPosition[0] = camera_x;
        cameraPosition[1] = 0.0f;
        cameraPosition[2] = camera_z;
        //cameraTarget[3] = { 0.0f, 0.0f, 0.0f };
        cameraDirection[0] = cameraPosition[0] - cameraTarget[0];
        cameraDirection[1] = cameraPosition[1] - cameraTarget[1];
        cameraDirection[2] = cameraPosition[2] - cameraTarget[2];
        vector_normalize(cameraDirection);

        //up[3] = { 0.0f, 1.0f, 0.0f };
        cameraRight[0] = up[0];
        cameraRight[1] = up[1];
        cameraRight[2] = up[2];
        vector_cross(cameraRight, cameraDirection);
        vector_normalize(cameraRight);

        cameraUp[0] = cameraDirection[0];
        cameraUp[1] = cameraDirection[1];
        cameraUp[2] = cameraDirection[2];
        vector_cross(cameraUp, cameraRight);

        matrix_identity((GLfloat *)viewMatrix);
        matrix_lootAt((GLfloat *)viewMatrix, cameraRight, cameraUp, cameraDirection, cameraPosition);
        glUniformMatrix4fv(viewID, 1, GL_TRUE, (const float *)viewMatrix);

        for(cubePositions_i = 0; cubePositions_i < 10; cubePositions_i++) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_rotate(*modelMatrix, 10.0 * cubePositions_i, axes);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)cubePositions[cubePositions_i]);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);

            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window);
    }

    // Clean all
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    
    return (EXIT_SUCCESS);
}
