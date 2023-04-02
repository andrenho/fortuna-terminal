#include "ftclient-png.h"

#include <png.h>
#include <stdlib.h>
#include <setjmp.h>

int ft_image_load(FTClient* ft, const char* filename, char* error, size_t err_sz)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        snprintf(error, err_sz, "Error opening PNG file.");
        return 1;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    setjmp(png_jmpbuf(png));
    png_init_io(png, fp);
    png_read_info(png, info);

    if (png_get_color_type(png, info) != PNG_COLOR_TYPE_PALETTE || png_get_bit_depth(png, info) != 8) {
        snprintf(error, err_sz, "Error: only 8-bit color indexing is supported\n");
        return 1;
    }

    size_t w = png_get_image_width(png, info);
    size_t h = png_get_image_height(png, info);
    if (w % 16 != 0 || h % 16 != 0) {
        snprintf(error, err_sz, "Image width and height need to be a multiple of 16.");
        return 1;
    }

    uint8_t pixels[h][w];
    png_read_image(png, pixels);

    /*
    for (int y = 0; y < png_get_image_height(png, info); ++y) {
        for (int x = 0; x < png_get_image_width(png, info); ++x) {
            printf("%d", row_pointers[y][x]);
            if (x != png_get_image_width(png, info) - 1 || y != png_get_image_height(png, info) - 1) {
                printf(";");
            }
        }
    }

    for (int y = 0; y < png_get_image_height(png, info); ++y) {
        free(row_pointers[y]);
    }
     */
    fclose(fp);

    return 0;
}