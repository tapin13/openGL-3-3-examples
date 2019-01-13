#include "bmp_load.h"

char bmp_load(const char *filename, uint8_t **image_data, uint32_t *texture_width, uint32_t *texture_height) {
    uint8_t *data = NULL;
    uint32_t size;
    
    FILE *fp;
    
    fp = fopen(filename, "rb");
    if(fp == NULL) {
        printf("Failed to open %s\n", filename);
        return 0;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    //printf("bmp size: %d\n", size);
    
    rewind(fp);
    data = malloc(size * sizeof(uint8_t));
    
    fread(data, 1, size, fp);
    fclose(fp);
   
    bitmap *header;
    header = (bitmap*)data;

//    printf("file signature %c%c\n", header->fileheader.signature[0], header->fileheader.signature[1]);
//    printf("file filesize %d\n", header->fileheader.filesize);
//    printf("file reserved %d\n", header->fileheader.reserved);
//    printf("file fileoffset_to_pixelarray %d\n", header->fileheader.fileoffset_to_pixelarray);
//    
//    printf("file dibheadersize %d\n", header->bitmapinfoheader.dibheadersize);
//    printf("file width %d\n", header->bitmapinfoheader.width);
//    printf("file height %d\n", header->bitmapinfoheader.height);
//    printf("file planes %d\n", header->bitmapinfoheader.planes);
//    printf("file bitsperpixel %d\n", header->bitmapinfoheader.bitsperpixel);
//    printf("file compression %d\n", header->bitmapinfoheader.compression);
//    printf("file imagesize %d\n", header->bitmapinfoheader.imagesize);
//    printf("file ypixelpermeter %d\n", header->bitmapinfoheader.ypixelpermeter);
//    printf("file xpixelpermeter %d\n", header->bitmapinfoheader.xpixelpermeter);
//    printf("file numcolorspallette %d\n", header->bitmapinfoheader.numcolorspallette);
//    printf("file mostimpcolor %d\n", header->bitmapinfoheader.mostimpcolor);
    
    *texture_width = header->bitmapinfoheader.width;
    *texture_height = header->bitmapinfoheader.height;

    int bits_per_line  = *texture_width * 3;
    int bmp_line = bits_per_line + *texture_width % 4; 
    
    *image_data = (uint8_t*) malloc(*texture_width * *texture_height * 3); // 24 bit per pixel -> 3 colors - 8 bit per pixel
    
    int i = 0;
    for(i = 0; i < *texture_height; i++) {
        memcpy(
                *image_data + (i * bits_per_line), 
                data + header->fileheader.fileoffset_to_pixelarray + (i * bmp_line), 
                bits_per_line
            );
    }
    
    return 1;
}