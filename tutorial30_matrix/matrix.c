/* 
 * Author: tapin13
 *
 * Created on May 8, 2017, 1:27 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"
#include "common/matrixMath.c"
#include "common/tgaLoader.c"

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
    window = glfwCreateWindow(640, 480, "Tutorial 03", NULL, NULL);
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
    
    int width, height;
    glfwGetWindowSize(window, &width, &height);    
    
    const float aspectRatio = (float)width / (float)height;

    glfwSwapInterval(1);

    /*************************************************************************/
    
    static float cubeRotation[3] = { 0.0f, 0.0f, 0.0f };
    
    GLuint programId = loadShader("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
    glUseProgram(programId);
    
    Matrix4 viewMatrix = {0.0f}, projectionMatrix = {0.0f}, viewProjectionMatrix = {0.0f};
    Matrix4Perspective(projectionMatrix, 45.0f, aspectRatio, 1.0f, 10.0f);
    
    Matrix4Translation(viewMatrix, 0.0f, 0.0f, -4.0f);
    
    Matrix4Mul(viewProjectionMatrix, projectionMatrix, viewMatrix);
    
    Matrix4 modelMatrix = {0.0f}, modelViewProjectionMatrix = {0.0f};
    
    static const unsigned int vertexPoints = 3;
    static const unsigned int cubeVerticesCount = 24;
    
    const float s = 1.0f;
    
    const float cubePositions[][3] = { // [cubeVerticesCount][vertexPoints]
        {-s, s, s}, { s, s, s}, { s,-s, s}, {-s,-s, s}, // front
	{ s, s,-s}, {-s, s,-s}, {-s,-s,-s}, { s,-s,-s}, // back
	{-s, s,-s}, { s, s,-s}, { s, s, s}, {-s, s, s}, // top
	{ s,-s,-s}, {-s,-s,-s}, {-s,-s, s}, { s,-s, s}, // bottom
	{-s, s,-s}, {-s, s, s}, {-s,-s, s}, {-s,-s,-s}, // left
	{ s, s, s}, { s, s,-s}, { s,-s,-s}, { s,-s, s}  // right
    };

    const float cubeTexcoords[][2] = {
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // front
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // back
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // top
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // bottom
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // left
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}  // right
    };
    
    const unsigned int cubeIndicesCount = 36;
    
    const unsigned int cubeIndeces[] = { // [cubeIndicesCount]
	 0, 3, 1,  1, 3, 2, // front
	 4, 7, 5,  5, 7, 6, // back
	 8,11, 9,  9,11,10, // top
	12,15,13, 13,15,14, // bottom
	16,19,17, 17,19,18, // left
	20,23,21, 21,23,22  // right      
    };
    
    GLuint cubeVBO[3], cubeVAO;
    
    GLuint positionLocation;
    GLuint texcoordLocation;
    
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    
    glGenBuffers(3, cubeVBO);
    
    
    positionLocation = glGetAttribLocation(programId, "position");
    if(positionLocation == -1) {
        fprintf(stderr, "positionLocation error\n");
        return EXIT_FAILURE;
    }

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (vertexPoints * sizeof(float)), cubePositions, GL_STATIC_DRAW);
    glVertexAttribPointer(positionLocation, vertexPoints, GL_FLOAT, GL_FALSE, vertexPoints * sizeof(float), 0);
    glEnableVertexAttribArray(positionLocation);
    
    texcoordLocation = glGetAttribLocation(programId, "texcoord");
    if(texcoordLocation == -1) {
        fprintf(stderr, "texcoordLocation error\n");
        return EXIT_FAILURE;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (2 * sizeof(float)), cubeTexcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoordLocation);
    
    GLuint colorTexture;
    colorTexture = TextureCreateFromTGA("data/texture.tga");
    if(!colorTexture) {
        return EXIT_FAILURE;
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    
    GLint textureLocation;
    
    textureLocation = glGetUniformLocation(programId, "colorTexture");
    if(textureLocation == -1) {
        fprintf(stderr, "textureLocation error\n");
        return EXIT_FAILURE;
    }
    
    glUniform1i(textureLocation, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesCount * sizeof(unsigned int), cubeIndeces, GL_STATIC_DRAW);
    
    GLint matrixLocation;
    matrixLocation = glGetUniformLocation(programId, "modelViewProjectionMatrix");
    glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, modelViewProjectionMatrix);

    /*************************************************************************/
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    uint32_t beginTime = (unsigned int)tp.tv_sec;
        
    /***/
    
    static double limitFPS = 1.0 / 60.0;
    double lastTime = glfwGetTime();
    double timer = lastTime;
    double deltaTime = 0.0;
    double nowTime = 0.0;
    unsigned int frames = 0;
    unsigned int updates = 0;
    
    /***/
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        // draw here!
        
        //clock_gettime(CLOCK_REALTIME, &tp);
//        if((unsigned int)tp.tv_sec <= beginTime) {
//            continue;
//        }
//        beginTime = (unsigned int)tp.tv_sec;
//        printf("sec: %u\n", (unsigned int)tp.tv_sec);
//        printf("nsec: %lu\n", (unsigned long)tp.tv_nsec);
        
        
        nowTime = glfwGetTime();
        deltaTime += (nowTime - lastTime) / limitFPS;
        lastTime = nowTime;
        
        //printf("glfwGetTime: %f\n", glfwGetTime());
        
        //printf("deltaTime: %f\n", deltaTime);
        
        while(deltaTime >= 1.0) {
            //printf("while deltaTime: %f\n", deltaTime);
            // UPDATE Start
            if ((cubeRotation[0] += 0.01f) > 360.0f) {
                cubeRotation[0] -= 360.0f;
            }
            //printf("cubeRotation[0]: %f\n", cubeRotation[0]);

            if ((cubeRotation[1] += 0.011f) > 360.0f) {
                cubeRotation[1] -= 360.0f;
            }

            if ((cubeRotation[2] += 0.012f) > 360.0f) {
                cubeRotation[2] -= 360.0f;
            }

            Matrix4Rotation(modelMatrix, cubeRotation[0], cubeRotation[1], cubeRotation[2]);
            Matrix4Mul(modelViewProjectionMatrix, viewProjectionMatrix, modelMatrix);
            
            // UPDATE End
        
            updates++;
            deltaTime -= 1.0;
        }

        // RENDER Start
        // glDrawArrays ...
        glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, modelViewProjectionMatrix);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        // RENDER End

        frames++;
        
        if(glfwGetTime() - timer > 1.0) {
            timer++;
            printf("FPS: %u, Updates: %u\n", frames, updates);
            updates = 0;
            frames = 0;
        }
        
        glfwPollEvents();    
    }
    
    glDisableVertexAttribArray(0);
    
    /*************************************************************************/
    
    glfwDestroyWindow(window);
    glfwTerminate();    
    
    return (EXIT_SUCCESS);
}
