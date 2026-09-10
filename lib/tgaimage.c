#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgaimage.h"

static bool load_rle_data(TGAImage *img, FILE *in);
static bool unload_rle_data(const TGAImage *img, FILE *out);

bool tga_image_init(TGAImage *img, int w, int h, int bpp, TGAColor c) {
    img->w = w;
    img->h = h;
    img->bpp = (uint8_t)bpp;
    img->data = calloc((size_t)w * h * bpp, 1);
    if (!img->data) return false;
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            tga_set(img, i, j, c);
    return true;
}

void tga_image_free(TGAImage *img) {
    free(img->data);
    img->data = NULL;
    img->w = img->h = 0;
    img->bpp = 0;
}

bool tga_read_file(TGAImage *img, const char *filename) {
    FILE *in = fopen(filename, "rb");
    if (!in) {
        fprintf(stderr, "can't open file %s\n", filename);
        return false;
    }

    TGAHeader header;
    if (fread(&header, sizeof(header), 1, in) != 1) {
        fprintf(stderr, "an error occured while reading the header\n");
        fclose(in);
        return false;
    }

    int w = header.width;
    int h = header.height;
    uint8_t bpp = (uint8_t)(header.bitsperpixel >> 3);
    if (w <= 0 || h <= 0 || (bpp != TGA_GRAYSCALE && bpp != TGA_RGB && bpp != TGA_RGBA)) {
        fprintf(stderr, "bad bpp (or width/height) value\n");
        fclose(in);
        return false;
    }

    tga_image_free(img); /* drop whatever this TGAImage held before */
    size_t nbytes = (size_t)bpp * w * h;
    img->data = calloc(nbytes, 1);
    if (!img->data) {
        fclose(in);
        return false;
    }
    img->w = w;
    img->h = h;
    img->bpp = bpp;

    if (header.datatypecode == 3 || header.datatypecode == 2) {
        if (fread(img->data, 1, nbytes, in) != nbytes) {
            fprintf(stderr, "an error occured while reading the data\n");
            fclose(in);
            return false;
        }
    } else if (header.datatypecode == 10 || header.datatypecode == 11) {
        if (!load_rle_data(img, in)) {
            fprintf(stderr, "an error occured while reading the data\n");
            fclose(in);
            return false;
        }
    } else {
        fprintf(stderr, "unknown file format %d\n", header.datatypecode);
        fclose(in);
        return false;
    }
    fclose(in);

    if (!(header.imagedescriptor & 0x20))
        tga_flip_vertically(img);
    if (header.imagedescriptor & 0x10)
        tga_flip_horizontally(img);

    fprintf(stderr, "%dx%d/%d\n", img->w, img->h, img->bpp * 8);
    return true;
}

static bool load_rle_data(TGAImage *img, FILE *in) {
    int bpp = img->bpp;
    size_t pixelcount = (size_t)img->w * img->h;
    size_t currentpixel = 0, currentbyte = 0;
    uint8_t colorbuffer[4];

    do {
        int chunkheader = fgetc(in);
        if (chunkheader == EOF) {
            fprintf(stderr, "an error occured while reading the data\n");
            return false;
        }
        if (chunkheader < 128) {
            chunkheader++;
            for (int i = 0; i < chunkheader; i++) {
                if (fread(colorbuffer, 1, (size_t)bpp, in) != (size_t)bpp) {
                    fprintf(stderr, "an error occured while reading the header\n");
                    return false;
                }
                for (int t = 0; t < bpp; t++)
                    img->data[currentbyte++] = colorbuffer[t];
                currentpixel++;
                if (currentpixel > pixelcount) {
                    fprintf(stderr, "Too many pixels read\n");
                    return false;
                }
            }
        } else {
            chunkheader -= 127;
            if (fread(colorbuffer, 1, (size_t)bpp, in) != (size_t)bpp) {
                fprintf(stderr, "an error occured while reading the header\n");
                return false;
            }
            for (int i = 0; i < chunkheader; i++) {
                for (int t = 0; t < bpp; t++)
                    img->data[currentbyte++] = colorbuffer[t];
                currentpixel++;
                if (currentpixel > pixelcount) {
                    fprintf(stderr, "Too many pixels read\n");
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return true;
}

bool tga_write_file(const TGAImage *img, const char *filename, bool vflip, bool rle) {
    static const uint8_t developer_area_ref[4] = {0, 0, 0, 0};
    static const uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    static const uint8_t footer[18] = "TRUEVISION-XFILE.";

    FILE *out = fopen(filename, "wb");
    if (!out) {
        fprintf(stderr, "can't open file %s\n", filename);
        return false;
    }

    TGAHeader header = {0};
    header.bitsperpixel = (uint8_t)(img->bpp << 3);
    header.width = (uint16_t)img->w;
    header.height = (uint16_t)img->h;
    header.datatypecode = (img->bpp == TGA_GRAYSCALE) ? (rle ? 11 : 3) : (rle ? 10 : 2);
    header.imagedescriptor = vflip ? 0x00 : 0x20;

    if (fwrite(&header, sizeof(header), 1, out) != 1) goto err;

    if (!rle) {
        size_t nbytes = (size_t)img->w * img->h * img->bpp;
        if (fwrite(img->data, 1, nbytes, out) != nbytes) goto err;
    } else if (!unload_rle_data(img, out)) {
        goto err;
    }

    if (fwrite(developer_area_ref, sizeof(developer_area_ref), 1, out) != 1) goto err;
    if (fwrite(extension_area_ref, sizeof(extension_area_ref), 1, out) != 1) goto err;
    if (fwrite(footer, sizeof(footer), 1, out) != 1) goto err;

    fclose(out);
    return true;

err:
    fprintf(stderr, "can't dump the tga file\n");
    fclose(out);
    return false;
}

static bool unload_rle_data(const TGAImage *img, FILE *out) {
    const int max_chunk_length = 128;
    int bpp = img->bpp;
    size_t npixels = (size_t)img->w * img->h;
    size_t curpix = 0;

    while (curpix < npixels) {
        size_t chunkstart = curpix * bpp;
        size_t curbyte = curpix * bpp;
        int run_length = 1;
        bool raw = true;

        while (curpix + run_length < npixels && run_length < max_chunk_length) {
            bool succ_eq = true;
            for (int t = 0; succ_eq && t < bpp; t++)
                succ_eq = (img->data[curbyte + t] == img->data[curbyte + t + bpp]);
            curbyte += bpp;
            if (run_length == 1)
                raw = !succ_eq;
            if (raw && succ_eq) {
                run_length--;
                break;
            }
            if (!raw && !succ_eq)
                break;
            run_length++;
        }
        curpix += run_length;
        if (fputc(raw ? run_length - 1 : run_length + 127, out) == EOF) return false;
        size_t nbytes = raw ? (size_t)run_length * bpp : (size_t)bpp;
        if (fwrite(img->data + chunkstart, 1, nbytes, out) != nbytes) return false;
    }
    return true;
}

TGAColor tga_get(const TGAImage *img, int x, int y) {
    TGAColor ret = {0};
    ret.bytespp = img->bpp;
    if (!img->data || x < 0 || y < 0 || x >= img->w || y >= img->h) return ret;
    const uint8_t *p = img->data + (size_t)(x + y * img->w) * img->bpp;
    memcpy(ret.bgra, p, img->bpp);
    return ret;
}

void tga_set(TGAImage *img, int x, int y, TGAColor c) {
    if (!img->data || x < 0 || y < 0 || x >= img->w || y >= img->h) return;
    memcpy(img->data + (size_t)(x + y * img->w) * img->bpp, c.bgra, img->bpp);
}

void tga_flip_horizontally(TGAImage *img) {
    int w = img->w, h = img->h, bpp = img->bpp;
    for (int i = 0; i < w / 2; i++)
        for (int j = 0; j < h; j++)
            for (int b = 0; b < bpp; b++) {
                uint8_t *a = &img->data[(size_t)(i + j * w) * bpp + b];
                uint8_t *c = &img->data[(size_t)(w - 1 - i + j * w) * bpp + b];
                uint8_t tmp = *a; *a = *c; *c = tmp;
            }
}

void tga_flip_vertically(TGAImage *img) {
    int w = img->w, h = img->h, bpp = img->bpp;
    for (int i = 0; i < w; i++)
        for (int j = 0; j < h / 2; j++)
            for (int b = 0; b < bpp; b++) {
                uint8_t *a = &img->data[(size_t)(i + j * w) * bpp + b];
                uint8_t *c = &img->data[(size_t)(i + (h - 1 - j) * w) * bpp + b];
                uint8_t tmp = *a; *a = *c; *c = tmp;
            }
}

int tga_width(const TGAImage *img)  { return img->w; }
int tga_height(const TGAImage *img) { return img->h; }