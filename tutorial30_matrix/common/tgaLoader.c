
struct TGAHeader {
    uint8_t idlength;
    uint8_t colormap;
    uint8_t datatype;
    uint8_t colormapinfo[5];
    uint16_t xoriginal;
    uint16_t yoriginal;
    uint16_t width;
    uint16_t height;
    uint8_t bitprepel;
    uint8_t description;
};

typedef struct TGAHeader TGAHeader;

GLuint TextureCreateFromTGA(const char *filename) {
    if(strlen(filename) == 0) {
        fprintf(stderr, "filename empty\n");
        return -1;
    }
    
    TGAHeader *header;
    uint8_t *buffer;
    uint32_t size;
    GLuint format, internalFormat;
    GLuint texture;
    
    FILE *fp;
    u_int32_t sizeOfFile, reader;
    
    fp = fopen(filename, "rb");
    if(fp == NULL) {
        fprintf(stderr, "can not open file: %s\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    sizeOfFile = (uint32_t)ftell(fp);
    rewind(fp);
    
    printf("--------------------------\n");
    printf("Size of file '%s': %u\n", filename, sizeOfFile);
    
    
    buffer = malloc(sizeOfFile * sizeof(uint8_t));
    
    fread(buffer, 1, sizeOfFile, fp);
    fclose(fp);

    if(sizeOfFile <= sizeof(TGAHeader)) {
        fprintf(stderr, "too small file\n");
        return -1;        
    }
    
    header = (TGAHeader*)buffer;
    
    printf("idlength: %u\n", (unsigned int)header->idlength);
    printf("colormap: %u\n", (unsigned int)header->colormap);
    printf("datatype: %u\n", (unsigned int)header->datatype);
    printf("xoriginal: %u\n", (unsigned int)header->xoriginal);
    printf("yoriginal: %u\n", (unsigned int)header->yoriginal);
    printf("width: %u\n", (unsigned int)header->width);
    printf("height: %u\n", (unsigned int)header->height);
    printf("bitprepel: %u\n", (unsigned int)header->bitprepel);
    printf("description: %u\n", (unsigned int)header->description);
    printf("--------------------------\n");
    
    if(header->datatype != 2 || (header->bitprepel != 24 && header->bitprepel != 32)) {
        fprintf(stderr, "wrong TGA format\n");
        return -1;
    }
    
    format = (header->bitprepel == 24 ? GL_RGB : GL_RGBA);
    internalFormat = (format == GL_RGB ? GL_RGB8 : GL_RGBA8);
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0
            , format, GL_UNSIGNED_BYTE, (const GLvoid*)(buffer + sizeof(TGAHeader) + header->idlength));
    
    free(buffer);
    
    return texture;
}