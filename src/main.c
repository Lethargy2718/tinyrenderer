#include <math.h>
#include <stdlib.h>

#include "tgaimage.h"
#include "wireframe.h"

#define OBJ_PATH "../obj/diablo3_pose.obj"  // TODO: argv
#define IMG_PATH "framebuffer.tga"          // TODO: argv

int main(void)
{
    const int width = 1200;
    const int height = 1200;

    const TGAColor white  = tga_color(255, 255, 255, 255, TGA_RGB);
    const TGAColor green  = tga_color(  0, 255,   0, 255, TGA_RGB);
    const TGAColor red    = tga_color(  0,   0, 255, 255, TGA_RGB);
    const TGAColor blue   = tga_color(255, 128,  64, 255, TGA_RGB);
    const TGAColor yellow = tga_color(  0, 200, 255, 255, TGA_RGB);
    const TGAColor black  = tga_color(  0,   0,   0, 255, TGA_RGB);

    TGAImage framebuffer;

    if (!tga_image_init(&framebuffer, width, height, TGA_RGB, black))
        return 1;

    Model model = {0};
    model_load(&model, OBJ_PATH);
    draw_wireframe(&model, &framebuffer, red, yellow);

    tga_write_file(
        &framebuffer,
        IMG_PATH,
        true,
        false
    );

    tga_image_free(&framebuffer);
    model_free(&model);

    return 0;
}