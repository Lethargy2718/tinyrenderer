#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "tgaimage.h"
#include "zbuffer.h"
#include "render.h"
#include "triangle.h"
#include "transform.h"

#define OBJ_PATH "../assets/models/diablo3_pose.obj"    // TODO: argv
#define IMG_PATH "framebuffer.tga"                      // TODO: argv
#define ZBUF_PATH "zbuffer.tga"                         // TODO: argv

int main(void)
{
    srand((unsigned)time(NULL));

    // Screen size
    const int width = 800;
    const int height = 800;

    // Colors
    const TGAColor white  = tga_color(255, 255, 255, 255, TGA_RGB);
    const TGAColor green  = tga_color(  0, 255,   0, 255, TGA_RGB);
    const TGAColor red    = tga_color(  0,   0, 100, 255, TGA_RGB);
    const TGAColor blue   = tga_color(255, 128,  64, 255, TGA_RGB);
    const TGAColor yellow = tga_color(  0, 200, 255, 255, TGA_RGB);
    const TGAColor black  = tga_color(  0,   0,   0, 255, TGA_RGB);

    const TGAColor g1 = tga_color(25, 0, 0, 255, TGA_GRAYSCALE);
    const TGAColor g2 = tga_color(0, 0, 0, 255, TGA_GRAYSCALE);

    // Init buffers
    TGAImage framebuffer;
    if (!tga_image_init(&framebuffer, width, height, TGA_RGB, black))
        return 1;

    ZBuffer zbuffer;
    if (!zbuffer_init(&zbuffer, width, height)) {
        return 1;
    }

    TGAImage zimg;
    const TGAColor zero = tga_color(0, 0, 0, 0, TGA_GRAYSCALE);
    if (!tga_image_init(&zimg, width, height, TGA_GRAYSCALE, zero)) {
        return 1;
    }

    // Camera
    const Vec3 eye    = { 0, 0, 1 };
    const Vec3 center = { 0, 0, 0 };
    const Vec3 up     = { 0, 1, 0 };
    const double focal = 1.0;
    Camera cam = camera_make(eye, center, up, width, height, focal);

    // Model
    Model model = {0};
    model_load(&model, OBJ_PATH);

    Transform a = transform_identity();
    a.position = (Vec3){-0.4, 0, 0};
    a.scale    = (Vec3){0.7, 0.7, 0.7};

    Transform b = transform_identity();
    b.position   = (Vec3){0.4, 0, 0};
    b.rotation.y = DEG2RAD(180);
    b.scale      = (Vec3){0.7, 0.7, 0.7};

    // Draw
    draw_gradient_background(&framebuffer, black, red);
    draw_gradient_background(&zimg, g1, g2);

    draw_filled(&model, &framebuffer, &zbuffer, &cam, transform_matrix(&a), inside_default, 1.0f);
    draw_filled(&model, &framebuffer, &zbuffer, &cam, transform_matrix(&b), inside_default, 1.0f);
    // draw_wireframe(&model, &framebuffer, &cam, transform_matrix(&a), red);
    
    zbuffer_to_image(&zbuffer, &zimg);

    // Write files
    tga_write_file(
        &zimg,
        ZBUF_PATH,
        true,
        false
    );

    tga_write_file(
        &framebuffer,
        IMG_PATH,
        true,
        false
    );

    // Free memory
    tga_image_free(&framebuffer);
    tga_image_free(&zimg);
    zbuffer_free(&zbuffer);
    model_free(&model);

    return 0;
}