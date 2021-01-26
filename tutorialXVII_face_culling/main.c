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

#include "matrix.h"

#define true  1
#define false 0

double current_mouse_x = -1.0, current_mouse_y = -1.0;
double current_zoom_y = 0.0;

void copyArray4(GLfloat *source, GLfloat *dist);
void sortArray(int total, GLfloat distance[][4]);
void copyArrayAddDistance(int total, GLfloat distance[], const GLfloat source[][3], GLfloat temp[][4]);
void sortByDistance(int total, GLfloat distance[], const GLfloat source[][3], GLfloat sorted[][3]);

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
    // glDepthFunc(GL_ALWAYS);
    // glDepthFunc(GL_NEVER);
    // glDepthFunc(GL_LESS);
    // glDepthFunc(GL_EQUAL);
    // glDepthFunc(GL_LEQUAL);
    // glDepthFunc(GL_GREATER);
    // glDepthFunc(GL_NOTEQUAL);
    // glDepthFunc(GL_GEQUAL);
    
    // glDepthMask(GL_FALSE);
    
    // glEnable(GL_BLEND);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); // default: GL_BACK
    // glFrontFace(GL_CW); // default: GL_CCW

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    int i = 0;
    
    float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // default
    // float clearColor[] = { 0.671, 0.369, 0.204, 1.0f }; // desert
    // float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // factory
    // float clearColor[] = { 0.0, 0.0, 0.0, 1.0f }; // horror
    // float clearColor[] = { 0.9, 0.9, 0.9, 1.0f }; // biochemical lab
    
    static const GLfloat box[] = {
        // positions          // texture coords
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

    // counter-clockwise order
    float cubeCCO[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };

    static const GLfloat floor[] = {
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f
    };

    static const GLfloat grass[] = {
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    static const GLfloat cubePositions[] = {
        -1.0f, 0.0001f, -1.0f,
        2.0f, 0.0001f, 0.0f
    //     -1.5f, -2.2f, -2.5f,
    //     -3.8f, -2.0f, -12.3f,
    //      2.4f, -0.4f, -3.5f,
    //     -1.7f,  3.0f, -7.5f,
    //      1.3f, -2.0f, -2.5f,
    //      1.5f,  2.0f, -2.5f,
    //      1.5f,  0.2f, -1.5f,
    //     -1.3f,  1.0f, -1.5f,
    };    
    static const uint8_t CUBES = (sizeof(cubePositions) / sizeof(GLfloat)) / 3;

    static const GLfloat grassPositions[][3] = { 
        { -1.5f, 0.0f, -0.48f },
        { 1.5f,  0.0f,  0.51f }, 
        { 0.0f,  0.0f,  0.7f }, 
        { -0.3f,  0.0f, -2.3f }, 
        { 0.5f,  0.0f, -0.6f }, 
    };
    static const uint8_t grasses = (sizeof(grassPositions) / sizeof(GLfloat)) / 3;

    char *filename = NULL;
    uint8_t *textureData = NULL;
    imageMeta textureMeta;
        
    filename = "images/marble.png";
    printf("load %s file...\n", filename);
    textureData = stbi_load(filename, &textureMeta.x, &textureMeta.y, &textureMeta.colors, 0);
    if(!textureData) {
        printf("Failed to load %s. Reason: %s\n", filename, stbi_failure_reason());
        return EXIT_FAILURE;
    }
    GLuint containerTextureID;
    glGenTextures(1, &containerTextureID);
    printf("containerTextureID %d\n", containerTextureID);
    glBindTexture(GL_TEXTURE_2D, containerTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexImage2D(GL_TEXTURE_2D, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_BGR : GL_RGBA), GL_UNSIGNED_BYTE, textureData);
    printf("load %s width: %d, height: %d, colors: %d\n", filename, textureMeta.x, textureMeta.y, textureMeta.colors);
    stbi_image_free(textureData);

    filename = "images/metal.png";
    textureData = stbi_load(filename, &textureMeta.x, &textureMeta.y, &textureMeta.colors, 0);
    if(!textureData) {
        printf("Failed to load %s. Reason: %s\n", filename, stbi_failure_reason());
        return EXIT_FAILURE;
    }
    GLuint floorTextureID;
    glGenTextures(1, &floorTextureID);
    printf("floorTextureID %d\n", floorTextureID);
    glBindTexture(GL_TEXTURE_2D, floorTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexImage2D(GL_TEXTURE_2D, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_BGR : GL_RGBA), GL_UNSIGNED_BYTE, textureData);
    printf("load %s width: %d, height: %d, colors: %d\n", filename, textureMeta.x, textureMeta.y, textureMeta.colors);
    stbi_image_free(textureData);
    
    filename = "images/blending_transparent_window.png";
    textureData = stbi_load(filename, &textureMeta.x, &textureMeta.y, &textureMeta.colors, 0);
    if(!textureData) {
        printf("Failed to load %s. Reason: %s\n", filename, stbi_failure_reason());
        return EXIT_FAILURE;
    }
    GLuint grassTextureID;
    glGenTextures(1, &grassTextureID);
    printf("grassTextureID %d\n", grassTextureID);
    glBindTexture(GL_TEXTURE_2D, grassTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexImage2D(GL_TEXTURE_2D, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_BGR : GL_RGBA), GL_UNSIGNED_BYTE, textureData);
    printf("load %s width: %d, height: %d, colors: %d\n", filename, textureMeta.x, textureMeta.y, textureMeta.colors);
    stbi_image_free(textureData);
    
    // Init Shaders, VAO, VBO, Link it
    GLuint programId;
    programId = loadShader("shader.vs", "shader.fs");
    if(programId == -1) {
        return (EXIT_FAILURE);
    }
    
    // Create VAO
    GLuint boxVertexArrayObjectID; // VAO
    glGenVertexArrays(1, &boxVertexArrayObjectID);
    
    // Create VBO
    glBindVertexArray(boxVertexArrayObjectID);
    GLuint boxVertexBufferObjectID;
    glGenBuffers(1, &boxVertexBufferObjectID);
    glBindBuffer(GL_ARRAY_BUFFER, boxVertexBufferObjectID);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCCO), cubeCCO, GL_STATIC_DRAW);
    
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
        1, //  position vertex attribute in the vertex shader with layout (location = 1)
        2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 5, // stride - space between consecutive vertex attributes
        (void*) (sizeof(GLfloat) * 3) // array buffer offset
    );
    glEnableVertexAttribArray(1);
    
    // glVertexAttribPointer(
    //     2, //  position vertex attribute in the vertex shader with layout (location = 2)
    //     2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
    //     GL_FLOAT, // type of the data which is GL_FLOAT
    //     GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
    //     sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
    //     (void*) (sizeof(GLfloat) * 6) // array buffer offset
    // );
    // glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);  // unbind VAO
    
    GLuint floorVAO;
    glGenVertexArrays(1, &floorVAO);

    GLuint floorVBO;
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor), &floor, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0); // unbind VAO

    GLuint grassVAO;
    glGenVertexArrays(1, &grassVAO);
    GLuint grassVBO;
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grass), &grass, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0); // unbind VAO

    GLuint modelID = glGetUniformLocation(programId, "model");
    GLuint viewID = glGetUniformLocation(programId, "view");
    GLuint projectionID = glGetUniformLocation(programId, "projection");
    
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
    //char firstMouse = true;
    double last_mouse_x = -1.0, last_mouse_y = -1.0;
    
    // camera
    float front[3];
    float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    
    // Draw time
    glUseProgram(programId);

    int shaderTextureId = glGetUniformLocation(programId, "texture1");
    // printf("shaderTextureId %d\n", shaderTextureId);
    glUniform1i(shaderTextureId, 0);

    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

    // float axis[3] = { 1.0f, 0.3f, 0.5f };
    // int nbFrames = 0;
    // int nbFramesLast = glfwGetTime();
    // glfwSwapInterval(0); // Disable vsync - vsync make maximum 60 FPS (or sync with your monitor)

    while(glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //printf("currentFrame: %f\n", currentFrame);
        //printf("deltaTime: %f\n", deltaTime);

        // nbFrames++;
        // if(currentFrame - nbFramesLast >= 1.0) {
        //     printf("FPS: %d\n", nbFrames);
        //     nbFrames = 0;
        //     nbFramesLast = currentFrame;
        // }
        
        processInput(window, cameraPosition, cameraFront, cameraUp, deltaTime);


        GLfloat matrixResult[3][3] = {};
        GLfloat grassesDistance[grasses];
        for(i = 0; i < grasses; i++) {
            matrix_minus(cameraPosition, (GLfloat *)grassPositions[i], (GLfloat *)matrixResult[i]);
            grassesDistance[i] = vector_length(matrixResult[i]);
        }
        
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
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
        }
        
        glUseProgram(programId);
        
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
            glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);
        }
        
        matrix_identity((GLfloat *)viewMatrix);
        cameraPositionFront[0] = cameraPosition[0] + cameraFront[0]; 
        cameraPositionFront[1] = cameraPosition[1] + cameraFront[1]; 
        cameraPositionFront[2] = cameraPosition[2] + cameraFront[2]; 
        matrix_lootAt((GLfloat *)viewMatrix, cameraPosition, cameraPositionFront, cameraUp);
        glUniformMatrix4fv(viewID, 1, GL_TRUE, (const float *)viewMatrix);

        for(i = 0; i < CUBES * 3; i += 3) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)&cubePositions[i]);
            // float angel = 20.0f * i / 3;
            // matrix_rotate((GLfloat *)modelMatrix, angel, axis);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            
            glBindVertexArray(boxVertexArrayObjectID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, containerTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        // floor
        glBindVertexArray(floorVAO);
        glBindTexture(GL_TEXTURE_2D, floorTextureID);
        matrix_identity((GLfloat *)modelMatrix);
        glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(grassVAO);
        glBindTexture(GL_TEXTURE_2D, grassTextureID);
        GLfloat grassPositionsSorted[grasses][3];
        sortByDistance(grasses, grassesDistance, grassPositions, grassPositionsSorted);
        for(i = 0; i < grasses; i++) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)&grassPositionsSorted[i][0]);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0); // unbind
        glfwSwapBuffers(window);
    }

    // Clean all
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    return (EXIT_SUCCESS);
}

void sortByDistance(int total, GLfloat distance[], const GLfloat source[][3], GLfloat sorted[][3]) {
    int i, j;
    GLfloat temp[total][4];

    copyArrayAddDistance(total, distance, source, temp);
    sortArray(total, temp);

    for(i = 0; i < total; i++) {
        for(j = 0; j < 3; j++) {
            sorted[i][j] = temp[i][j];
        }
    }
}

void sortArray(int total, GLfloat distance[][4]) {
    int i, j;
    GLfloat swap[4];
    for(i = 0; i < total; i++) {
        for(j = 0; j < total; j++) {
            if(distance[i][3] > distance[j][3]) {
                copyArray4(distance[i], swap);
                copyArray4(distance[j], distance[i]);
                copyArray4(swap, distance[j]);
            }
        }
    }
}

void copyArray4(GLfloat *source, GLfloat *dist) {
    int i;
    for(i = 0; i < 4; i++) {
        dist[i] = source[i];
    }
}

void copyArrayAddDistance(int total, GLfloat distance[], const GLfloat source[][3], GLfloat temp[][4]) {
    int i, j;
    for(i = 0; i < total; i++) {
        for(j = 0; j < 3; j++) {
            temp[i][j] = source[i][j];
        }
        temp[i][j] = distance[i];
    }
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

