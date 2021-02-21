#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <GL/glew.h>

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags

#include "image_structs.h"

#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )

typedef struct aiScene aiScene;
typedef struct aiNode aiNode;
typedef struct aiMesh aiMesh;
typedef struct aiFace aiFace;
typedef struct aiMaterial aiMaterial;
typedef enum aiTextureType aiTextureType;
typedef struct aiString aiString;

typedef struct Texture {
    u_int id;
    char *type;
    char *path;
} texture;

typedef struct Vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} vec3;

typedef struct Vec2 {
    GLfloat x;
    GLfloat y;
} vec2;

typedef struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;

typedef struct Mesh {
    u_int numberOfVertices;
    vertex *vertices;
    u_int numberOfIndexes;
    u_int *indexes;
    u_int numberOfTextures;
    texture *textures;
    GLuint VAO;
} mesh;

typedef struct Model {
    char *directory;
    u_int numberOfMeshes;
    mesh **meshes;
} model;

model *currentModel = NULL;

u_int numberOfTexturesLoaded = 0;
texture *texturesLoaded = NULL;

/** Get filename with path. Return directory.
 *  @param filename filename with path
 *  @return directory
 *  @note help func */
char *fileDirectory(char *filename) {
    char *tmp;

    u_int directoryLength = strlen(filename) - strlen(strrchr(filename, '/')) + 1;
    tmp = malloc(directoryLength * sizeof(char));
    memcpy(tmp, filename, directoryLength);
    tmp[directoryLength] = '\0';

    return tmp;
}

int textureFromFile(char *filename, char *directory) {
    // printf("textureFromFile filename: %s directory: %s\n", filename, directory);

    char *path = NULL;
    uint8_t *textureData = NULL;
    imageMeta textureMeta;   

    path = malloc(sizeof(char) * (strlen(directory) + strlen(filename) + 1));
    path[0] = '\0';
    strcat(path, directory);
    strcat(path, filename);
    // printf("load %s file...\n", path);

    // Flip texture on the y-axis. OpenGL glTexImage2D data pointer parameter: "The first element corresponds to the LOWER LEFT corner of the texture image."
    // Solution 1. flip image on load
    // Solution 2. flip images before load
    // Solution 3. flip with shader(fs) textureCoordinates -> vec2(textureCoordinates.x, 1.0 - textureCoordinates.y)
    stbi_set_flip_vertically_on_load(true);
    textureData = stbi_load(path, &textureMeta.x, &textureMeta.y, &textureMeta.colors, 0);
    if(!textureData) {
        printf("Failed to load %s. Reason: %s\n", path, stbi_failure_reason());
        return -1;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    // printf("textureID %d\n", textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), textureMeta.x, textureMeta.y, 0, (textureMeta.colors == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    

    // printf("load %s width: %d, height: %d, colors: %d\n", filename, textureMeta.x, textureMeta.y, textureMeta.colors);
    stbi_image_free(textureData);

    return textureID;
}

void loadMaterialTexture(mesh *tempMesh, aiMaterial *material, aiTextureType textureType, char *typeName) {
    printf("loadMaterialTexture %s total: %d\n", typeName, aiGetMaterialTextureCount(material, textureType));

    for(int i = 0; i < aiGetMaterialTextureCount(material, textureType); i++) {
        aiString str;
        aiGetMaterialTexture(material, textureType, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
        bool skip = false;
        for(int j = 0; j < numberOfTexturesLoaded; j++) {
            // check if exists and add to textures from texturesLoaded
            // printf("strcmp texturesLoaded[%d].path: %s ??? str.data: %s --- %d \n", j, texturesLoaded[j].path, str.data, strcmp(texturesLoaded[j].path, str.data));
            if(strcmp(texturesLoaded[j].path, str.data) == 0) {
                skip = true;

                tempMesh->numberOfTextures++;
                tempMesh->textures = realloc(tempMesh->textures, tempMesh->numberOfTextures * sizeof(texture));
                memcpy(&tempMesh->textures[tempMesh->numberOfTextures - 1], &texturesLoaded[j], sizeof(texture));
                tempMesh->textures[tempMesh->numberOfTextures - 1].type = typeName;

                break;
            }
        }
        // printf("skip %d \n", skip);

        if(skip == false) {
            texture *currentTexture;
            currentTexture = malloc(sizeof(texture));
            currentTexture->type = typeName;
            currentTexture->path = malloc(sizeof(char) * (strlen(str.data) + 1));
            strcpy(currentTexture->path, str.data);
            currentTexture->id = textureFromFile(str.data, currentModel->directory);
            printf("loadMaterialTexture textureId: %d file: %s%s\n", currentTexture->id, currentModel->directory, currentTexture->path);

            tempMesh->numberOfTextures++;
            tempMesh->textures = realloc(tempMesh->textures, tempMesh->numberOfTextures * sizeof(texture));
            memcpy(&tempMesh->textures[tempMesh->numberOfTextures - 1], currentTexture, sizeof(texture));

            numberOfTexturesLoaded++;
            texturesLoaded = realloc(texturesLoaded, numberOfTexturesLoaded * sizeof(texture));
            memcpy(&texturesLoaded[numberOfTexturesLoaded - 1], currentTexture, sizeof(texture));
        }
    }
    printf("loadMaterialTexture end --- \n");
}

mesh *processMesh(aiMesh *assetMesh, const aiScene *scene) {
    mesh *_mesh = malloc(sizeof(mesh));
    vertex *vertices;
    u_int *indexes;

    printf("processMesh sta -----------------\n");
    printf("mesh->mVertices: %d - ", assetMesh->mNumVertices);
    printf("mesh->mNumFaces: %d - ", assetMesh->mNumFaces);
    _mesh->numberOfVertices = assetMesh->mNumVertices;
    _mesh->numberOfIndexes = assetMesh->mNumFaces * assetMesh->mFaces[0].mNumIndices;
    printf("numberOfIndexes: %d\n", _mesh->numberOfIndexes);

    vertices = malloc(_mesh->numberOfVertices * sizeof(vertex));
    // printf("numberOfVertices * sizeof(vertex) %d\n", numberOfVertices * sizeof(vertex));
    for(u_int i = 0; i < assetMesh->mNumVertices; i++) {
        // printf("vertice[%d] position (%f, %f, %f)\n", i, mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertices[i].position.x = assetMesh->mVertices[i].x;
        vertices[i].position.y = assetMesh->mVertices[i].y;
        vertices[i].position.z = assetMesh->mVertices[i].z;

        if(assetMesh->mNormals) {
            // printf("vertice[%d] normals (%f, %f, %f)\n", i, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertices[i].normal.x = assetMesh->mNormals[i].x;
            vertices[i].normal.y = assetMesh->mNormals[i].y;
            vertices[i].normal.z = assetMesh->mNormals[i].z;
        }

        if(assetMesh->mTextureCoords[0]) {
            // printf("vertice[%d] mTextureCoords (%f, %f)\n", i, assetMesh->mTextureCoords[0][i].x, assetMesh->mTextureCoords[0][i].y);
            vertices[i].texCoords.x = assetMesh->mTextureCoords[0][i].x;
            vertices[i].texCoords.y = assetMesh->mTextureCoords[0][i].y;
        // } else {
        //     printf("vertice[%d] mTextureCoords no\n", i);
        }
    }

    indexes = malloc(_mesh->numberOfIndexes * sizeof(u_int));
    for(u_int i = 0; i < assetMesh->mNumFaces; i++) {
        aiFace face = assetMesh->mFaces[i];
        for(u_int j = 0; j < face.mNumIndices; j++) {
            // printf("face[%d][%d] index %d\n", i, j, face.mIndices[j]);
            indexes[i * face.mNumIndices + j] = face.mIndices[j];
            // printf("indexes[%d] %d\n", i * 3 + j, indexes[i * 3 + j]);
        }
    }

    printf("assetMesh->mMaterialIndex %d\n", assetMesh->mMaterialIndex);
    _mesh->numberOfTextures = 0;
    _mesh->textures = NULL;
    if(assetMesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[assetMesh->mMaterialIndex];
        loadMaterialTexture(_mesh, material, aiTextureType_DIFFUSE, "texture_diffuse");
        loadMaterialTexture(_mesh, material, aiTextureType_SPECULAR, "texture_specular");
    }

    _mesh->vertices = vertices;
    _mesh->indexes = indexes;
    printf("processMesh end -----------------\n");

    return _mesh;
}

void processNode(aiNode *node, const aiScene *scene) {
    // printf("processNode --- start --- \n");
    // printf("processNode node->mNumMeshes %d\n", node->mNumMeshes);

    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *assetMesh = scene->mMeshes[node->mMeshes[i]];

        currentModel->numberOfMeshes++;

        currentModel->meshes = (mesh **)realloc(currentModel->meshes, (currentModel->numberOfMeshes * sizeof(mesh*)));

        currentModel->meshes[currentModel->numberOfMeshes - 1] = processMesh(assetMesh, scene);
        // printf("currentModel->meshes[%d]: %d\n", currentModel->numberOfMeshes - 1, currentModel->meshes[currentModel->numberOfMeshes - 1]->numberOfVertices);
    }

    // printf("processNode node->mNumChildren %d\n", node->mNumChildren);
    for(int i = 0; i < node->mNumChildren; i++) {
        // printf("node->child %s\n", node[i].mName);
        processNode(node->mChildren[i], scene);
    }

    // printf("processNode --- end --- \n");
}

u_int totalMeshes(aiNode *node, const aiScene *scene, u_int meshes) {
    // printf("totalMeshes node->mNumMeshes %d\n", node->mNumMeshes);
    meshes += node->mNumMeshes;
    
    // printf("totalMeshes node->mNumChildren %d\n", node->mNumChildren);
    for(int i = 0; i < node->mNumChildren; i++) {
        meshes = totalMeshes(node->mChildren[i], scene, meshes);
    }

    return meshes;
}

GLuint meshSetup(mesh *currentMesh) {
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, currentMesh->numberOfVertices * sizeof(vertex), currentMesh->vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh->numberOfIndexes * sizeof(u_int), currentMesh->indexes, GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind VAO

    return VAO;
}

void modelInstanceSetup(model *currentModel, u_int instanceVBO) {
    for(int i = 0; i < currentModel->numberOfMeshes; i++) {
        glBindVertexArray(currentModel->meshes[i]->VAO);

        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
        glVertexAttribDivisor(3, 1);
    }

    glBindVertexArray(0); // unbind VAO
}

void modelDraw(GLuint shaderID, model *currentModel, u_int instalces) {
    glUseProgram(shaderID);
    // printf("draw %s - Meshes %d\n", currentModel->directory, currentModel->numberOfMeshes);

    for(int i = 0; i < currentModel->numberOfMeshes; i++) {
        // printf("draw currentModel->meshes[%d]->VAO %d\n", i, currentModel->meshes[i]->VAO);
        // printf("draw currentModel->meshes[%d]->numberOfIndexes %d\n", i, currentModel->meshes[i]->numberOfIndexes);
        // printf("texture_diffuse %d\n", glGetUniformLocation(shaderID, "texture_diffuse"));

        u_int diffuseNr = 1;
        u_int specularNr = 1;
        
        for(int j = 0; j < currentModel->meshes[i]->numberOfTextures; j++) {
            if(strcmp(currentModel->meshes[i]->textures[j].type, "texture_diffuse") == 0) {
                glActiveTexture(GL_TEXTURE0 + j);

                glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse"), j);
                // printf("texture_diffuse: %d j: %d\n", glGetUniformLocation(shaderID, "texture_diffuse"), j);
                diffuseNr++;

                glBindTexture(GL_TEXTURE_2D, currentModel->meshes[i]->textures[j].id);
                // printf("currentModel->textures[%d] id %d path %s type %s\n", j, currentModel->meshes[i]->textures[j].id, currentModel->meshes[i]->textures[j].path, currentModel->meshes[i]->textures[j].type);
            }
            
            if(strcmp(currentModel->meshes[i]->textures[j].type, "texture_specular") == 0) {
                glUniform1i(glGetUniformLocation(shaderID, "texture_specular"), j);
                specularNr++;
            }
        }

        glBindVertexArray(currentModel->meshes[i]->VAO);
        if(instalces) {
            glDrawElementsInstanced(GL_TRIANGLES, currentModel->meshes[i]->numberOfIndexes, GL_UNSIGNED_INT, 0, instalces);
        } else {
            glDrawElements(GL_TRIANGLES, currentModel->meshes[i]->numberOfIndexes, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0); // unbind VAO

        glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
    }
}

model *modelLoad(char *filename) {
    numberOfTexturesLoaded = 0;
    texturesLoaded = NULL;

    const aiScene* scene = aiImportFile(filename
        , aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    if(!scene) {
        fprintf(stderr, "aiImportFile Error: %s", aiGetErrorString());
        // return EXIT_FAILURE;
    }

    printf("scene->mNumMaterials %d\n", scene->mNumMaterials);

    currentModel = malloc(sizeof(model));
    currentModel->numberOfMeshes = 0;
    currentModel->meshes = NULL;

    currentModel->directory = fileDirectory(filename);
    printf("\n ~~~ file directory %s ~~~\n", currentModel->directory);

    // printf("numberOfMeshes %d\n", currentModel->numberOfMeshes);
    // currentModel->numberOfMeshes = totalMeshes(scene->mRootNode, scene, 0);
    // printf("numberOfMeshes %d\n", currentModel->numberOfMeshes);

    // currentModel->meshes = malloc(currentModel->numberOfMeshes * sizeof(mesh));
    // just to know that scene can have meshes... but looks like it child (may be not always...)
    processNode(scene->mRootNode, scene);

    printf("meshSetup numberOfMeshes: %d\n", currentModel->numberOfMeshes);
    for(int i = 0; i < currentModel->numberOfMeshes; i++) {
        printf("meshes[%d] Vertices: %d - Indexes: %d - Textures: %d\n", i, currentModel->meshes[i]->numberOfVertices, currentModel->meshes[i]->numberOfIndexes, currentModel->meshes[i]->numberOfTextures);
        currentModel->meshes[i]->VAO = meshSetup(currentModel->meshes[i]);
    }

    return currentModel;
}