#include <math.h>
#include <stdlib.h>

#include "tgaimage.h"

const char* filename = "framebuffer.tga";

void line(int ax, int ay, int bx, int by, TGAImage *framebuffer, TGAColor color)
{
    int dx = abs(bx - ax);
    int sx = (ax < bx) ? 1 : -1;

    int dy = -abs(by - ay);
    int sy = (ay < by) ? 1 : -1;

    int err = dx + dy;

    while (1)
    {
        tga_set(framebuffer, ax, ay, color);

        if (ax == bx && ay == by)
            break;

        int e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;
            ax += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            ay += sy;
        }
    }
}

int main(void)
{
    const int width = 64;
    const int height = 64;

    const TGAColor white  = tga_color(255, 255, 255, 255, TGA_RGB);
    const TGAColor green  = tga_color(  0, 255,   0, 255, TGA_RGB);
    const TGAColor red    = tga_color(  0,   0, 255, 255, TGA_RGB);
    const TGAColor blue   = tga_color(255, 128,  64, 255, TGA_RGB);
    const TGAColor yellow = tga_color(  0, 200, 255, 255, TGA_RGB);
    const TGAColor black  = tga_color(  0,   0,   0, 255, TGA_RGB);

    TGAImage framebuffer;

    if (!tga_image_init(&framebuffer, width, height, TGA_RGB, black))
        return 1;

    int ax = 7,  ay = 3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    line(ax, ay, bx, by, &framebuffer, blue);
    line(cx, cy, bx, by, &framebuffer, green);
    line(cx, cy, ax, ay, &framebuffer, yellow);
    line(ax, ay, cx, cy, &framebuffer, red);

    tga_set(&framebuffer, ax, ay, white);
    tga_set(&framebuffer, bx, by, white);
    tga_set(&framebuffer, cx, cy, white);

    tga_write_file(
        &framebuffer,
        filename,
        true,
        false
    );

    tga_image_free(&framebuffer);

    return 0;
}