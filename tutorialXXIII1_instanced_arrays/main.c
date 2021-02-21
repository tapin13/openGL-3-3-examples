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
    glfwSwapInterval(0); // unlock vsync
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW");
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);  
    // glEnable(GL_CULL_FACE);  

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
    
    // Data

    int i = 0;

    printf("Load data...\n");

    float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // default
    // float clearColor[] = { 0.5, 0.5, 0.5, 1.0f }; // day
    // float clearColor[] = { 0.671, 0.369, 0.204, 1.0f }; // desert
    // float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // factory
    // float clearColor[] = { 0.0, 0.0, 0.0, 1.0f }; // horror
    // float clearColor[] = { 0.9, 0.9, 0.9, 1.0f }; // biochemical lab
    
    static const GLfloat box[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    static const GLfloat floor[] = {
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f
    };

    static const GLfloat cubemapVertices[] = {
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

    // static const uint8_t CUBES = 1;
    static const GLfloat cubePositions[] = {
        0.0f,  0.001f,  0.0f,
    //     2.0f,  5.0f, -15.0f,
    //     -1.5f, -2.2f, -2.5f,
    //     -3.8f, -2.0f, -12.3f,
    //      2.4f, -0.4f, -3.5f,
    //     -1.7f,  3.0f, -7.5f,
    //      1.3f, -2.0f, -2.5f,
    //      1.5f,  2.0f, -2.5f,
    //      1.5f,  0.2f, -1.5f,
    //     -1.3f,  1.0f, -1.5f,
    };    

    printf("Load textures...\n");

    uint8_t *textureData = NULL;
    imageMeta textureMeta;

    GLuint cubemapTextureID;
    glGenTextures(1, &cubemapTextureID);
    printf("cubemapTextureID %d\n", cubemapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
    char *textures_faces[] = {
        "images/skybox/right.jpg",
        "images/skybox/left.jpg",
        "images/skybox/top.jpg",
        "images/skybox/bottom.jpg",
        "images/skybox/front.jpg",
        "images/skybox/back.jpg",
    };
    // char *textures_faces[] = {
    //     "images/city/right.jpg",
    //     "images/city/left.jpg",
    //     "images/city/top.jpg",
    //     "images/city/bottom.jpg",
    //     "images/city/front.jpg",
    //     "images/city/back.jpg",
    // };
    for(i = 0; i < sizeof(textures_faces) / sizeof(textures_faces[0]); i++) {
        printf("load %s file...\n", textures_faces[i]);
        textureData = stbi_load(textures_faces[i], &textureMeta.x, &textureMeta.y, &textureMeta.colors, 0);
        if(!textureData) {
            printf("Failed to load %s. Reason: %s\n", textures_faces[i], stbi_failure_reason());
            return EXIT_FAILURE;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, textureData);
        stbi_image_free(textureData);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);    

    // GLuint containerTextureID = textureFromFile("marble.png", "images/");
    GLuint floorTextureID = textureFromFile("metal.png", "images/");
    
    model *boxModel = modelLoad("models/box/box.obj");
    // model *skullModel = modelLoad("models/skull/skull.obj");
    model *nanosuitModel = modelLoad("models/nanosuit/nanosuit.obj");
    model *handgunModel = modelLoad("models/handgun/handgun.obj");

    printf("Load shaders...\n");

    // Init Shaders, VAO, VBO, Link it
    GLuint programId;
    programId = loadShader("shaders/shader.vs", "shaders/shader.fs");
    // programId = loadShader("shaders/reflection_shader.vs", "shaders/reflection_shader.fs");
    if(programId == -1) {
        return (EXIT_FAILURE);
    }

    GLuint modelShaderId;
    modelShaderId = loadShader("shaders/model_shader.vs", "shaders/model_shader.fs");
    if(modelShaderId == -1) {
        return (EXIT_FAILURE);
    }

    GLuint modelInstancedArrayShaderId;
    modelInstancedArrayShaderId = loadShader("shaders/model_instanced_array.vs", "shaders/model_instanced_array.fs");
    if(modelInstancedArrayShaderId == -1) {
        return (EXIT_FAILURE);
    }

    GLuint cubemapShaderId;
    cubemapShaderId = loadShader("shaders/cubemap_shader.vs", "shaders/cubemap_shader.fs");
    if(cubemapShaderId == -1) {
        return (EXIT_FAILURE);
    }

    printf("Load VAO, VBO, EBO...\n");

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
        sizeof(GLfloat) * 6, // stride - space between consecutive vertex attributes
        (void*)0 // array buffer offset
    );
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(
        1, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 6, // stride - space between consecutive vertex attributes
        (void*) (sizeof(GLfloat) * 3) // array buffer offset
    );
    glEnableVertexAttribArray(1);
    
    // glVertexAttribPointer(
    //     2, //  position vertex attribute in the vertex shader with layout (location = 0)
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

    GLuint cubemapVAO;
    glGenVertexArrays(1, &cubemapVAO);

    GLuint cubemapVBO;
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glBindVertexArray(0); // unbind VAO    

    // 
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
    
    // configure shader
    int shaderTextureId;

    glUseProgram(cubemapShaderId);
    GLuint cubemapViewID = glGetUniformLocation(cubemapShaderId, "view");
    GLuint cubemapProjectionID = glGetUniformLocation(cubemapShaderId, "projection");
    shaderTextureId = glGetUniformLocation(cubemapShaderId, "cubemap");
    // printf("shaderTextureId %d\n", shaderTextureId);
    if(cubemapViewID == -1 || cubemapProjectionID == -1 || shaderTextureId == -1) {
        printf("Failed to locate uniform variable location cubemapShaderId\n");
        return (EXIT_FAILURE);
    }
    glUniform1i(shaderTextureId, 0); // send Active Texture Unit ID.

    glUseProgram(programId);
    GLuint modelID = glGetUniformLocation(programId, "model");
    GLuint viewID = glGetUniformLocation(programId, "view");
    GLuint projectionID = glGetUniformLocation(programId, "projection");
    // GLuint cameraPositionID = glGetUniformLocation(programId, "cameraPos");
    // shaderTextureId = glGetUniformLocation(programId, "cubemap");
    // printf("shaderTextureId %d\n", shaderTextureId);
    // if(modelID == -1 || viewID == -1 || projectionID == -1 || cameraPositionID == -1 || shaderTextureId == -1) {
    if(modelID == -1 || viewID == -1 || projectionID == -1) {
        printf("Failed to locate uniform variable location programId\n");
        return (EXIT_FAILURE);

    }

    glUseProgram(modelShaderId);
    GLuint modelModelID = glGetUniformLocation(modelShaderId, "model");
    GLuint modelViewID = glGetUniformLocation(modelShaderId, "view");
    GLuint modelProjectionID = glGetUniformLocation(modelShaderId, "projection");
    // GLuint cameraPositionID = glGetUniformLocation(modelShaderId, "cameraPos");
    // shaderTextureId = glGetUniformLocation(modelShaderId, "cubemap");
    // printf("shaderTextureId %d\n", shaderTextureId);
    if(modelModelID == -1 || modelViewID == -1 || modelProjectionID == -1) {
        printf("Failed to locate uniform variable location modelShaderId\n");
        return (EXIT_FAILURE);
    }

    glUseProgram(modelInstancedArrayShaderId);
    GLuint modelInstancingModelID = glGetUniformLocation(modelInstancedArrayShaderId, "model");
    GLuint modelInstancingViewID = glGetUniformLocation(modelInstancedArrayShaderId, "view");
    GLuint modelInstancingProjectionID = glGetUniformLocation(modelInstancedArrayShaderId, "projection");
    // GLuint cameraPositionID = glGetUniformLocation(modelShaderId, "cameraPos");
    // shaderTextureId = glGetUniformLocation(modelShaderId, "cubemap");
    // printf("shaderTextureId %d\n", shaderTextureId);
    if(modelInstancingModelID == -1 || modelInstancingViewID == -1 || modelInstancingProjectionID == -1) {
        printf("Failed to locate uniform variable location modelInstancedArrayShaderId\n");
        return (EXIT_FAILURE);
    }

    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

    int nbFrames = 0;
    float currentTime;
    float lastTime = glfwGetTime();

    // float axis[3] = { 1.0f, 0.3f, 0.5f };
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    #define cubeTotal 25
    GLfloat offsets[cubeTotal * cubeTotal * cubeTotal][3] = { };
    for (i = 0; i < cubeTotal; i++) {
        for (int j = 0; j < cubeTotal; j++) {
            for (int k = 0; k < cubeTotal; k++) {
                offsets[i * cubeTotal * cubeTotal + j * cubeTotal + k][0] = -5.0f + j * 1.3f;
                offsets[i * cubeTotal * cubeTotal + j * cubeTotal + k][1] = 0.0f + i * 1.3f;
                offsets[i * cubeTotal * cubeTotal + j * cubeTotal + k][2] = -5.0f - k * 1.3f;        
            }
        }
    }
    glUseProgram(modelInstancedArrayShaderId);
    u_int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeTotal * cubeTotal * cubeTotal * 3 * sizeof(GLfloat), offsets, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind

    modelInstanceSetup(boxModel, instanceVBO);

    while(glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        currentFrame = glfwGetTime();
        currentTime = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        nbFrames++;
        if(currentTime - lastTime >= 1.0){ // If last prinf() was more than 1 sec ago
            printf("cubeTotal: %d - %d ms/frame\n", cubeTotal * cubeTotal * cubeTotal, nbFrames);
            nbFrames = 0;
            lastTime = glfwGetTime();

            // printf("currentFrame: %f\n", currentFrame);
            printf("deltaTime: %f\n", deltaTime * 1000);
        }        
        
        processInput(window, cameraPosition, cameraFront, cameraUp, deltaTime);
        
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
        }

        matrix_identity((GLfloat *)viewMatrix);
        vector_minus(cameraPositionFront, cameraPosition, cameraFront);
        matrix_lootAt((GLfloat *)viewMatrix, cameraPosition, cameraPositionFront, cameraUp);
        
        glUseProgram(programId);
        glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);
        glUniformMatrix4fv(viewID, 1, GL_TRUE, (const float *)viewMatrix);
        // glUniform3fv(cameraPositionID, 1, cameraPosition);
        // for(i = 0; i < CUBES * 3; i += 3) {
        // // i = 0;
        //     matrix_identity((GLfloat *)modelMatrix);
        //     matrix_translate((GLfloat *)modelMatrix, (GLfloat *)&cubePositions[i]);
        //     float angel = 20.0f * i / 3;
        //     matrix_rotate((GLfloat *)modelMatrix, angel, axis);
        //     glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            
        //     glBindVertexArray(vertexArrayObjectID);
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        //     glBindVertexArray(0); // unbind
        // }

        // floor
        matrix_identity((GLfloat *)modelMatrix);
        glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0); // unbind texture

        // enable shader before setting uniforms
        glUseProgram(modelShaderId);
        glUniformMatrix4fv(modelProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);
        glUniformMatrix4fv(modelViewID, 1, GL_TRUE, (const float *)viewMatrix);
        matrix_identity((GLfloat *)modelMatrix);
        glUniformMatrix4fv(modelModelID, 1, GL_TRUE, (const float *)modelMatrix);
        // modelDraw(modelShaderId, boxModel);
        // modelDraw(modelShaderId, cylinderModel);
        // modelDraw(modelShaderId, skullModel);
        // modelDraw(modelShaderId, sphereModel);
        modelDraw(modelShaderId, nanosuitModel, NULL);
        // modelDraw(modelShaderId, handgunModel);
        // modelDraw(modelShaderId, skull3Model);

        // for (i = 0; i < cubeTotal; i++) {
            glUseProgram(modelInstancedArrayShaderId);
            glUniformMatrix4fv(modelInstancingProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);
            glUniformMatrix4fv(modelInstancingViewID, 1, GL_TRUE, (const float *)viewMatrix);
            matrix_identity((GLfloat *)modelMatrix);
            glUniformMatrix4fv(modelInstancingModelID, 1, GL_TRUE, (const float *)modelMatrix);
            modelDraw(modelInstancedArrayShaderId, boxModel, cubeTotal * cubeTotal * cubeTotal);
            // modelDraw(modelShaderId, handgunModel);
        // }

        glUseProgram(cubemapShaderId);
        glUniformMatrix4fv(cubemapProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);

        GLfloat viewTransformationTemp[3] = {
            viewMatrix[0][3],
            viewMatrix[1][3],   
            viewMatrix[2][3],    
        };
        // viewMatrix[3][3] = 0.9999999f; // don't now why but if fix problem when load, cubemap is empty
        viewMatrix[0][3] = 0.0f;
        viewMatrix[1][3] = 0.0f;
        viewMatrix[2][3] = 0.0f;
        // matrix_print((GLfloat *)viewMatrix);
        glUniformMatrix4fv(cubemapViewID, 1, GL_TRUE, (const float *)viewMatrix);
        viewMatrix[0][3] = viewTransformationTemp[0];
        viewMatrix[1][3] = viewTransformationTemp[1];
        viewMatrix[2][3] = viewTransformationTemp[2];

        // glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(cubemapVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0); // unbind
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

