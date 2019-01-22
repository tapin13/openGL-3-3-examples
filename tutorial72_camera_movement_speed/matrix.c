#include "matrix.h"

float deg2rad(float angle) { 
    return angle * (M_PI / 180.0f); 
}

void vector_normalize(float *vector) {
    float magnitude = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    
    if(magnitude == 0.0) {
        vector[0] = 0.0;
        vector[1] = 0.0;
        vector[2] = 0.0;
        
        return;
    }
    
    vector[0] /= magnitude;
    vector[1] /= magnitude;
    vector[2] /= magnitude;   
}

void vector_cross(float *vector, float *vector2) {
    float vector_result[3] = {
        vector[1] * vector2[2] - vector[2] * vector2[1],
        vector[2] * vector2[0] - vector[0] * vector2[2],
        vector[0] * vector2[1] - vector[1] * vector2[0],
    };
        
    vector[0] = vector_result[0];
    vector[1] = vector_result[1];
    vector[2] = vector_result[2];
}

void matrix_print(GLfloat *matrix) {
    int i, j;    
    printf("-------\n");
    for(j = 0; j < 4; j++) {
        for(i = 0; i < 4; i++) {
            printf("%0.2f ", (float)matrix[j * 4 + i]);
        }
        printf("\n");
    }
    printf("-------\n");    
}

void matrix_identity(GLfloat *matrix) {
    int i = 0;
    
    for(i = 0; i < 4 * 4; i++) {
        matrix[i] = 0.0f;
    }
    
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}

void matrix_translate(GLfloat *matrix, GLfloat *position) {
    matrix[0 * 4 + 3] = position[0];
    matrix[1 * 4 + 3] = position[1];
    matrix[2 * 4 + 3] = position[2];
}

void matrix_scale(float scale, GLfloat *matrix) {
    matrix[0] *= scale;
    matrix[1] *= scale;
    matrix[4] *= scale;
    matrix[5] *= scale;
}

void matrix_perspective(float angle, float ratio, float near, float far, GLfloat *matrix) {
    float tan_half_angle = tan(deg2rad(angle) / 2.0f);
    
    matrix[0 * 4 + 0] = 1.0f / (ratio * tan_half_angle);
    matrix[1 * 4 + 1] = 1.0f / tan_half_angle;
    matrix[2 * 4 + 2] = -(far + near) / (far - near);
    matrix[2 * 4 + 3] = -2 * far * near / (far - near);
    matrix[3 * 4 + 2] = -1.0f;
}

void matrix_rotate(GLfloat *matrix, float angle, float *axis) {
    angle = deg2rad(angle);

    float		sinus = sin(angle);
    float		cosinus = cos(angle);

    vector_normalize(axis);

    matrix[0 + 0] = cosinus + axis[0]*axis[0]*(1-cosinus);
    matrix[0 + 1] = axis[0]*axis[1]*(1-cosinus) - axis[2]*sinus;
    matrix[0 + 2] = axis[0]*axis[2]*(1-cosinus) + axis[1]*sinus;
    matrix[1 * 4 + 0] = axis[0]*axis[1]*(1-cosinus) + axis[2]*sinus;
    matrix[1 * 4 + 1] = cosinus + axis[1]*axis[1]*(1-cosinus);
    matrix[1 * 4 + 2] = axis[1]*axis[2]*(1-cosinus) - axis[0]*sinus;
    matrix[2 * 4 + 0] = axis[0]*axis[2]*(1-cosinus) - axis[1]*sinus;
    matrix[2 * 4 + 1] = axis[1]*axis[2]*(1-cosinus) + axis[0]*sinus;
    matrix[2 * 4 + 2] = cosinus + axis[2]*axis[2]*(1-cosinus);
}

//void matrix_lootAt(GLfloat *matrix, GLfloat *cameraRight, GLfloat *cameraUp, GLfloat *cameraDirection, GLfloat *cameraPosition)
void matrix_lootAt(GLfloat *matrix, GLfloat *cameraPosition, GLfloat *cameraTarget, GLfloat *up) {
    GLfloat cameraDirection[3] = { 
        cameraPosition[0] - cameraTarget[0], 
        cameraPosition[1] - cameraTarget[1], 
        cameraPosition[2] - cameraTarget[2], 
    };
    vector_normalize(cameraDirection);    

    GLfloat cameraRight[3] = { up[0], up[1], up[2] };
    vector_cross(cameraRight, cameraDirection);
    vector_normalize(cameraRight);
    
    GLfloat cameraUp[3] = { cameraDirection[0], cameraDirection[1], cameraDirection[2] };
    vector_cross(cameraUp, cameraRight);
    
    matrix[4 * 0 + 0] = cameraRight[0];
    matrix[4 * 0 + 1] = cameraRight[1];
    matrix[4 * 0 + 2] = cameraRight[2];
    matrix[4 * 1 + 0] = cameraUp[0];
    matrix[4 * 1 + 1] = cameraUp[1];
    matrix[4 * 1 + 2] = cameraUp[2];
    matrix[4 * 2 + 0] = cameraDirection[0];
    matrix[4 * 2 + 1] = cameraDirection[1];
    matrix[4 * 2 + 2] = cameraDirection[2];
    
    matrix[4 * 0 + 3] = matrix[4 * 0 + 0] * -cameraPosition[0] + matrix[4 * 0 + 1] * -cameraPosition[1] + matrix[4 * 0 + 2] * -cameraPosition[2];
    matrix[4 * 1 + 3] = matrix[4 * 1 + 0] * -cameraPosition[0] + matrix[4 * 1 + 1] * -cameraPosition[1] + matrix[4 * 1 + 2] * -cameraPosition[2];
    matrix[4 * 2 + 3] = matrix[4 * 2 + 0] * -cameraPosition[0] + matrix[4 * 2 + 1] * -cameraPosition[1] + matrix[4 * 2 + 2] * -cameraPosition[2];
}