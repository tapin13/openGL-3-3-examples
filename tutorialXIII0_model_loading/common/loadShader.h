#ifndef LOADSHADER_H
#define LOADSHADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GL/glew.h>

GLuint loadShader(const char *vertex_file_path, const char *fragment_file_path);

#endif /* LOADSHADER_H */
