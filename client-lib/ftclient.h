#ifndef FTCLIENT_H_
#define FTCLIENT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define FT_N_COLORS 32

typedef struct FTClient {
    int    (*write_cb)(const char* buf, size_t bufsz, void* data);
    int    (*read_cb)(char* buf, size_t bufsz, void* data);
    int    (*finalize)(struct FTClient* ft, void* data);
    void*  data;
    size_t bufsz;
} FTClient;

typedef struct FTColor {
    uint8_t r, g, b;
} FTColor;

typedef enum FT_EventType {
    FTE_KEY_PRESS, FTE_MOUSE_PRESS, FTE_MOUSE_RELEASE, FTE_MOUSE_MOVE,
    FTE_JOY_PRESS, FTE_JOY_RELEASE, FTE_VERSION, FTE_COLLISION, FTE_VSYNC,
    FTE_INVALID_ESCAPE,
} FT_EventType;

typedef struct FTE_Mouse {
    int16_t button;
    int16_t pos_x;
    int16_t pos_y;
} FTE_Mouse;

typedef struct FTE_Version {
    int16_t major;
    int16_t minor;
    int16_t patch;
} FTE_Version;

typedef enum FTE_CollisionType {
    FTEC_COLLISION, FTEC_SEPARATION,
} FTE_CollisionType;

typedef struct FTE_Collision {
    FTE_CollisionType type;
    int16_t           sprite_a;
    int16_t           sprite_b;
} FTE_Collision;

typedef struct FT_Event {
    FT_EventType type;
    union {
        char          key;
        FTE_Mouse     mouse;
        FTE_Version   version;
        int16_t       joy;
        FTE_Collision collision;
    };
} FT_Event;

#define FT_RECOMMENDED_BUFSZ 1024

int ftclient_init(FTClient* ft_client,
                  int (*write_cb)(const char* buf, size_t bufsz, void* data),
                  int (*read_cb)(char* buf, size_t bufsz, void* data),
                  int (*finalize)(struct FTClient* ft, void* data),
                  void* data,
                  size_t bufsz);  // use FT_RECOMMENDED_BUFSZ, except for microcontrollers
int ftclient_finalize(FTClient* ft);

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
int ft_map(FTClient* ft, int16_t map_n, int16_t width, int16_t height, int16_t const* indexes);

int ft_bg_color(FTClient* ft, int16_t color);
int ft_enable_layer(FTClient* ft, int16_t layer, bool enable);
int ft_map_pos(FTClient* ft, int16_t layer, int16_t map, int16_t pos_x, int16_t pos_y);

int ft_sprite_0(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y);
int ft_sprite_1(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible);
int ft_sprite_2(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible, bool mirrored_h, bool mirrored_v);
int ft_sprite_3(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible, bool mirrored_h, bool mirrored_v, int16_t z_order);
int ft_sprite_4(FTClient* ft, int16_t sprite_n, int16_t pos_x, int16_t pos_y, bool visible, bool mirrored_h, bool mirrored_v, int16_t z_order, int16_t image_idx);

int ft_image(FTClient* ft, int16_t index, int16_t transparent_color, const uint8_t bytes[256]);

int ft_subscribe_collisions(FTClient* ft, int16_t sprite_a, int16_t sprite_b);
int ft_unsubscribe_collisions(FTClient* ft, int16_t sprite_a, int16_t sprite_b);
int ft_unsubscribe_all_collisions(FTClient* ft);

int ft_poll_event(FTClient* ft, FT_Event* event);

#endif //FTCLIENT_H_
