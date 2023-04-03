#include "ftclient-png.h"

#include <png.h>
#include <stdlib.h>
#include <setjmp.h>

// TODO - close stuff on error

int write_request(FTClient* ft, char cmd, int16_t* array, size_t array_sz);

int ft_image_load(FTClient* ft, const char* filename, char* error, size_t err_sz)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        snprintf(error, err_sz, "Error opening PNG file.");
        return -1;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        snprintf(error, err_sz, "Failed to create read struct.");
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        snprintf(error, err_sz, "Failed to create info struct.");
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        snprintf(error, err_sz, "Error during PNG read.");
        return -1;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    if (png_get_color_type(png, info) != PNG_COLOR_TYPE_PALETTE || png_get_bit_depth(png, info) != 8) {
        snprintf(error, err_sz, "Error: only 8-bit color indexing is supported\n");
        return -1;
    }

    size_t w = png_get_image_width(png, info);
    size_t h = png_get_image_height(png, info);
    if (w % 16 != 0 || h % 16 != 0) {
        snprintf(error, err_sz, "Image width and height need to be a multiple of 16.");
        return -1;
    }

    uint8_t* pixels[h];
    for (size_t y = 0; y < h; ++y)
        pixels[y] = malloc(png_get_rowbytes(png, info));

    png_read_image(png, pixels);

    int16_t transparent_index = -1;
    {
        png_colorp palette;
        int num_palette;
        png_get_PLTE(png, info, &palette, &num_palette);

        png_bytep trans;
        int num_trans;
        png_get_tRNS(png, info, &trans, &num_trans, NULL);

        if (num_trans > 0) {
            for (int16_t i = 0; i < num_palette; ++i) {
                if (trans[i] == 0) {
                    transparent_index = i;
                    break;
                }
            }
        }
    }

    int16_t image_n = 0;

    for (size_t xx = 0; xx < w; xx += 16) {
        for (size_t yy = 0; yy < h; yy += 16) {

            int16_t array[258];
            size_t i = 0;
            array[i++] = image_n++;
            array[i++] = transparent_index;

            for (size_t x = 0; x < 16; ++x)
                for (size_t y = 0; y < 16; ++y)
                    array[i++] = pixels[yy + y][xx + x];

            write_request(ft, 'i', array, 258);
        }
    }

    fclose(fp);

    return 0;
}