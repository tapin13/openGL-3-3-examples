#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846	/* pi */
#endif

// angle convert degree to radian
float deg2rad(float angle);

void vector_normalize(float *vector);
void vector_cross(float *vector, float *vector2);

void matrix_print(GLfloat *matrix);
void matrix_identity(GLfloat *matrix);
void matrix_translate(GLfloat *matrix, GLfloat *position);
void matrix_scale(GLfloat *matrix, float *scale);
void matrix_perspective(float angle, float ratio, float near, float far, GLfloat *matrix);
void matrix_rotate(GLfloat *matrix, float angle, float *axis);

//void matrix_lootAt(GLfloat *matrix, GLfloat *cameraRight, GLfloat *cameraUp, GLfloat *cameraDirection, GLfloat *cameraPosition);
void matrix_lootAt(GLfloat *matrix, GLfloat *cameraPosition, GLfloat *cameraTarget, GLfloat *up);

#endif /* MATRIX_H */

