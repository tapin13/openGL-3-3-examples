#include "model.h"

#include "bmp_load.h"

char model_load(char *filepath, model *_model) {
    printf("model_load: %s\n", filepath);
    
    _model->objects_count = 0;
    // offset for first object
    _model->objects[0].vertex_start = 0;
    
    int i = 0;
    
    //char *filepath = "models/nanosuit/nanosuit.obj";
    FILE *fp;
    
    const char path_separator = '/';
    char *path_separator_ptr;

    path_separator_ptr = strrchr(filepath, path_separator);
    
    char model_path[1024];
    
    memcpy(model_path, filepath, strlen(filepath) - strlen(path_separator_ptr) + 1);
    model_path[strlen(filepath) - strlen(path_separator_ptr) + 1] = '\0';
    //printf("model_path: %s\n", model_path);
    
    fp = fopen(filepath, "rb");
    if(fp == NULL) {
        printf("Failed to open %s\n", filepath);
        return -1;
    }
    
    char str[MAX_CHARS_LINE];
    char objects[100][255];
    char object_mtllib[255];
    
    char object_mtls[100][255];
    int object_mtls_count = 0;
    /*
    float vertexes[100000][3];
    float normals[100000][3];
    float textures[100000][2];
    GLuint indexes_v[100000];
    GLuint indexes_t[100000];
    GLuint indexes_n[100000];
    */

    float *vertexes = (float*)malloc(100000 * 3 * sizeof(float));
    float *normals = (float*)malloc(100000 * 3 * sizeof(float));
    float *textures = (float*)malloc(100000 * 2 * sizeof(float));
    GLuint *indexes_v = (GLuint*)malloc(100000 * 3 * sizeof(GLuint));
    GLuint *indexes_t = (GLuint*)malloc(100000 * 3 * sizeof(GLuint));
    GLuint *indexes_n = (GLuint*)malloc(100000 * 3 * sizeof(GLuint));

    int vertex_counter = 0;
    int normals_counter = 0;
    int textures_counter = 0;
    int index_counter = 0;
    while (fgets(str, MAX_CHARS_LINE, fp) != NULL) {
        //printf("%s", str);
        if(strncmp(str, "o ", 2) == 0) {
            sscanf(str, "o %s", &objects[_model->objects_count][0]);
            //printf("objects[_model->objects_count %u]: ~%s~\n", _model->objects_count, objects[_model->objects_count]);
            if(_model->objects_count != 0) {
                _model->objects[_model->objects_count].vertex_start = index_counter;
                _model->objects[_model->objects_count - 1].vertex_end = index_counter;
            }
            _model->objects_count++;
        } else if(strncmp(str, "usemtl ", 7) == 0) {
            sscanf(str, "usemtl %s", &object_mtls[object_mtls_count][0]);
            //printf("object_mtls[object_mtls_count]: ~%s~\n", object_mtls[object_mtls_count]);
            object_mtls_count++;
        } else if(strncmp(str, "v ", 2) == 0) {
//                printf("%s", str);
            sscanf(str, "v %f %f %f", 
                    &vertexes[vertex_counter + 0],
                    &vertexes[vertex_counter + 1],
                    &vertexes[vertex_counter + 2]
                    );
            //    printf("vertexes: %f %f %f\n"
            //            , vertexes[vertex_counter + 0]
            //            , vertexes[vertex_counter + 1]
            //            , vertexes[vertex_counter + 2]
            //            );
            vertex_counter += 3;
        } else if(strncmp(str, "vn ", 3) == 0) {
//                printf("%s", str); 
            sscanf(str, "vn %f %f %f", 
                    &normals[normals_counter + 0],
                    &normals[normals_counter + 1],
                    &normals[normals_counter + 2]
                    );
//                printf("normalszz: %f %f %f\n"
//                        , normalszz[normals_counter + 0]
//                        , normalszz[normals_counter + 1]
//                        , normalszz[normals_counter + 2]
//                        );
            normals_counter += 3;
        } else if(strncmp(str, "vt ", 3) == 0) {
//                printf("%s", str);
            sscanf(str, "vt %f %f", 
                    &textures[textures_counter + 0],
                    &textures[textures_counter + 1]
                    );
//                printf("vertexes: %f %f %f\n"
//                        , vertexes[vertex_counter + 0]
//                        , vertexes[vertex_counter + 1]
//                        , vertexes[vertex_counter + 2]
//                        );
            textures_counter += 2;
        } else if(strncmp(str, "f ", 2) == 0) {
            //printf("%s", str);
            sscanf(str, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
                    &indexes_v[index_counter + 0],
                    &indexes_t[index_counter + 0],
                    &indexes_n[index_counter + 0],
                    &indexes_v[index_counter + 1],
                    &indexes_t[index_counter + 1],
                    &indexes_n[index_counter + 1],
                    &indexes_v[index_counter + 2],
                    &indexes_t[index_counter + 2],
                    &indexes_n[index_counter + 2]
                    );

            indexes_v[index_counter + 0]--;
            indexes_t[index_counter + 0]--;
            indexes_n[index_counter + 0]--;
            indexes_v[index_counter + 1]--;
            indexes_t[index_counter + 1]--;
            indexes_n[index_counter + 1]--;
            indexes_v[index_counter + 2]--;
            indexes_t[index_counter + 2]--;
            indexes_n[index_counter + 2]--;

//                printf("indexes: %u %u %u\n"
//                        , indexes[index_counter + 0]
//                        , indexes[index_counter + 1]
//                        , indexes[index_counter + 2]
//                        );
            index_counter += 3;
        } else if(strncmp(str, "mtllib ", 7) == 0) {
            sscanf(str, "mtllib %s", &object_mtllib[0]);
        }
    }
    // no objects definition
    if(index_counter > 0 && _model->objects_count == 0) {
        _model->objects_count++;
    }
    _model->objects[_model->objects_count - 1].vertex_end = index_counter;
    
    fclose(fp);
    
//    printf("vertex_counter: %d\n", vertex_counter);
//    printf("normals_counter: %d\n", normals_counter);
//    printf("textures_counter: %d\n", textures_counter);
//    printf("index_counter: %d\n", index_counter);

    int vnt_count = 0;
    int vnt_i = 0;
    uint32_t vnt_size = index_counter * (3 + 3 + 2) * sizeof(float);
    float *vnt;
    vnt = (float*)malloc(vnt_size); // 3 vetex, 3 normal, 2 texture
    if(vnt == NULL) {
        fprintf(stderr, "Failed to malloc of size %d\n", vnt_size);
        return -1;
    }

    for(i = 0; i < index_counter; i++) {
        vnt[vnt_i + 0] = vertexes[indexes_v[i] * 3 + 0];
        vnt[vnt_i + 1] = vertexes[indexes_v[i] * 3 + 1];
        vnt[vnt_i + 2] = vertexes[indexes_v[i] * 3 + 2];
        
        vnt[vnt_i + 3] = normals[indexes_n[i] * 3 + 0];
        vnt[vnt_i + 4] = normals[indexes_n[i] * 3 + 1];
        vnt[vnt_i + 5] = normals[indexes_n[i] * 3 + 2];
        
        vnt[vnt_i + 6] = textures[indexes_t[i] * 2 + 0];
        vnt[vnt_i + 7] = textures[indexes_t[i] * 2 + 1];

        vnt_count++;
        vnt_i+=8;
    }
    
    printf("vnt_count: %d\n", vnt_count);
    printf("vnt_i: %d\n", vnt_i);

    /* Load model end  */

    mtl mtls[100];
    int mtls_count;

    load_mtls(model_path, object_mtllib, mtls, &mtls_count);

    _model->diffuseTextureID = malloc(mtls_count * sizeof(GLuint));
    glGenTextures(mtls_count, &_model->diffuseTextureID[0]);

    _model->specularTextureID = malloc(mtls_count * sizeof(GLuint));
    glGenTextures(mtls_count, &_model->specularTextureID[0]);

    //printf("mtls_count: %u\n", mtls_count);
    
    for(i = 0; i < mtls_count; i++) {
        texture_load(model_path, mtls[i].map_Kd, _model->diffuseTextureID[i]);
        
        if(strlen(mtls[i].map_Ks) > 0) {
            texture_load(model_path, mtls[i].map_Ks, _model->specularTextureID[i]);
        }
    }

    bind_mtl_to_object(_model, mtls, object_mtls, mtls_count);

    set_VAO_VBO(_model, vnt, vnt_size);
    
    free(vnt);

    return 0;
}

void texture_load(char *model_path, char *filename, GLuint textureID) {
    char str[MAX_CHARS_LINE];

    uint8_t *textureData = NULL;
    uint32_t textureSize[3] = { 0, 0, 0 }; // width, height, colors

    strcpy(str, model_path);
    strcat(str, filename);
    strcpy(filename, str);

    //bmp_load("container2.bmp", &textureData, &textureSize[0], &textureSize[1]);
    bmp_load(filename, &textureData, &textureSize[0], &textureSize[1], &textureSize[2]);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, (textureSize[2] == 3 ? GL_RGB : GL_RGBA), textureSize[0], textureSize[1], 0, (textureSize[2] == 3 ? GL_BGR : GL_ABGR_EXT), GL_UNSIGNED_BYTE, textureData);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(textureData);
}

void bind_mtl_to_object(model *_model, mtl *mtls, char object_mtls[][255], int mtls_count) {
    int i, j;
    for(i = 0; i < _model->objects_count; i++) {
        for(j = 0; j < mtls_count; j++) {
            if(strcmp(object_mtls[i], mtls[j].name) == 0) {
                //printf("%u object_mtls %s mtls %s\n", i, object_mtls[i], mtls[j].name);
                _model->objects[i].diffuseTextureID = _model->diffuseTextureID[j];
                _model->objects[i].specularTextureID = _model->specularTextureID[j];
                _model->objects[i].shininess = mtls[j].Ns;
                break;
            }
        }
    }    
}

char load_mtls(char *model_path, char *mtllib, mtl *mtls, int *mtls_count) {
    char filepath_mtl[1024];
    strcpy(filepath_mtl, model_path);
    strcat(filepath_mtl, mtllib);
    
    //printf("filepath_mtl to open %s\n", filepath_mtl);

    FILE *fp;
    char str[MAX_CHARS_LINE];
    
    fp = fopen(filepath_mtl, "rb");
    if(fp == NULL) {
        printf("Failed to open mtl %s\n", filepath_mtl);
        return -1;
    }

    *mtls_count = -1;
    
    while (fgets(str, MAX_CHARS_LINE, fp) != NULL) {
        //printf("%s", str);
        if(strncmp(str, "newmtl ", 7) == 0) {
            (*mtls_count)++;
            sscanf(str, "newmtl %s", mtls[*mtls_count].name);
            mtls[*mtls_count].Ns = 32.0f; // default
            //printf("mtl->name: ~%s~\n", mtls[*mtls_count].name);
        } else if(strncmp(str, "map_Kd ", 7) == 0) {
            sscanf(str, "map_Kd %s", mtls[*mtls_count].map_Kd);
            //printf("map_Kd: ~%s~\n", mtls[*mtls_count].map_Kd);
        } else if(strncmp(str, "map_Ks ", 7) == 0) {
            sscanf(str, "map_Ks %s", mtls[*mtls_count].map_Ks);
            //printf("map_Ks: ~%s~\n", mtls[*mtls_count].map_Ks);
        } else if(strncmp(str, "Ns ", 3) == 0) {
            sscanf(str, "Ns %f", &mtls[*mtls_count].Ns);
            //printf("Ns: ~%f~\n", mtls[*mtls_count].Ns);
        }
    }
    (*mtls_count)++;
    
    fclose(fp);
    
    return 0;
}

void set_VAO_VBO(model *_model, float vnt[], uint32_t vntSize) {
    // Create VAO
    glGenVertexArrays(1, &_model->vertexArrayObjectID);
    
    glBindVertexArray(_model->vertexArrayObjectID);
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, vnt_count * 8 * sizeof(GLfloat), vnt, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vntSize, vnt, GL_STATIC_DRAW);
            
    glBindVertexArray(_model->vertexArrayObjectID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    // layout (location = 0) in vec3 Position;
    // [x,y,z,r,g,b,x,y,z,r....] -> 3 floats + 3 floats for stride
    glVertexAttribPointer(
        0, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
        (void*)(sizeof(GLfloat) * 0) // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);    
    glVertexAttribPointer(
        1, //  position vertex attribute in the vertex shader with layout (location = 0)
        3, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
        (void*)(sizeof(GLfloat) * 3) // array buffer offset
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(
        2, //  position vertex attribute in the vertex shader with layout (location = 0)
        2, //  size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT, // type of the data which is GL_FLOAT
        GL_FALSE, // we want the data to be normalized. But we don't want, data normalized. From 0 to 1.
        sizeof(GLfloat) * 8, // stride - space between consecutive vertex attributes
        (void*)(sizeof(GLfloat) * 6) // array buffer offset
    );
    glEnableVertexAttribArray(2);
    
//    glBindVertexArray(_model->vertexArrayObjectID);
//    GLuint indexBufferObject;
//    glGenBuffers(1, &indexBufferObject);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_counter * sizeof(GLuint), indexes_v, GL_STATIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gl_intex_buffer_data), gl_intex_buffer_data, GL_STATIC_DRAW);
    
    glBindVertexArray(0);  // unbind VAO      
}

void model_set_uniform(model *_model, GLuint programId) {
    _model->shaderMaterialDiffuseID = glGetUniformLocation(programId, "material.diffuse");
    _model->shaderMaterialSpecularID = glGetUniformLocation(programId, "material.specular");
    _model->shaderMaterialShininessID = glGetUniformLocation(programId, "material.shininess");
}

void model_draw(model *_model, GLuint programId) {
    int i = 0;
    
    glBindVertexArray(_model->vertexArrayObjectID);

    for(i = 0; i < _model->objects_count; i++) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _model->objects[i].diffuseTextureID);
        glUniform1i(_model->shaderMaterialDiffuseID, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _model->objects[i].specularTextureID);
        glUniform1i(_model->shaderMaterialSpecularID, 1);

        glUniform1f(_model->shaderMaterialShininessID, _model->objects[i].shininess);
        
        glDrawArrays(GL_TRIANGLES, _model->objects[i].vertex_start, _model->objects[i].vertex_end - _model->objects[i].vertex_start);
    }
    
    glBindVertexArray(0);  // unbind VAO
}
