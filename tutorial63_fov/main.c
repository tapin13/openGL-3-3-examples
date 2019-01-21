#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"

#include "bmp_load.h"

#define true  1
#define false 0

// angle convert degree to radian
float deg2rad(float angle) { return angle * (M_PI / 180.0f); }

void vector_normalize(float *vector);

void matrix_print(GLfloat *matrix);
void matrix_identity(GLfloat *matrix);
void matrix_translate(GLfloat *matrix, GLfloat *position);
void matrix_scale(float scale, GLfloat *matrix);
void matrix_perspective(float angle, float ratio, float near, float far, GLfloat *matrix);
void matrix_rotate(GLfloat *matrix, float angle, float *axis);

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
    
//    static const GLuint gl_intex_buffer_data[] = {
//        0, 1, 2,
//        2, 3, 0,
//    };
    
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
    
    // Draw time
    
    glUseProgram(programId);
    glUniform1i(glGetUniformLocation(programId, "Texture1"), 0); // once
    glUniform1i(glGetUniformLocation(programId, "Texture2"), 1); // once
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID_2);
    glBindVertexArray(vertexArrayObjectID);

//    float angle = 0.0f;
//    float scale = 1.0f;
//    float translate_x = 0.0f;
//    float translate_y = -0.5f;

    GLfloat modelMatrix[4][4] = {};
    GLfloat viewMatrix[4][4] = {};
    GLfloat projectionMatrix[4][4] = {};
    
    GLfloat axes[3] = { 0.5f, 1.0f, 0.0f };

    matrix_identity((GLfloat *)modelMatrix);
    matrix_identity((GLfloat *)viewMatrix);
    matrix_identity((GLfloat *)projectionMatrix);
    
//    matrix_rotate(*modelMatrix, 50.0f, axes);
//    matrix_print(*modelMatrix);

    GLfloat cubePositions[10][3] = {
        { 0.0f,  0.0f,  -2.2f },
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
    
    //matrix_translate((GLfloat *)viewMatrix, (GLfloat *)cubePositions[1]);
    //matrix_print(*viewMatrix);

    GLuint windowWidth = 0;
    GLuint windowHeight = 0;
    glfwGetWindowSize(window, (int *)&windowWidth, (int *)&windowHeight);
    
    //glViewport(0, 0, windowWidth, windowHeight);
    
    float fov = 0.0;
    float tmp = 1.0;
    int cubePositions_i = 0;
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        tmp = 1.0;
        for(cubePositions_i = 0; cubePositions_i < 10; cubePositions_i++) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_rotate(*modelMatrix, tmp, axes);
            tmp += 20.0f;

            matrix_identity((GLfloat *)viewMatrix);
            matrix_translate((GLfloat *)viewMatrix, (GLfloat *)cubePositions[cubePositions_i]);

            matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
            
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            glUniformMatrix4fv(viewID, 1, GL_TRUE, (const float *)viewMatrix);
            glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        printf("fov: %f\n", fov);
        fov += 0.5f;
        if(fov > 360.0f) {
            fov = 0.0f;
        }

        glfwSwapBuffers(window);
    }

    // Clean all
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    
    return (EXIT_SUCCESS);
}

void matrix_print(GLfloat *matrix) {
    int i, j;    
    printf("-------\n");
    for(j = 0; j < 4; j++) {
        for(i = 0; i < 4; i++) {
            printf("%0.2f ", (float)matrix[j * 4 + i]);
        }
        printf("\n");
    }
    printf("-------\n");    
}

void matrix_identity(GLfloat *matrix) {
    int i = 0;
    
    for(i = 0; i < 4 * 4; i++) {
        matrix[i] = 0.0f;
    }
    
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}

void matrix_translate(GLfloat *matrix, GLfloat *position) {
    matrix[0 * 4 + 3] = position[0];
    matrix[1 * 4 + 3] = position[1];
    matrix[2 * 4 + 3] = position[2];
}

void matrix_scale(float scale, GLfloat *matrix) {
    matrix[0] *= scale;
    matrix[1] *= scale;
    matrix[4] *= scale;
    matrix[5] *= scale;
}

void matrix_perspective(float angle, float ratio, float near, float far, GLfloat *matrix) {
    float tan_half_angle = tan(deg2rad(angle) / 2.0f);
    
    matrix[0 * 4 + 0] = 1.0f / (ratio * tan_half_angle);
    matrix[1 * 4 + 1] = 1.0f / tan_half_angle;
    matrix[2 * 4 + 2] = -(far + near) / (far - near);
    matrix[2 * 4 + 3] = -2 * far * near / (far - near);
    matrix[3 * 4 + 2] = -1.0f;
}

void matrix_rotate(GLfloat *matrix, float angle, float *axis) {
    angle = deg2rad(angle);

    float		sinus = sin(angle);
    float		cosinus = cos(angle);

    vector_normalize(axis);

    matrix[0 + 0] = cosinus + axis[0]*axis[0]*(1-cosinus);
    matrix[0 + 1] = axis[0]*axis[1]*(1-cosinus) - axis[2]*sinus;
    matrix[0 + 2] = axis[0]*axis[2]*(1-cosinus) + axis[1]*sinus;
    matrix[1 * 4 + 0] = axis[0]*axis[1]*(1-cosinus) + axis[2]*sinus;
    matrix[1 * 4 + 1] = cosinus + axis[1]*axis[1]*(1-cosinus);
    matrix[1 * 4 + 2] = axis[1]*axis[2]*(1-cosinus) - axis[0]*sinus;
    matrix[2 * 4 + 0] = axis[0]*axis[2]*(1-cosinus) - axis[1]*sinus;
    matrix[2 * 4 + 1] = axis[1]*axis[2]*(1-cosinus) + axis[0]*sinus;
    matrix[2 * 4 + 2] = cosinus + axis[2]*axis[2]*(1-cosinus);
}

void vector_normalize(float *vector) {
    float magnitude = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    
    if(magnitude == 0.0) {
        vector[0] = 0.0;
        vector[1] = 0.0;
        vector[2] = 0.0;
        
        return;
    }
    
    vector[0] /= magnitude;
    vector[1] /= magnitude;
    vector[2] /= magnitude;   
}