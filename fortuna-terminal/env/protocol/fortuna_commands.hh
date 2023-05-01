#ifndef FORTUNA_COMMANDS_HH_
#define FORUNTA_COMMANDS_HH_

enum InputCommand : int {
    // configuration
    I_RESET_TERMINAL     = 1,
    I_RESET_COMPUTER     = 2,
    I_ENABLE_VSYNC       = 3,
    I_DISABLE_VSYNC      = 4,
    I_TERMINAL_VERSION   = 5,

    // input
    I_MOUSE_ENABLE       = 6,
    I_MOUSE_DISABLE      = 7,
    I_MOUSEMOVE_ENABLE   = 8,
    I_MOUSEMOVE_DISABLE  = 9,
    I_JOYEMU_ENABLE      = 10,
    I_JOYEMU_DISABLE     = 11,

    // graphics
    I_GRAPHICS_DISABLE   = 12,
    I_CHANGE_PALETTE     = 13,
    I_UPLOAD_IMAGE       = 14,
    I_UPLOAD_MAP         = 15,
    I_SET_BG             = 16,
    I_ENABLE_LAYER       = 17,
    I_DISABLE_LAYER      = 18,
    I_CONFIG_TILEMAP     = 19,

    // sprites
    I_SPRITE_IMAGE       = 20,
    I_SPRITE_POSITION    = 21,
    I_SPRITE_VISIBLE     = 22,
    I_SPRITE_INVISIBLE   = 23,
    I_SPRITE_HMIRROR     = 24,
    I_SPRITE_NOT_HMIRROR = 25,
    I_SPRITE_VMIRROR     = 26,
    I_SPRITE_NOT_VMIRROR = 27,
    I_SPRITE_ZORDER      = 28,
    I_SPRITE_LOCK        = 29,

    // text
    I_TEXT               = 30,
    I_TEXT_COLOR         = 31,
    I_CLEAR_TXT_SCR      = 32,

    // collisions
    I_COLL_SUBSCRIBE     = 33,
    I_COLL_UNSUBSCRIBE   = 34,
    I_COLL_UNSUBSCRIBE_ALL = 35,
};

enum OutputEvent : int {
    E_VERSION            = 1,
    E_MOUSE_PRESSED      = 2,
    E_MOUSE_RELEASED     = 3,
    E_MOUSE_MOVED        = 4,
    E_JOY_PRESS          = 5,
    E_JOY_RELEASE        = 6,
    E_MAP_CHECKSUM       = 7,
    E_IMAGE_CHECKSUM     = 8,
    E_COLLISION          = 9,
    E_SEPARATION         = 10,
    E_VSYNC              = 127,
};

#endif