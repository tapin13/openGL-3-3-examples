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

double current_mouse_x = -1.0, current_mouse_y = -1.0;
double current_zoom_y = 0.0;

void processInput(GLFWwindow *window, GLfloat *cameraPosition, GLfloat *cameraFront, GLfloat *cameraUp, float deltaTime);
void mouse_callback(GLFWwindow *window, double mouse_x, double mouse_y);
void processMouse(double *last_mouse_x, double *last_mouse_y, double current_mouse_x, double current_mouse_y, float *yaw, float *pitch);

void mouse_scroll_callback(GLFWwindow *window, double mouse_scroll_x, double mouse_scroll_y);
void processMouseScroll(double *last_mouse_x, double *last_mouse_y, double current_mouse_x, double current_mouse_y, float *yaw, float *pitch);

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
    
    // Mouse it won't be visible and it should not leave the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    
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
    GLfloat cameraFront[3] = { 0.0f, 0.0f, -1.0f };
    
    GLfloat cameraUp[3] = { 0.0f, 1.0f, 0.0f };
    GLfloat cameraPositionFront[3] = { 
        cameraPosition[0] + cameraFront[0], 
        cameraPosition[1] + cameraFront[1], 
        cameraPosition[2] + cameraFront[2], 
    };
 
    // mouse
    //char firstMouse = true;
    double last_mouse_x = -1.0, last_mouse_y = -1.0;
    
    // camera
    float front[3];
    float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    
    // Draw time
    
    glUseProgram(programId);
    glUniform1i(glGetUniformLocation(programId, "Texture1"), 0); // once
    glUniform1i(glGetUniformLocation(programId, "Texture2"), 1); // once
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID_1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID_2);
    glBindVertexArray(vertexArrayObjectID);

    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    int cubePositions_i = 0;
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;
    
    while(glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //printf("deltaTime: %f\n", deltaTime);
        
        processInput(window, cameraPosition, cameraFront, cameraUp, deltaTime);
        
        glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(last_mouse_x != current_mouse_x || last_mouse_y != current_mouse_y) {
            processMouse(&last_mouse_x, &last_mouse_y, current_mouse_x, current_mouse_y, &yaw, &pitch);
            
            front[0] = cos(deg2rad(pitch)) * cos(deg2rad(yaw));
            front[1] = sin(deg2rad(pitch));
            front[2] = cos(deg2rad(pitch)) * sin(deg2rad(yaw));
            vector_normalize(front);

            cameraFront[0] = front[0];
            cameraFront[1] = front[1];
            cameraFront[2] = front[2];

            //printf("cameraFront: (%f, %f, %f)\n", cameraFront[0], cameraFront[1], cameraFront[2]);
        }
        
        if(current_zoom_y != 0.0) {
            printf("current_zoom_y: %f\n", current_zoom_y);
            
            if(fov > 0.0f && fov < 56.0f) {
                fov -= current_zoom_y;
            }
            if(fov <= 1.0f) {
                fov = 1.0f;
            }
            if(fov >= 56.0f) {
                fov = 55.0f;
            }
            current_zoom_y = 0.0f;
            
            printf("fov: %f\n", fov);
            
            matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
            glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);
        }
        
        matrix_identity((GLfloat *)viewMatrix);
        cameraPositionFront[0] = cameraPosition[0] + cameraFront[0]; 
        cameraPositionFront[1] = cameraPosition[1] + cameraFront[1]; 
        cameraPositionFront[2] = cameraPosition[2] + cameraFront[2]; 
        matrix_lootAt((GLfloat *)viewMatrix, cameraPosition, cameraPositionFront, cameraUp);
        glUniformMatrix4fv(viewID, 1, GL_TRUE, (const float *)viewMatrix);

        for(cubePositions_i = 0; cubePositions_i < 10; cubePositions_i++) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_rotate(*modelMatrix, 10.0 * cubePositions_i, axes);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)cubePositions[cubePositions_i]);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window);
    }

    // Clean all
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    
    return (EXIT_SUCCESS);
}

void processInput(GLFWwindow *window, GLfloat *cameraPosition, GLfloat *cameraFront, GLfloat *cameraUp, float deltaTime) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    
    float cameraSpeed = 2.5f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPosition[0] += cameraSpeed * cameraFront[0];
        cameraPosition[1] += cameraSpeed * cameraFront[1];
        cameraPosition[2] += cameraSpeed * cameraFront[2];
        
        return;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPosition[0] -= cameraSpeed * cameraFront[0];
        cameraPosition[1] -= cameraSpeed * cameraFront[1];
        cameraPosition[2] -= cameraSpeed * cameraFront[2];
        
        return;
    }

    GLfloat normalize_cross_front_up[3];

    normalize_cross_front_up[0] = cameraFront[0];
    normalize_cross_front_up[1] = cameraFront[1];
    normalize_cross_front_up[2] = cameraFront[2];
    vector_cross(normalize_cross_front_up, cameraUp);
    vector_normalize(normalize_cross_front_up);
    
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPosition[0] -= cameraSpeed * normalize_cross_front_up[0];
        cameraPosition[1] -= cameraSpeed * normalize_cross_front_up[1];
        cameraPosition[2] -= cameraSpeed * normalize_cross_front_up[2];
        
        return;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPosition[0] += cameraSpeed * normalize_cross_front_up[0];
        cameraPosition[1] += cameraSpeed * normalize_cross_front_up[1];
        cameraPosition[2] += cameraSpeed * normalize_cross_front_up[2];
        
        return;
    }
}

void mouse_callback(GLFWwindow *window, double mouse_x, double mouse_y) {
    //printf("mouse: (%f, %f)\n", mouse_x, mouse_y);
    
    current_mouse_x = mouse_x;
    current_mouse_y = mouse_y;
}

void processMouse(double *last_mouse_x, double *last_mouse_y, double current_mouse_x, double current_mouse_y, float *yaw, float *pitch) {
    if(*last_mouse_x == -1.0 || *last_mouse_y == -1.0) {
        *last_mouse_x = current_mouse_x;
        *last_mouse_y = current_mouse_y;
        //printf("last: (%f, %f)\n", lastX, lastY);
    }
    
    float xoffset = current_mouse_x - *last_mouse_x;
    float yoffset = *last_mouse_y - current_mouse_y;
    *last_mouse_x = current_mouse_x;
    *last_mouse_y = current_mouse_y;
    
    //printf("offset: (%f, %f)\n", xoffset, yoffset);
    
    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    *yaw += xoffset;
    *pitch += yoffset;

    //printf("pitch, yaw: (%f, %f)\n", pitch, yaw);
    
    if(*pitch > 89.0f) {
        *pitch = 89.0f;
    }
    if(*pitch < -89.0f) {
        *pitch = -89.0f;
    }
}

void mouse_scroll_callback(GLFWwindow *window, double mouse_scroll_x, double mouse_scroll_y) {
    //printf("mouse scroll: (%f, %f)\n", mouse_scroll_x, mouse_scroll_y);
    
    current_zoom_y = mouse_scroll_y;
}

