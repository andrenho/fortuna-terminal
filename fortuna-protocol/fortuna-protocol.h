#ifndef FORTUNA_PROTOCOL_H_
#define FORTUNA_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Documentation: https://github.com/andrenho/fortuna-terminal/wiki/Fortuna-Protocol

// REQUEST

typedef enum __attribute__((packed)) {
    FP_REQ_QUERY   = 0x1,
    FP_REQ_REPEAT  = 0x2,
    FP_REQ_EVENT   = 0x3,
    FP_REQ_VSYNC   = 0x4,
} FP_Request;

/********************
 *                  *
 *     MESSAGE      *
 *                  *
 ********************/

#define PALETTE_SZ 16

#define FP_MSG_CONTENTS_SZ 43

typedef enum __attribute__((packed)) {

    //
    // GENERIC COMMANDS
    //

    FP_RESET                        = 0x01, // pulls the RESET line down, effectively resetting the computer
    FP_GRAPHICAL_MODE               = 0x02, // sets the mode to either text-only 80-columns (80) or 40 columns with graphics (40)
    FP_ACTIVATE_MOUSE               = 0x03, // enable (1) or disable (0) mouse - will show pointer on the screen, and receive moves and clicks as events

    //
    // TEXT COMMANDS
    //

    // statements

    FP_TEXT_PRINT_CHAR              = 0x10, // print one character
    FP_TEXT_SET_CHAR                = 0x11, // set character in the following position, with the following attributes
    FP_TEXT_PRINT_TEXT              = 0x12, // print the following sequence of characters
    FP_TEXT_SET_POS                 = 0x13, // move the cursor to this row/column
    FP_TEXT_SET_CHAR_ATTRIB         = 0x14, // set the text color (and other attributes)
    FP_TEXT_SET_PALETTE             = 0x15, // update the text palette with the following 16 colors
    FP_TEXT_CLEAR_SCREEN            = 0x16, // clear the screen
    FP_TEXT_SET_CURSOR_ATTRIB       = 0x17, // set cursor attrib
    FP_TEXT_BEEP                    = 0x18, // sends a beep to the terminal

    // events (terminal to host)

    FP_EVENT_KEY_PRESS              = 0x60, // a key was pressed on the keyboard
    FP_EVENT_KEY_RELEASE            = 0x61, // a key was released on the keyboard
    FP_EVENT_KEYSTROKE              = 0x62, // a keystroke was registered (can be repeated if the user keeps pressing the key)
    FP_EVENT_JOYSTICK_PRESS         = 0x63, // a key was pressed on the joystick
    FP_EVENT_JOYSTICK_RELEASE       = 0x64, // a key was released on the joystick
    FP_EVENT_MOUSE_MOVE             = 0x65, // mouse was moved
    FP_EVENT_MOUSE_CLICK            = 0x66, // mouse was clicked

    //
    // GRAPHICAL COMMANDS
    //

    // statements
    // TODO - commands to create sprites, tiles and tilemaps, show/hide things from the screen

    // queries
    // TODO - commands to query collisions

    //
    // AUDIO COMMANDS
    //

    // TODO - commands to create and play/pause music, and to create and play special effects

} FP_Command;

typedef enum __attribute__((packed)) {
    TEXT_80_COLUMNS   = 0,
    TEXT_AND_GRAPHICS = 1,
} GraphicalMode;

typedef enum __attribute__((packed)) {
    SK_ESC = 128, SK_F1, SK_F2, SK_F3, SK_F4, SK_F5, SK_F6, SK_F7, SK_F8, SK_F9, SK_F10, SK_F11, SK_F12, SK_TAB, SK_CAPSLOCK, SK_WIN,
    SK_INSERT, SK_HOME, SK_END, SK_PAGEUP, SK_PAGEDOWN, SK_UP, SK_DOWN, SK_LEFT, SK_RIGHT, SK_ENTER, SK_BACKSPACE,
    SK_DELETE, SK_PRINTSCREEN, SK_PAUSEBREAK,
} SpecialKey;

typedef struct __attribute__((packed)) {
    bool shift   : 1;
    bool control : 1;
    bool alt     : 1;
} KeyMod;

typedef struct __attribute__((packed)) {
    union {
        uint8_t    key;
        SpecialKey special_key;
    };
    KeyMod mod;
} FP_KeyEvent;

typedef struct __attribute__((packed)) {
    uint8_t color   : 4;
    bool    reverse : 1;
} CharAttrib;

typedef struct __attribute__((packed)) {
    uint8_t color   : 4;
    bool    visible : 1;
} CursorAttrib;

typedef struct __attribute__((packed)) {
    FP_Command command;
    union {

        GraphicalMode graphical_mode;

        uint8_t chr;

        struct __attribute__((packed)) {
            uint8_t    c;
            uint8_t    line;
            uint8_t    column;
            CharAttrib attrib;
        } set_char;

        uint8_t text[FP_MSG_CONTENTS_SZ];

        struct __attribute__((packed)) {
            uint8_t line;
            uint8_t column;
        } set_pos;

        CharAttrib char_attrib;

        uint8_t colors[PALETTE_SZ * 3];

        CursorAttrib cursor_attrib;

        char keystroke[FP_MSG_CONTENTS_SZ];

        FP_KeyEvent key;
    };
} FP_Message;

#endif //FORTUNA_PROTOCOL_H_
