#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GL/glew.h>

GLuint loadShader(const char *vertex_file_path, const char *fragment_file_path) {
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    
    GLint result = GL_FALSE;
    int infoLogLength;
    
    FILE *fp;
    int sizeOfFile;
    
    fp = fopen(vertex_file_path, "r");
    if(fp == NULL) {
        fprintf(stderr, "vertex shader file not found\n");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    sizeOfFile = ftell(fp);
    rewind(fp);
    
    char *vertexShaderCode = malloc((sizeOfFile + 1) * sizeof(char));
    fread(vertexShaderCode, 1, (sizeOfFile + 1), fp);
    fclose(fp);
    vertexShaderCode[sizeOfFile] = '\0';
    //printf("vertexShaderCode %s\n", vertexShaderCode);

    char const *vertexSourcePointer = vertexShaderCode;
    glShaderSource(vertexShaderId, 1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderId);
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        fprintf(stderr, "vertex shader compile error\n");
        
        glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *vertexShaderErrorMessage = malloc(sizeof(char) * (infoLogLength + 1));
        glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, vertexShaderErrorMessage);
        fprintf(stderr, "vertex Shader Error Message %s\n", vertexShaderErrorMessage);
        
        return -1;
    }
    
    fp = fopen(fragment_file_path, "r");
    if(fp == NULL) {
        fprintf(stderr, "fragment shader file not found\n");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    sizeOfFile = ftell(fp);
    rewind(fp);
    
    char *fragmentShaderCode = malloc((sizeOfFile + 1) * sizeof(char));
    fread(fragmentShaderCode, 1, (sizeOfFile + 1), fp);
    fclose(fp);
    fragmentShaderCode[sizeOfFile] = '\0';
    //printf("fragmentShaderCode %s\n", fragmentShaderCode);
    
    char const *fragmentSourcePointer = fragmentShaderCode;
    glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        fprintf(stderr, "fragment shader compile error\n");
        
        glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *fragmentShaderErrorMessage = malloc(sizeof(char) * (infoLogLength + 1));
        glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, fragmentShaderErrorMessage);
        fprintf(stderr, "fragment Shader Error Message %s\n", fragmentShaderErrorMessage);
        
        return -1;
    }    

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) {
        fprintf(stderr, "fragment shader compile error\n");
        return -1;
    }
    
    //glDetachShader(programId, vertexShaderId);
    //glDetachShader(programId, fragmentShaderId);
    
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    
    return programId;
}
