#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define MAX_CHARS_LINE 255 // maximum number of characters per line

typedef struct {
    char name[255];
    char map_Kd[255];
    char map_Ks[255];
    float Ns; // shininess of the material
} mtl;

typedef struct {
    uint32_t vertex_start;
    uint32_t vertex_end;
    
    GLuint diffuseTextureID;
    GLuint specularTextureID;
    float shininess; // Ns
} object;

typedef struct {
    object objects[100];
    int objects_count;
    
    GLuint vertexArrayObjectID; // VAO
    GLuint *diffuseTextureID;
    GLuint *specularTextureID;
    
    GLuint shaderMaterialDiffuseID;
    GLuint shaderMaterialSpecularID;
    GLuint shaderMaterialShininessID;
} model;

// public
char model_load(char *filepath, model *_model);
void model_set_uniform(model *_model, GLuint programId);
void model_draw(model *_model, GLuint programId);

// private
void texture_load(char *model_path, char *filename, GLuint textureID);
void bind_mtl_to_object(model *_model, mtl *mtls, char object_mtls[][255], int mtls_count);
char load_mtls(char *model_path, char *mtllib, mtl *mtls, int *mtls_count);
void set_VAO_VBO(model *_model, float vnt[], uint32_t vntSize);

#endif /* MODEL_H */