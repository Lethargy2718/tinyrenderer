#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#pragma pack(push,1)
typedef struct {
    uint8_t  idlength;
    uint8_t  colormaptype;
    uint8_t  datatypecode;
    uint16_t colormaporigin;
    uint16_t colormaplength;
    uint8_t  colormapdepth;
    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t width;
    uint16_t height;
    uint8_t  bitsperpixel;
    uint8_t  imagedescriptor;
} TGAHeader;
#pragma pack(pop)

typedef struct {
    uint8_t bgra[4];
    uint8_t bytespp;
} TGAColor;

enum { TGA_GRAYSCALE = 1, TGA_RGB = 3, TGA_RGBA = 4 };

typedef struct {
    int w, h;
    uint8_t bpp;
    uint8_t *data; /* w*h*bpp bytes, owned by this struct, or NULL */
} TGAImage;

static inline TGAColor tga_color(uint8_t b, uint8_t g, uint8_t r, uint8_t a, uint8_t bytespp) {
    TGAColor c;
    c.bgra[0] = b; c.bgra[1] = g; c.bgra[2] = r; c.bgra[3] = a;
    c.bytespp = bytespp;
    return c;
}

bool tga_image_init(TGAImage *img, int w, int h, int bpp, TGAColor c);
void tga_image_free(TGAImage *img);

bool tga_read_file(TGAImage *img, const char *filename);
bool tga_write_file(const TGAImage *img, const char *filename, bool vflip, bool rle);

void tga_flip_horizontally(TGAImage *img);
void tga_flip_vertically(TGAImage *img);

TGAColor tga_get(const TGAImage *img, int x, int y);
void tga_set(TGAImage *img, int x, int y, TGAColor c);

int tga_width(const TGAImage *img);
int tga_height(const TGAImage *img);