#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>

// angle convert degree to radian
float deg2rad(float angle);

void vector_normalize(float *vector);
void vector_cross(float *vector, float *vector2);

void matrix_print(GLfloat *matrix);
void matrix_identity(GLfloat *matrix);
void matrix_translate(GLfloat *matrix, GLfloat *position);
void matrix_scale(float scale, GLfloat *matrix);
void matrix_perspective(float angle, float ratio, float near, float far, GLfloat *matrix);
void matrix_rotate(GLfloat *matrix, float angle, float *axis);

//void matrix_lootAt(GLfloat *matrix, GLfloat *cameraRight, GLfloat *cameraUp, GLfloat *cameraDirection, GLfloat *cameraPosition);
void matrix_lootAt(GLfloat *matrix, GLfloat *cameraPosition, GLfloat *cameraTarget, GLfloat *up);

#endif /* MATRIX_H */

