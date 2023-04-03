#include "lua.h"
#include "lauxlib.h"

int add_numbers(lua_State *L) {
    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);
    lua_pushnumber(L, a + b);
    return 1;
}

static const struct luaL_Reg ftclient[] = {
        {"add_numbers", add_numbers},
        {NULL, NULL}
};

#ifdef _WIN32
__declspec(dllexport)
#endif
int luaopen_ftclient(lua_State *L) {
    luaL_newlib(L, ftclient);
    return 1;
}