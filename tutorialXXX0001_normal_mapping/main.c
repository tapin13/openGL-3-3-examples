#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"

#include "common/image_structs.h"
// stb - image loader
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_GIF // prevent warning variable ‘delays_size’ set but not used
#include "common/stb_image.h"

#include "common/model_loader.c" // must go after stb_image included

#include "matrix.h"

#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )
#define true  1
#define false 0

double current_mouse_x = -1.0, current_mouse_y = -1.0;
double current_zoom_y = 0.0;

// model *boxModel;

void processInput(GLFWwindow *window, GLfloat *cameraPosition, GLfloat *cameraFront, GLfloat *cameraUp, float deltaTime);
void mouse_callback(GLFWwindow *window, double mouse_x, double mouse_y);
void processMouse(double *last_mouse_x, double *last_mouse_y, double current_mouse_x, double current_mouse_y, float *yaw, float *pitch);

void mouse_scroll_callback(GLFWwindow *window, double mouse_scroll_x, double mouse_scroll_y);
void processMouseScroll(double *last_mouse_x, double *last_mouse_y, double current_mouse_x, double current_mouse_y, float *yaw, float *pitch);

void renderCube();
void renderQuad();

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
    window = glfwCreateWindow(800, 600, "", NULL, NULL);
    if(window == NULL) {
        fprintf(stderr, "Failed to open GLFW windows. If you have an Intel GPU, they are not 3.3 compatible.");
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 0 - unlock vsync 1 - enable vsync
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW");
        return EXIT_FAILURE;
    }

    // Ensure we can capture Esc.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Mouse it won't be visible and it should not leave the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    
    GLuint windowWidth = 0;
    GLuint windowHeight = 0;
    glfwGetWindowSize(window, (int *)&windowWidth, (int *)&windowHeight);
    
    glViewport(0, 0, windowWidth, windowHeight);
    
    // Data

    printf("Load data...\n");

    // float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // default
    // float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // default2
    // float clearColor[] = { 0.5, 0.5, 0.5, 1.0f }; // day
    // float clearColor[] = { 0.671, 0.369, 0.204, 1.0f }; // desert
    // float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // factory
    // float clearColor[] = { 0.0, 0.0, 0.0, 1.0f }; // horror
    // float clearColor[] = { 0.9, 0.9, 0.9, 1.0f }; // biochemical lab
    
    // static const GLfloat box[] = {
    //     // positions          // normals
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    //     -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    // };

    static const GLfloat cubeVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    static const GLfloat textureCoordinates[] = {
        // 3 coords - 2 texture
        0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,      0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };


    printf("Load textures...\n");

    // GLuint containerTextureID = textureFromFile("marble.png", "images/");
    // floorTextureID = textureFromFile("marble.png", "images/");
    // GLuint brickwallTextureID = textureFromFile("wood.png", "images/");

    GLuint brickwallTextureID = textureFromFile("brickwall.jpg", "images/");
    GLuint brickwallNormalTextureID = textureFromFile("brickwall_normal.jpg", "images/");
    
    // boxModel = modelLoad("models/box/box.obj");
    // model *skullModel = modelLoad("models/skull/skull.obj");
    // model *nanosuitModel = modelLoad("models/nanosuit/nanosuit.obj");
    // model *handgunModel = modelLoad("models/handgun/handgun.obj");
    // model *planetModel = modelLoad("models/planet/planet.obj");
    // model *rockModel = modelLoad("models/rock/rock.obj");

    printf("Load shaders...\n");

    // Init Shaders, VAO, VBO, Link it

    GLuint normalShaderId;
    normalShaderId = loadShader("shaders/normal.vs", "shaders/normal.fs");
    if(normalShaderId == -1) {
        return (EXIT_FAILURE);
    }
    glUseProgram(normalShaderId);

    GLuint simpleShaderId;
    simpleShaderId = loadShader("shaders/simple.vs", "shaders/simple.fs");
    if(simpleShaderId == -1) {
        return (EXIT_FAILURE);
    }

    // Create VAO
    GLuint vertexArrayObjectID; // VAO
    glGenVertexArrays(1, &vertexArrayObjectID);
    glBindVertexArray(vertexArrayObjectID);

    // Create VBO
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
    
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
    
    // glVertexAttribPointer(
    //     1, //  position vertex attribute in the vertex shader with layout (location = 0)
    //     3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
    //     GL_FLOAT, // type of the data which is GL_FLOAT
    //     GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
    //     sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
    //     (void*) (sizeof(GLfloat) * 3) // array buffer offset
    // );
    // glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(
        1, //  position vertex attribute in the vertex shader with layout (location = 0)
        2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 5, // stride - space between consecutive vertex attributes
        (void*) (sizeof(GLfloat) * 3) // array buffer offset
    );
    glEnableVertexAttribArray(1);

    // Create EBO
    GLuint vertexEBOId;
    glGenBuffers(1, &vertexEBOId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexEBOId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);  // unbind VAO

    // setup cube shader start

    GLuint lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    
    GLuint lightCubeVBO;
    glGenBuffers(1, &lightCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);  // unbind VAO

    // setup cube shader end

    GLfloat modelMatrix[4][4] = {};
    GLfloat viewMatrix[4][4] = {};
    GLfloat projectionMatrix[4][4] = {};
    
    matrix_identity((GLfloat *)modelMatrix);
    matrix_identity((GLfloat *)viewMatrix);
    matrix_identity((GLfloat *)projectionMatrix);

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
    double last_mouse_x = -1.0, last_mouse_y = -1.0;
    
    // camera
    float front[3];
    float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    
    // configure shader

    glUseProgram(normalShaderId);
    GLuint normalProjectionID = glGetUniformLocation(normalShaderId, "projection");
    GLuint normalViewID = glGetUniformLocation(normalShaderId, "view");
    GLuint normalModelID = glGetUniformLocation(normalShaderId, "model");
    GLuint normalLightPositionID = glGetUniformLocation(normalShaderId, "lightPosition");
    GLuint normalLightColorID = glGetUniformLocation(normalShaderId, "lightColor");
    GLuint normalViewPositionID = glGetUniformLocation(normalShaderId, "viewPosition");
    GLuint normalDiffuseMapId = glGetUniformLocation(normalShaderId, "diffuseMap");
    GLuint normalNormalMapId = glGetUniformLocation(normalShaderId, "normalMap");

    glUseProgram(simpleShaderId);
    GLuint simpleProjectionID = glGetUniformLocation(simpleShaderId, "projection");
    GLuint simpleViewID = glGetUniformLocation(simpleShaderId, "view");
    GLuint simpleModelID = glGetUniformLocation(simpleShaderId, "model");

    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);

    glUseProgram(normalShaderId);
    glUniformMatrix4fv(normalProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);
    glUseProgram(simpleShaderId);
    glUniformMatrix4fv(simpleProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

    float lightCubeScale[] = { 0.1f, 0.1f, 0.1f };
    GLfloat lightPosition[] = { 0.0f, 0.2f, 0.5f };
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f };
    float lightPositionStep = 1.0;
    float lightPositionMove = lightPositionStep;
    float update_interval = 0.1;
    double last_update_time;

    while(glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //printf("deltaTime: %f\n", deltaTime);
        
        processInput(window, cameraPosition, cameraFront, cameraUp, deltaTime);
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        if(last_mouse_x != current_mouse_x || last_mouse_y != current_mouse_y) {
            processMouse(&last_mouse_x, &last_mouse_y, current_mouse_x, current_mouse_y, &yaw, &pitch);
            
            front[0] = cos(deg2rad(pitch)) * cos(deg2rad(yaw));
            front[1] = sin(deg2rad(pitch));
            front[2] = cos(deg2rad(pitch)) * sin(deg2rad(yaw));
            vector_normalize(front);

            cameraFront[0] = front[0];
            cameraFront[1] = front[1];
            cameraFront[2] = front[2];
        }
        
        if(current_zoom_y != 0.0) {
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
            
            matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
            glUseProgram(normalShaderId);
            glUniformMatrix4fv(normalProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);
            glUseProgram(simpleShaderId);
            glUniformMatrix4fv(simpleProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);
        }

        if(lightPosition[0] >= 1.5) {
            lightPositionMove = -lightPositionStep;
        } else if(lightPosition[0] <= -1.5) {
            lightPositionMove = lightPositionStep;
        }
        lightPosition[0] += lightPositionMove * deltaTime;

        matrix_identity((GLfloat *)viewMatrix);
        vector_minus(cameraPositionFront, cameraPosition, cameraFront);
        matrix_lootAt((GLfloat *)viewMatrix, cameraPosition, cameraPositionFront, cameraUp);

        glUseProgram(normalShaderId);
        glUniformMatrix4fv(normalViewID, 1, GL_TRUE, (const float *)viewMatrix);
        glUniform3fv(normalViewPositionID, 1, cameraPosition);

        matrix_identity((GLfloat *)modelMatrix);
        glUniformMatrix4fv(normalModelID, 1, GL_TRUE, (const float *)modelMatrix);
        glUniform3fv(normalLightPositionID, 1, lightPosition);
        glUniform3fv(normalLightColorID, 1, lightColor);
        
        glBindVertexArray(vertexArrayObjectID);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(normalDiffuseMapId, 0);
        glBindTexture(GL_TEXTURE_2D, brickwallTextureID);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(normalNormalMapId, 1);
        glBindTexture(GL_TEXTURE_2D, brickwallNormalTextureID);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);   
        glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
    
        // Light cube start

        glUseProgram(simpleShaderId);

        glUniformMatrix4fv(simpleViewID, 1, GL_TRUE, (const float *)viewMatrix);

        matrix_identity((GLfloat *)modelMatrix);
        matrix_scale((GLfloat *)modelMatrix, lightCubeScale);
        matrix_translate((GLfloat *)modelMatrix, lightPosition);
        glUniformMatrix4fv(simpleModelID, 1, GL_TRUE, (const float *)modelMatrix);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Light cube end

	    glBindVertexArray(0); // unbind
        glfwSwapBuffers(window);
    }

    // Clean all
    
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vertexArrayObjectID);
    glDeleteVertexArrays(1, &lightCubeVAO);

    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &lightCubeVBO);
    glDeleteBuffers(1, &vertexEBOId);
    
    return (EXIT_SUCCESS);
}

void renderQuad() {
    unsigned int quadVAO = 0;
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glBindVertexArray(quadVAO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

