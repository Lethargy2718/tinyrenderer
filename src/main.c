#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "tgaimage.h"
#include "zbuffer.h"
#include "render.h"
#include "triangle.h"

#define OBJ_PATH "../assets/models/diablo3_pose.obj"    // TODO: argv
#define IMG_PATH "framebuffer.tga"                      // TODO: argv

int main(void)
{
    srand((unsigned)time(NULL));

    const int width = 1200;
    const int height = 1200;

    const TGAColor white  = tga_color(255, 255, 255, 255, TGA_RGB);
    const TGAColor green  = tga_color(  0, 255,   0, 255, TGA_RGB);
    const TGAColor red    = tga_color(  0,   0, 50, 255, TGA_RGB);
    const TGAColor blue   = tga_color(255, 128,  64, 255, TGA_RGB);
    const TGAColor yellow = tga_color(  0, 200, 255, 255, TGA_RGB);
    const TGAColor black  = tga_color(  0,   0,   0, 255, TGA_RGB);

    TGAImage framebuffer;

    if (!tga_image_init(&framebuffer, width, height, TGA_RGB, black))
        return 1;

    ZBuffer zbuffer;

    if (!zbuffer_init(&zbuffer, width, height)) {
        return 1;
    }

    Model model = {0};
    model_load(&model, OBJ_PATH);
    draw_gradient_background(&framebuffer, black, red);
    draw_filled(&model, &framebuffer, &zbuffer, draw_triangle_aabb, inside_hollow, 0.2f);
    
    // draw_wireframe(&model, &framebuffer, red);

    // Vec3 v0 = {100, 100, 0};
    // Vec3 v1 = {200, 100, 0};
    // Vec3 v2 = {100, 200, 0};

    // Triangle t = {v0, v1, v2};
    // draw_triangle_aabb(&framebuffer, red, t);


    tga_write_file(
        &framebuffer,
        IMG_PATH,
        true,
        false
    );

    tga_image_free(&framebuffer);
    zbuffer_free(&zbuffer);
    model_free(&model);

    return 0;
}