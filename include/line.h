#pragma once

#include <math.h>

#include "tgaimage.h"
#include "vec3.h"

void draw_line(Vec3 v1, Vec3 v2, TGAImage *framebuffer, TGAColor color)
{
    int ax = v1.x, ay = v1.y;
    int bx = v2.x, by = v2.y;

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