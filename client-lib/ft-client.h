#ifndef FT_CLIENT_H_
#define FT_CLIENT_H_

#include <stdbool.h>
#include <stdint.h>

#define FT_N_COLORS 32

typedef struct FTClient {
    int    (*write_cb)(const char* buf, size_t bufsz, void* data);
    int    (*read_cb)(char* buf, size_t bufsz, void* data);
    void*  data;
    size_t bufsz;
} FTClient;

typedef struct FTColor {
    uint8_t r, g, b;
} FTColor;

#define FT_RECOMMENDED_BUFSZ 1024

int ftclient_init(FTClient* ft_client,
                  int (*write_cb)(const char* buf, size_t bufsz, void* data),
                  int (*read_cb)(char* buf, size_t bufsz, void* data),
                  void* data,
                  size_t bufsz);  // use FT_RECOMMENDED_BUFSZ, except for microcontrollers

int ft_print(FTClient* ft, const char* fmt, ...);

int ft_reset_terminal(FTClient* ft);
int ft_reset_computer(FTClient* ft);
int ft_enable_vsync(FTClient* ft, bool enable);
int ft_request_version(FTClient* ft);

int ft_mouse_enable(FTClient* ft, bool enable);
int ft_mouse_move_enable(FTClient* ft, bool enable);
int ft_joystick_emulation(FTClient* ft, bool enable);

int ft_graphics(FTClient* ft, bool enable);
int ft_palette(FTClient* ft, FTColor colors[FT_N_COLORS]);
int ft_image(FTClient* ft, int16_t index, int16_t transparent_color, const uint8_t bytes[256]);
int ft_map(FTClient* ft, int16_t map_n, int16_t width, int16_t height, int16_t const* indexes);

int ft_bg_color(FTClient* ft, int16_t color);
int ft_enable_layer(FTClient* ft, int16_t layer, bool enable);
int ft_map_pos(FTClient* ft, int16_t layer, int16_t map, int16_t pos_x, int16_t pos_y);

#endif //FT_CLIENT_H_
