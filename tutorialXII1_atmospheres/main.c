#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/loadShader.c"

#include "common/bmp_load.h"
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

    int i = 0;
    
    //clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // default
    //float clearColor[] = { 0.671, 0.369, 0.204, 1.0f }; // desert
    //float clearColor[] = { 0.1, 0.1, 0.1, 1.0f }; // factory
    float clearColor[] = { 0.0, 0.0, 0.0, 1.0f }; // horror
    //float clearColor[] = { 0.9, 0.9, 0.9, 1.0f }; // biochemical lab
    
    static const GLfloat box[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    static const uint8_t CUBES = 10;
    static const GLfloat cubePositions[] = {
        0.0f,  0.0f,  0.0f,
        2.0f,  5.0f, -15.0f,
        -1.5f, -2.2f, -2.5f,
        -3.8f, -2.0f, -12.3f,
         2.4f, -0.4f, -3.5f,
        -1.7f,  3.0f, -7.5f,
         1.3f, -2.0f, -2.5f,
         1.5f,  2.0f, -2.5f,
         1.5f,  0.2f, -1.5f,
        -1.3f,  1.0f, -1.5f,
    };    
    
    uint8_t *containerTextureData = NULL;
    uint32_t containerTextureSize[2] = { 0, 0 };
    
    bmp_load("container2.bmp", &containerTextureData, &containerTextureSize[0], &containerTextureSize[1]);
    
    GLuint containerTextureID;
    glGenTextures(1, &containerTextureID);
    glBindTexture(GL_TEXTURE_2D, containerTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, containerTextureSize[0], containerTextureSize[1], 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, containerTextureData);
    
    free(containerTextureData);
    
    bmp_load("container2_specular.bmp", &containerTextureData, &containerTextureSize[0], &containerTextureSize[1]);
    
    GLuint containerSpecularTextureID;
    glGenTextures(1, &containerSpecularTextureID);
    glBindTexture(GL_TEXTURE_2D, containerSpecularTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, containerTextureSize[0], containerTextureSize[1], 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, containerTextureData);
    
    free(containerTextureData);
    
    //Material
    float shininess = 32.0f;
    
//    float directionLightDirection[3] = { -0.2f, -1.0f, -0.3f };
//    float directionLightAmbient[3] = { 0.2f, 0.2f, 0.2f };
//    float directionLightDiffuse[3] = { 0.5f, 0.5f, 0.5f };
//    float directionLightSpecular[3] = { 1.0f, 1.0f, 1.0f };

      // desert
//    float directionLight[][3] = { 
//        { -0.2f, -1.0f, -0.3f }, // Direction
//        { 0.2f, 0.2f, 0.2f }, // Ambient
//        { 0.5f, 0.5f, 0.5f }, // Diffuse
//        { 1.0f, 1.0f, 1.0f }, // Specular
//    };

//    // factory
//    float directionLight[][3] = { 
//        { -0.2f, -1.0f, -0.3f }, // Direction
//        { 0.05f, 0.05f, 0.1f }, // Ambient
//        { 0.2f, 0.2f, 0.7f }, // Diffuse
//        { 0.7f, 0.7f, 0.7f }, // Specular
//    };
    
    // horror
    float directionLight[][3] = {
        { -0.2f, -1.0f, -0.3f }, // Direction
        { 0.0f, 0.0f, 0.0f }, // Ambient
        { 0.05f, 0.05f, 0.05f }, // Diffuse
        { 0.2f, 0.2f, 0.2f }, // Specular
    };

//    // biochemical lab
//    float directionLight[][3] = {
//        { -0.2f, -1.0f, -0.3f }, // Direction
//        { 0.5f, 0.5f, 0.5f }, // Ambient
//        { 1.0f, 1.0f, 1.0f }, // Diffuse
//        { 1.0f, 1.0f, 1.0f }, // Specular
//    };

    static const uint8_t POINT_LIGHTS = 4;
    float pointLightPositions[4][3] = {
	{ 0.7f,  0.2f,  2.0f }, // x-y-z
	{ 2.3f, -3.3f, -4.0f },
	{ -4.0f,  2.0f, -12.0f },
	{ 0.0f,  0.0f, -3.0f },
    };

        // desert
//    float pointLightColors[4][3] = {
//        { 1.0f, 0.6f, 0.0f }, // r-g-b
//        { 1.0f, 0.0f, 0.0f },
//        { 0.0f, 1.0f, 0.0f },
//        { 0.2f, 0.2f, 1.0f },
//    };
    
//    // factory
//    float pointLightColors[4][3] = {
//        { 0.2f, 0.2f, 0.6f }, // r-g-b
//        { 0.3f, 0.3f, 0.7f },
//        { 0.0f, 0.0f, 0.3f },
//        { 0.4f, 0.4f, 0.4f },
//    };
    
    // horror
    float pointLightColors[4][3] = {
        { 0.1f, 0.1f, 0.1f }, // r-g-b
        { 0.1f, 0.1f, 0.1f },
        { 0.1f, 0.1f, 0.1f },
        { 0.3f, 0.1f, 0.1f },
    };
    
//    // biochemical lab
//    float pointLightColors[4][3] = {
//        { 0.4f, 0.7f, 0.1f }, // r-g-b
//        { 0.4f, 0.7f, 0.1f },
//        { 0.4f, 0.7f, 0.1f },
//        { 0.4f, 0.7f, 0.1f },
//    };
    
    float pointLights[4][3][3] = {
        {
            { pointLightColors[0][0] * 0.1f, pointLightColors[0][1] * 0.1f, pointLightColors[0][2] * 0.1f }, // Ambient
            { pointLightColors[0][0], pointLightColors[0][1], pointLightColors[0][2] }, // Diffuse
            { pointLightColors[0][0], pointLightColors[0][1], pointLightColors[0][2] }, // Specular
        },
        {
            { pointLightColors[1][0] * 0.1f, pointLightColors[1][1] * 0.1f, pointLightColors[1][2] * 0.1f }, // Ambient
            { pointLightColors[1][0], pointLightColors[1][1], pointLightColors[1][2] }, // Diffuse
            { pointLightColors[1][0], pointLightColors[1][1], pointLightColors[1][2] }, // Specular
        },
        {
            { pointLightColors[2][0] * 0.1f, pointLightColors[2][1] * 0.1f, pointLightColors[2][2] * 0.1f }, // Ambient
            { pointLightColors[2][0], pointLightColors[2][1], pointLightColors[2][2] }, // Diffuse
            { pointLightColors[2][0], pointLightColors[2][1], pointLightColors[2][2] }, // Specular
        },
        {
            { pointLightColors[3][0] * 0.1f, pointLightColors[3][1] * 0.1f, pointLightColors[3][2] * 0.1f }, // Ambient
            { pointLightColors[3][0], pointLightColors[3][1], pointLightColors[3][2] }, // Diffuse
            { pointLightColors[3][0], pointLightColors[3][1], pointLightColors[3][2] }, // Specular
        },
    };
    
    float pointLightConstant = 1.0f;
    float pointLightLinear = 0.09f;
    float pointLightQuadratic = 0.032f;

        // desert
//    float spotLightCutOff = cos(deg2rad(12.5f));
//    float spotLightOuterCutOff = cos(deg2rad(13.0f));
//    float spotLightAmbient[3] = { 0.0f, 0.0f, 0.0f };
//    float spotLightDiffuse[3] = { 0.8f, 0.8f, 0.8f };
//    float spotLightSpecular[3] = { 0.8f, 0.8f, 0.8f };
//    float spotLightConstant = 1.0f;
//    float spotLightLinear = 0.09f;
//    float spotLightQuadratic = 0.032f;

//    // factory
//    float spotLightCutOff = cos(deg2rad(10.0f));
//    float spotLightOuterCutOff = cos(deg2rad(12.5f));
//    float spotLightAmbient[3] = { 0.0f, 0.0f, 0.0f };
//    float spotLightDiffuse[3] = { 1.0f, 1.0f, 1.0f };
//    float spotLightSpecular[3] = { 1.0f, 1.0f, 1.0f };
//    float spotLightConstant = 1.0f;
//    float spotLightLinear = 0.09f;
//    float spotLightQuadratic = 0.032f;
    
    // horror
    float spotLightCutOff = cos(deg2rad(10.0f));
    float spotLightOuterCutOff = cos(deg2rad(15.0f));
    float spotLightAmbient[3] = { 0.0f, 0.0f, 0.0f };
    float spotLightDiffuse[3] = { 1.0f, 1.0f, 1.0f };
    float spotLightSpecular[3] = { 1.0f, 1.0f, 1.0f };
    float spotLightConstant = 1.0f;
    float spotLightLinear = 0.09f;
    float spotLightQuadratic = 0.032f;
    
//    // biochemical lab
//    float spotLightCutOff = cos(deg2rad(7.0f));
//    float spotLightOuterCutOff = cos(deg2rad(10.0f));
//    float spotLightAmbient[3] = { 0.0f, 0.0f, 0.0f };
//    float spotLightDiffuse[3] = { 0.0f, 1.0f, 0.0f };
//    float spotLightSpecular[3] = { 0.0f, 1.0f, 0.0f };
//    float spotLightConstant = 1.0f;
//    float spotLightLinear = 0.09f;
//    float spotLightQuadratic = 0.032f;
    
    // Init Shaders, VAO, VBO, Link it
    
    GLuint programId;
    programId = loadShader("shader.vs", "shader.fs");
    if(programId == -1) {
        return (EXIT_FAILURE);
    }
    
    GLuint lightProgramId;
    lightProgramId = loadShader("shader_light.vs", "shader_light.fs");
    if(lightProgramId == -1) {
        return (EXIT_FAILURE);
    }
    
    // Create VAO
    GLuint vertexArrayObjectID; // VAO
    glGenVertexArrays(1, &vertexArrayObjectID);
    
    // Create VBO
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    
    glBindVertexArray(vertexArrayObjectID);
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
    
    glBindVertexArray(0);  // unbind VAO
    
    // Lighting
    GLuint lightVertexArrayObjectID;
    glGenVertexArrays(1, &lightVertexArrayObjectID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // same cube data
    
    glBindVertexArray(lightVertexArrayObjectID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);  // unbind VAO

    GLfloat lightScale[3] = { 0.2f, 0.2f, 0.2f };
    
    GLuint modelID = glGetUniformLocation(programId, "model");
    GLuint viewID = glGetUniformLocation(programId, "view");
    GLuint projectionID = glGetUniformLocation(programId, "projection");

    GLuint viewPositionID = glGetUniformLocation(programId, "viewPosition");
    
    // Material
    //GLuint materialAmbientID = glGetUniformLocation(programId, "material.ambient");
    GLuint materialDiffuseID = glGetUniformLocation(programId, "material.diffuse");
    GLuint materialSpecularID = glGetUniformLocation(programId, "material.specular");
    GLuint materialShininessID = glGetUniformLocation(programId, "material.shininess");
    
    // light
    GLuint directionLightDirectionID = glGetUniformLocation(programId, "directionLight.direction");
    GLuint directionLightAmbientID = glGetUniformLocation(programId, "directionLight.ambient");
    GLuint directionLightDiffuseID = glGetUniformLocation(programId, "directionLight.diffuse");
    GLuint directionLightSpecularID = glGetUniformLocation(programId, "directionLight.specular");
    
    GLuint pointLightPositionID[POINT_LIGHTS];
    GLuint pointLightAmbientID[POINT_LIGHTS];
    GLuint pointLightDiffuseID[POINT_LIGHTS];
    GLuint pointLightSpecularID[POINT_LIGHTS];
    // light attenuation
    GLuint pointLightConstantID[POINT_LIGHTS];
    GLuint pointLightLinearID[POINT_LIGHTS];
    GLuint pointLightQuadraticID[POINT_LIGHTS];
    
    char tmp[255];
    for(i = 0; i < 4; i++) {
        sprintf(tmp, "pointLights[%u].position", i);
        pointLightPositionID[i] = glGetUniformLocation(programId, tmp);
        sprintf(tmp, "pointLights[%u].ambient", i);
        pointLightAmbientID[i] = glGetUniformLocation(programId, tmp);
        sprintf(tmp, "pointLights[%u].diffuse", i);
        pointLightDiffuseID[i] = glGetUniformLocation(programId, tmp);
        sprintf(tmp, "pointLights[%u].specular", i);
        pointLightSpecularID[i] = glGetUniformLocation(programId, tmp);
        // light attenuation
        sprintf(tmp, "pointLights[%u].constant", i);
        pointLightConstantID[i] = glGetUniformLocation(programId, tmp);
        sprintf(tmp, "pointLights[%u].linear", i);
        pointLightLinearID[i] = glGetUniformLocation(programId, tmp);
        sprintf(tmp, "pointLights[%u].quadratic", i);
        pointLightQuadraticID[i] = glGetUniformLocation(programId, tmp);
    }
    
    GLuint spotLightPositionID = glGetUniformLocation(programId, "spotLight.position");
    GLuint spotLightDirectionID = glGetUniformLocation(programId, "spotLight.direction");
    GLuint spotLightCutOffID = glGetUniformLocation(programId, "spotLight.cutOff");
    GLuint spotLightOuterCutOffID = glGetUniformLocation(programId, "spotLight.outerCutOff");
    GLuint spotLightAmbientID = glGetUniformLocation(programId, "spotLight.ambient");
    GLuint spotLightDiffuseID = glGetUniformLocation(programId, "spotLight.diffuse");
    GLuint spotLightSpecularID = glGetUniformLocation(programId, "spotLight.specular");
    // light attenuation
    GLuint spotLightConstantID = glGetUniformLocation(programId, "spotLight.constant");
    GLuint spotLightLinearID = glGetUniformLocation(programId, "spotLight.linear");
    GLuint spotLightQuadraticID = glGetUniformLocation(programId, "spotLight.quadratic");    
    
    // lightProgramId
    GLuint lightModelID = glGetUniformLocation(lightProgramId, "model");
    GLuint lightViewID = glGetUniformLocation(lightProgramId, "view");
    GLuint lightProjectionID = glGetUniformLocation(lightProgramId, "projection");
    GLuint lightColorID = glGetUniformLocation(lightProgramId, "color");    
    
    
    GLfloat modelMatrix[4][4] = {};
    GLfloat viewMatrix[4][4] = {};
    GLfloat projectionMatrix[4][4] = {};
    
    matrix_identity((GLfloat *)modelMatrix);
    matrix_identity((GLfloat *)viewMatrix);
    matrix_identity((GLfloat *)projectionMatrix);

    //GLfloat axes[3] = { 0.5f, 1.0f, 0.0f };

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTextureID);
    glUniform1i(materialDiffuseID, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, containerSpecularTextureID);
    glUniform1i(materialSpecularID, 1);
    glActiveTexture(GL_TEXTURE2);
    glUniform1f(materialShininessID, shininess);
    
    glUniform3fv(directionLightDirectionID, 1, (GLfloat *)&directionLight[0]);
    glUniform3fv(directionLightAmbientID, 1, (GLfloat *)&directionLight[1]);
    glUniform3fv(directionLightDiffuseID, 1, (GLfloat *)&directionLight[2]);
    glUniform3fv(directionLightSpecularID, 1, (GLfloat *)&directionLight[3]);
    for(i = 0; i < POINT_LIGHTS; i++) {
        glUniform3fv(pointLightAmbientID[i], 1, (GLfloat *)&pointLights[i][0]);
        glUniform3fv(pointLightDiffuseID[i], 1, (GLfloat *)&pointLights[i][1]);
        glUniform3fv(pointLightSpecularID[i], 1, (GLfloat *)&pointLights[i][2]);

        glUniform1f(pointLightConstantID[i], pointLightConstant);
        glUniform1f(pointLightLinearID[i], pointLightLinear);
        glUniform1f(pointLightQuadraticID[i], pointLightQuadratic);
    }

    glUniform1f(spotLightCutOffID, spotLightCutOff);
    glUniform1f(spotLightOuterCutOffID, spotLightOuterCutOff);
    
    glUniform3fv(spotLightAmbientID, 1, &spotLightAmbient[0]);
    glUniform3fv(spotLightDiffuseID, 1, &spotLightDiffuse[0]);
    glUniform3fv(spotLightSpecularID, 1, &spotLightSpecular[0]);

    glUniform1f(spotLightConstantID, spotLightConstant);
    glUniform1f(spotLightLinearID, spotLightLinear);
    glUniform1f(spotLightQuadraticID, spotLightQuadratic);   
    
    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

    float axis[3] = { 1.0f, 0.3f, 0.5f };
    
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
        glUniform3fv(viewPositionID, 1, &cameraPosition[0]);

        for(i = 0; i < POINT_LIGHTS; i++) {
            glUniform3fv(pointLightPositionID[i], 1, (GLfloat *)&pointLightPositions[i]);
        }

        glUniform3fv(spotLightPositionID, 1, &cameraPosition[0]);
        glUniform3fv(spotLightDirectionID, 1, &cameraFront[0]);        
        
        for(i = 0; i < CUBES * 3; i += 3) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)&cubePositions[i]);
            float angel = 20.0f * i / 3;
            matrix_rotate((GLfloat *)modelMatrix, angel, axis);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            
            glBindVertexArray(vertexArrayObjectID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // light
        glUseProgram(lightProgramId);
        
        for(i = 0; i < POINT_LIGHTS; i++) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)pointLightPositions[i]);
            matrix_scale((GLfloat *)modelMatrix, (GLfloat *)lightScale);
            glUniformMatrix4fv(lightModelID, 1, GL_TRUE, (const float *)modelMatrix);
            
            glUniform3fv(lightColorID, 1, (GLfloat *)&pointLightColors[i]);

            glUniformMatrix4fv(lightViewID, 1, GL_TRUE, (const float *)viewMatrix);
            glUniformMatrix4fv(lightProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);

            glBindVertexArray(lightVertexArrayObjectID);
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

