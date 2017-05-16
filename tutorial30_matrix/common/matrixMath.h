/* 
 * Author: tapin13
 *
 * Created on May 12, 2017, 8:27 AM
 */

#ifndef MATRIXMATH_H
#define MATRIXMATH_H

typedef float Matrix4[16];

static void Matrix4Perspective(float *M, float fovy, float aspect, float znear, float zfar);
void Matrix4Rotation(Matrix4 M, float x, float y, float z);
void Matrix4Translation(Matrix4 M, float x, float y, float z);
void Matrix4Mul(Matrix4 M, Matrix4 A, Matrix4 B);

#endif /* MATRIXMATH_H */

