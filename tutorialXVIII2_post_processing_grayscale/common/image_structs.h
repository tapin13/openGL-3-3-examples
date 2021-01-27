#ifndef IMAGE_STRUCTS_H
#define IMAGE_STRUCTS_H

#include <GL/glew.h>

typedef struct {
    int32_t x;
    int32_t y;
    int32_t colors; // number of colors: RGB or RGBA aka image format of the pixel data
} imageMeta;

#endif /* IMAGE_STRUCTS_H */