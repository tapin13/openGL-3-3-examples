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

    float lightPosition[3] = { 0.3f, 0.25f, 1.2f };
    //float lightDirection[3] = { -0.2f, -1.0f, -0.3f };
    float cutOff = cos(deg2rad(12.5f));
    float outerCutOff = cos(deg2rad(17.5f));
    float lightAmbient[3] = { 0.1f, 0.1f, 0.1f };
    float lightDiffuse[3] = { 0.8f, 0.8f, 0.8f };
    float lightSpecular[3] = { 1.0f, 1.0f, 1.0f };
    
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    
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
    GLuint lightPositionID = glGetUniformLocation(programId, "light.position");
    GLuint lightDirectionID = glGetUniformLocation(programId, "light.direction");
    GLuint lightCutOffID = glGetUniformLocation(programId, "light.cutOff");
    GLuint lightOuterCutOffID = glGetUniformLocation(programId, "light.outerCutOff");
    GLuint lightAmbientID = glGetUniformLocation(programId, "light.ambient");
    GLuint lightDiffuseID = glGetUniformLocation(programId, "light.diffuse");
    GLuint lightSpecularID = glGetUniformLocation(programId, "light.specular");
    // light attenuation
    GLuint lightConstantID = glGetUniformLocation(programId, "light.constant");
    GLuint lightLinearID = glGetUniformLocation(programId, "light.linear");
    GLuint lightQuadraticID = glGetUniformLocation(programId, "light.quadratic");
    
    // lightProgramId
    GLuint lightModelID = glGetUniformLocation(lightProgramId, "model");
    GLuint lightViewID = glGetUniformLocation(lightProgramId, "view");
    GLuint lightProjectionID = glGetUniformLocation(lightProgramId, "projection");
    
    
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
    
    glUniform3fv(lightAmbientID, 1, &lightAmbient[0]);
    glUniform3fv(lightDiffuseID, 1, &lightDiffuse[0]);
    glUniform3fv(lightSpecularID, 1, &lightSpecular[0]);
    glUniform1f(lightCutOffID, cutOff);
    glUniform1f(lightOuterCutOffID, outerCutOff);

    glUniform1f(lightConstantID, constant);
    glUniform1f(lightLinearID, linear);
    glUniform1f(lightQuadraticID, quadratic);
    
    float fov = 55.0f;
    matrix_perspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f, (GLfloat *)projectionMatrix);
    glUniformMatrix4fv(projectionID, 1, GL_TRUE, (const float *)projectionMatrix);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;
    
    float lightYaw = 50.0f;
    float lightPitch = 25.0f;
    
    int i = 0;
    float axis[3] = { 1.0f, 0.3f, 0.5f };
    
    while(glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        
        lightYaw = glfwGetTime() * 100.0f;

        // light animation
        //lightPosition[0] = 1.5 * cos(deg2rad(lightPitch)) * cos(deg2rad(lightYaw));
        //lightPosition[1] = 1.5 * sin(deg2rad(lightPitch));
        //lightPosition[2] = 1.5 * cos(deg2rad(lightPitch)) * sin(deg2rad(lightYaw));
        //printf("cos(deg2rad(lightPitch)): %f\n", cos(deg2rad(lightPitch)));
        
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //printf("deltaTime: %f\n", deltaTime);
        
        processInput(window, cameraPosition, cameraFront, cameraUp, deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

        glUniform3fv(lightPositionID, 1, &cameraPosition[0]);
        glUniform3fv(lightDirectionID, 1, &cameraFront[0]);

        for(i = 0; i < 10 * 3; i += 3) {
            matrix_identity((GLfloat *)modelMatrix);
            matrix_translate((GLfloat *)modelMatrix, (GLfloat *)&cubePositions[i]);
            float angel = 20.0f * i / 3;
            matrix_rotate((GLfloat *)modelMatrix, angel, axis);
            glUniformMatrix4fv(modelID, 1, GL_TRUE, (const float *)modelMatrix);
            
            glBindVertexArray(vertexArrayObjectID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // light
        /*
        glUseProgram(lightProgramId);
        
        matrix_identity((GLfloat *)modelMatrix);
        matrix_translate((GLfloat *)modelMatrix, (GLfloat *)lightPosition);
        matrix_scale((GLfloat *)modelMatrix, (GLfloat *)lightScale);
        glUniformMatrix4fv(lightModelID, 1, GL_TRUE, (const float *)modelMatrix);
        
        glUniformMatrix4fv(lightViewID, 1, GL_TRUE, (const float *)viewMatrix);
        glUniformMatrix4fv(lightProjectionID, 1, GL_TRUE, (const float *)projectionMatrix);
        
        glBindVertexArray(lightVertexArrayObjectID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        */
        
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

