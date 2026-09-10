#include <stdio.h>
#include "tgaimage.h"

int main(void) {
    TGAImage img;
    if (!tga_image_init(&img, 100, 100, TGA_RGB, tga_color(0, 0, 0, 255, 3))) {
        fprintf(stderr, "failed to allocate framebuffer\n");
        return 1;
    }

    tga_set(&img, 50, 50, tga_color(255, 255, 255, 255, 3));

    if (!tga_write_file(&img, "output.tga", true, true)) {
        tga_image_free(&img);
        return 1;
    }

    tga_image_free(&img);
    printf("wrote output.tga\n");
    return 0;
}