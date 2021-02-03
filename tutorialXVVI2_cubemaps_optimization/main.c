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
    
    // OpenGL start

    // Data

    int i = 0;
    
    float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // default
    // float clearColor[] = { 0.671, 0.369, 0.204, 1.0f }; // desert
    // float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // factory
    // float clearColor[] = { 0.0, 0.0, 0.0, 1.0f }; // horror
    // float clearColor[] = { 0.9, 0.9, 0.9, 1.0f }; // biochemical lab
    
    static const GLfloat box[] = {
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

    printf("load data\n");
    // return;
    // static const uint8_t CUBES = 10;
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

    char *filename = NULL;
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
    glTexImage2D(GL_TEXTURE_2D, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, textureData);
    printf("load %s width: %d, height: %d, colors: %d\n", filename, textureMeta.x, textureMeta.y, textureMeta.colors);
    stbi_image_free(textureData);

    filename = "images/metal.png";
    textureData = stbi_load(filename, &textureMeta.x, &textureMeta.y, &textureMeta.colors, 0);
    GLuint floorTextureID;
    glGenTextures(1, &floorTextureID);
    printf("floorTextureID %d\n", floorTextureID);
    glBindTexture(GL_TEXTURE_2D, floorTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexImage2D(GL_TEXTURE_2D, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, textureData);
    stbi_image_free(textureData);
    
    // Init Shaders, VAO, VBO, Link it
    GLuint programId;
    programId = loadShader("shader.vs", "shader.fs");
    if(programId == -1) {
        return (EXIT_FAILURE);
    }

    GLuint cubemapShaderId;
    cubemapShaderId = loadShader("cubemap_shader.vs", "cubemap_shader.fs");
    if(cubemapShaderId == -1) {
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
        1, //  position vertex attribute in the vertex shader with layout (location = 0)
        2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 5, // stride - space between consecutive vertex attributes
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
    glUniform1i(shaderTextureId, 0);

    glUseProgram(programId);
    GLuint modelID = glGetUniformLocation(programId, "model");
    GLuint viewID = glGetUniformLocation(programId, "view");
    GLuint projectionID = glGetUniformLocation(programId, "projection");
    shaderTextureId = glGetUniformLocation(programId, "texture1");
    // printf("shaderTextureId %d\n", shaderTextureId);
    glUniform1i(shaderTextureId, 0);

    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

    float axis[3] = { 1.0f, 0.3f, 0.5f };
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //printf("currentFrame: %f\n", currentFrame);
        //printf("deltaTime: %f\n", deltaTime);
        
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

        // for(i = 0; i < CUBES * 3; i += 3) {
        i = 0;
            matrix_identity((GLfloat *)modelMatrix);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)&cubePositions[i]);
            float angel = 20.0f * i / 3;
            matrix_rotate((GLfloat *)modelMatrix, angel, axis);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            
            glBindVertexArray(vertexArrayObjectID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, containerTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        // floor
        matrix_identity((GLfloat *)modelMatrix);
        glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

