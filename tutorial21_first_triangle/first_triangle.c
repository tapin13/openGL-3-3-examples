/* 
 * File:   first_triangle.c
 * Author: tapin13
 *
 * Created on May 7, 2017, 1:27 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "common/loadShader.c"

#define true  1
#define false 0

static void Matrix4Perspective(float *M, float fovy, float aspect, float znear, float zfar)
{
	float f = 1 / tan(fovy / 2),
	      A = (zfar + znear) / (znear - zfar),
	      B = (2 * zfar * znear) / (znear - zfar);

	M[ 0] = f / aspect; M[ 1] =  0; M[ 2] =  0; M[ 3] =  0;
	M[ 4] = 0;          M[ 5] =  f; M[ 6] =  0; M[ 7] =  0;
	M[ 8] = 0;          M[ 9] =  0; M[10] =  A; M[11] =  B;
	M[12] = 0;          M[13] =  0; M[14] = -1; M[15] =  0;
        /*
	M[ 0] = 1;          M[ 1] =  0; M[ 2] =  0; M[ 3] =  0;
	M[ 4] = 0;          M[ 5] =  1; M[ 6] =  0; M[ 7] =  0;
	M[ 8] = 0;          M[ 9] =  0; M[10] =  1; M[11] =  1;
	M[12] = 0;          M[13] =  0; M[14] = 0; M[15] =  1;
        */
}

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
    window = glfwCreateWindow(640, 480, "Tutorial 01", NULL, NULL);
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

    GLuint programId = loadShader("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    glUseProgram(programId);

    
    float projectionMatrix[16];

    int width, height;
    glfwGetWindowSize(window, &width, &height);    
    
    const float aspectRatio = (float)width / (float)height;
    
    Matrix4Perspective(projectionMatrix, 45.0f, aspectRatio, 1.5f, 5.0f);
    
    int i;
    for(i = 0; i < 16; i++) {
        fprintf(stdout, "projectionMatrix %d: %f\n", i, projectionMatrix[i]);
    }
    
    GLint projectionMatrixLocation;
    
    projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
    if(projectionMatrixLocation == -1) {
        fprintf(stderr, "glGetUniformLocation projectionMatrixLocation FAILED\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "projectionMatrixLocation %d\n", projectionMatrixLocation);
    glUniformMatrix4fv(projectionMatrixLocation // position(location)
            , 1 // count
            , GL_TRUE // transpose
            , projectionMatrix
    );
    
    const int vertexCount = 3;
    const int vertexPoints = 3; //  x-y-z
    const int colorValues = 3;  //  R-G-B
    const int vertexSize = (vertexPoints + colorValues) * sizeof(GLfloat);
    
    //static const GLfloat triangleMesh[] = {
    static const GLfloat g_vertex_buffer_data[] = {
        /* 1 вершина, позиция: */ -1.0f, -1.0f, -2.0f, /* цвет: */ 1.0f, 0.0f, 0.0f,
	/* 2 вершина, позиция: */  0.0f,  1.0f, -2.0f, /* цвет: */ 0.0f, 1.0f, 0.0f,
        /* 3 вершина, позиция: */ 1.0f, -1.0f, -2.0f, /* цвет: */ 0.0f, 0.0f, 1.0f
    };

//    static const GLfloat g_vertex_buffer_data[] = {
//        -1.0f, -0.8f, 0.0f,     1.0f, 0.8f, 1.0f,
//        1.0f, -1.0f, 0.0f,      0.0f, 0.8f, 0.0f,
//        0.0f,  1.0f, 0.0f,      1.0f, 0.8f, 0.0f
//    };
    
    GLuint meshVAO, meshVBO;
    
    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    const int vertexOffsetPosition = 0; // first value is Position 1.0, 0.0, 0.0, etc // shift from array START
    const int vertexOffsetColor = 3 * sizeof(float); // first 3 is Position 1.0, 0.0, 0.0, then color value // shift from array START

    GLint positionLocation, colorLocation;
    
    positionLocation = glGetAttribLocation(programId, "position");
    if (positionLocation == -1) {
        fprintf(stderr, "glGetAttribLocation positionLocation FAILED\n");
        return EXIT_FAILURE;
    }    
    fprintf(stdout, "positionLocation %d\n", positionLocation);
    glVertexAttribPointer(
            positionLocation,
            vertexPoints,
            GL_FLOAT,
            GL_FALSE,
            vertexSize,
            (const GLvoid *) 0 //(const GLvoid *)vertexOffsetPosition // start offset in array
    );
    glEnableVertexAttribArray(positionLocation);
    
    colorLocation = glGetAttribLocation(programId, "color");
    if (colorLocation == -1) {
        fprintf(stderr, "glGetAttribLocation colorLocation FAILED\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "colorLocation %d\n", colorLocation);
    glVertexAttribPointer(colorLocation, colorValues, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid *)(3 * sizeof(float)));
    glEnableVertexAttribArray(colorLocation);
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        // draw here!
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        //glClearDepth(1.0f);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);// start from 0. total 3 points.
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
        
    glDisableVertexAttribArray(0);
    
    return (EXIT_SUCCESS);
}

