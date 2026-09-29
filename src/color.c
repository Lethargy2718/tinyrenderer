#include <stdlib.h>
#include <time.h>

#include "tgaimage.h"

TGAColor random_color(void) {
    return tga_color(rand() % 256, rand() % 256, rand() % 256, 255, TGA_RGB);
}