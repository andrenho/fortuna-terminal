#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#include <ftclient.h>
#include <ftclient-png.h>
#include <ftclient-tcpip.h>

#define FT_MT "FTClient.metadata"

static int ft_destroy(lua_State* L)
{
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT);
    ftclient_finalize(ft);
    return 0;
}

static int palette(lua_State* L)
{
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT);
    lua_pushstring(L, "");

    FTColor colors[FT_N_COLORS];
    memset(colors, 0, sizeof(colors));

    luaL_checktype(L, 2, LUA_TTABLE);
    size_t n_colors = lua_rawlen(L, 2);
    for (size_t i = 0; i < n_colors; ++i) {
        lua_geti(L, 2, i + 1);
        if (!lua_istable(L, -1))
            luaL_error(L, "`palette` must receive a table of tables (ex. { { r,g,b }, { r,g,b } })");
        lua_geti(L, -1, 1); if (!lua_isnil(L, -1)) { colors[i].r = lua_tonumber(L, -1); } lua_pop(L, 1);
        lua_geti(L, -1, 2); if (!lua_isnil(L, -1)) { colors[i].g = lua_tonumber(L, -1); } lua_pop(L, 1);
        lua_geti(L, -1, 3); if (!lua_isnil(L, -1)) { colors[i].b = lua_tonumber(L, -1); } lua_pop(L, 1);
        lua_pop(L, 1);
    }

    ft_palette(ft, colors);

    return 1;
}

static int map(lua_State* L)
{
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT);
    lua_pushstring(L, "");

    int16_t map_n = (int16_t) luaL_checkinteger(L, 2);
    int16_t w = (int16_t) luaL_checkinteger(L, 3);
    int16_t h = (int16_t) luaL_checkinteger(L, 4);
    luaL_checktype(L, 5, LUA_TTABLE);
    if (lua_rawlen(L, 5) != (size_t) (w * h))
        luaL_error(L, "Expected a table with %d elements.", w * h);

    int16_t indexes[w * h];
    for (int i = 0; i < (w * h); ++i) {
        lua_geti(L, 5, i + 1);
        indexes[i] = (int16_t) luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }

    ft_map(ft, map_n, w, h, indexes);

    return 1;
}

static int sprite(lua_State* L)
{
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT);
    lua_pushstring(L, "");

    size_t n_pars = lua_gettop(L);

    int16_t sprite_n = (int16_t) luaL_checkinteger(L, 2);
    int16_t pos_x = (int16_t) luaL_checkinteger(L, 3);
    int16_t pos_y = (int16_t) luaL_checkinteger(L, 4);
    if (n_pars == 4) {
        ft_sprite_0(ft, sprite_n, pos_x, pos_y);
    } else {
        bool visible = lua_toboolean(L, 5);
        if (n_pars == 5) {
            ft_sprite_1(ft, sprite_n, pos_x, pos_y, visible);
        } else {
            bool mirrored_h = lua_toboolean(L, 6);
            bool mirrored_v = lua_toboolean(L, 7);
            if (n_pars == 6 || n_pars == 7) {
                ft_sprite_2(ft, sprite_n, pos_x, pos_y, visible, mirrored_h, mirrored_v);
            } else {
                int16_t z_order = (int16_t) luaL_checkinteger(L, 8);
                if (n_pars == 8) {
                    ft_sprite_3(ft, sprite_n, pos_x, pos_y, visible, mirrored_h, mirrored_v, z_order);
                } else if (n_pars > 8) {
                    int16_t image_idx = (int16_t) luaL_checkinteger(L, 9);
                    ft_sprite_4(ft, sprite_n, pos_x, pos_y, visible, mirrored_h, mirrored_v, z_order, image_idx);
                }
            }
        }
    }

    return 1;
}

static int image(lua_State* L)
{
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT);
    lua_pushstring(L, "");

    luaL_checktype(L, 4, LUA_TTABLE);
    if (lua_rawlen(L, 4) != 256)
        luaL_error(L, "A 256-byte table is expected for the image data.");

    uint8_t image[256];
    for (int i = 0; i < 256; ++i) {
        lua_geti(L, 4, i + 1);
        image[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    ft_image(ft, (int16_t) luaL_checkinteger(L, 2), (int16_t) luaL_checkinteger(L, 3), image);

    return 1;
}

static int image_load(lua_State* L)
{
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT);
    lua_pushstring(L, "");

    char err_buf[1024];
    int r = ft_image_load(ft, luaL_checkstring(L, 2), err_buf, sizeof(err_buf));
    if (r < 0)
        luaL_error(L, "image_load: %s", err_buf);

    return 1;
}

#define FUNCTIONS \
    X(print,              { ft_print(ft, luaL_checkstring(L, 2)); }) \
    X(reset_terminal,     { ft_reset_terminal(ft); }) \
    X(reset_computer,     { ft_reset_computer(ft); }) \
    X(enable_vsync,       { ft_enable_vsync(ft, lua_toboolean(L, 2)); }) \
    X(request_version,    { ft_request_version(ft); })\
    X(mouse_enable,       { ft_mouse_enable(ft, lua_toboolean(L, 2)); }) \
    X(mouse_move_enable,  { ft_mouse_move_enable(ft, lua_toboolean(L, 2)); }) \
    X(joystick_emulation, { ft_joystick_emulation(ft, lua_toboolean(L, 2)); }) \
    X(graphics,           { ft_graphics(ft, lua_toboolean(L, 2)); }) \
    X(bg_color,           { ft_bg_color(ft, luaL_checkinteger(L, 2)); }) \
    X(enable_layer,       { ft_enable_layer(ft, luaL_checkinteger(L, 2), lua_toboolean(L, 3)); }) \
    X(map_pos,            { ft_map_pos(ft, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5)); }) \
    X(subscribe_collisions,       { ft_subscribe_collisions(ft, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)); }) \
    X(unsubscribe_collisions,     { ft_unsubscribe_collisions(ft, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)); }) \
    X(unsubscribe_all_collisions, { ft_unsubscribe_all_collisions(ft); })


#define X(name, code) \
static int name(lua_State* L) { \
    FTClient* ft = (FTClient *) luaL_checkudata(L, 1, FT_MT); \
    lua_pushstring(L, ""); \
    code \
    return 1;\
}
    FUNCTIONS
#undef X

static const struct luaL_Reg ft_meta[] = {
#define X(name, code) { #name, name },
    FUNCTIONS
#undef X
    {"palette", palette},
    {"map", map},
    {"sprite", sprite},
    {"image", image},
    {"image_load", image_load},
    {"__gc", ft_destroy},
    {NULL, NULL}
};

static int lua_write_cb(const char* buf, size_t bufsz, void* data)
{
    lua_State* L = (lua_State *) data;
    lua_pushlstring(L, buf, bufsz);
    lua_concat(L, 2);
    return 0;
}

static int ft_new(lua_State* L)
{
    FTClient* ft = (FTClient *) lua_newuserdatauv(L, sizeof(FTClient), 0);
    ftclient_init(ft, lua_write_cb, NULL, NULL, L, FT_RECOMMENDED_BUFSZ);

    luaL_setmetatable(L, FT_MT);

    return 1;
}

static int ft_tcpip(lua_State* L)
{
    FTClient* ft = (FTClient *) lua_newuserdatauv(L, sizeof(FTClient), 0);
    char error[1024];

    int port = 8076;
    if (lua_gettop(L) > 2)
        port = (int) luaL_checkinteger(L, 2);

    int r = ftclient_tcpip_init(ft, luaL_checkstring(L, 1), port, error, sizeof(error));
    if (r < 0)
        luaL_error(L, "tcpip: %s", error);

    luaL_setmetatable(L, FT_MT);

    return 1;
}

static const struct luaL_Reg ft_static[] = {
        {"new", ft_new},
        {"tcpip", ft_tcpip},
        {NULL, NULL}
};

int luaopen_ftclient(lua_State *L)
{
    // create metatable to be used with ft object
    luaL_newmetatable(L, FT_MT);
    luaL_setfuncs(L, ft_meta, 0);
    lua_setfield(L, -1, "__index");

    // create library with only the `new` function
    luaL_newlib(L, ft_static);

    return 1;
}