#ifndef FORTUNA_PROTOCOL_H_
#define FORTUNA_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

#define PALETTE_SZ 16

typedef enum __attribute__((packed)) {

    //
    // GENERIC COMMANDS
    //

    FP_RESET                        = 0x00, // pulls the RESET line down, effectively resetting the computer
    FP_GRAPHICAL_MODE               = 0x01, // sets the mode to either text-only 80-columns (80) or 40 columns with graphics (40)
    FP_ACTIVATE_MOUSE               = 0x02, // enable (1) or disable (0) mouse - will show pointer on the screen, and receive moves and clicks as events

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

    // queries

    FP_TEXT_GET_CURSOR_POS          = 0x50, // return the position of the cursor
    FP_KEYMOD_STATE                 = 0x5d, // return the state of the key modifiers (Ctrl, Alt, Shift)
    FP_MOUSE_STATE                  = 0x5e, // return the state of the mouse (position and buttons)
    FP_JOYSTICK_STATE               = 0x5f, // return the state of the joystick n

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

} FP_CommandType;

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
    FP_CommandType command;
    union {

        uint8_t chr;

        struct __attribute__((packed)) {
            uint8_t    c;
            uint8_t    line;
            uint8_t    column;
            CharAttrib attrib;
        } set_char;

        struct __attribute__((packed)) {
            uint8_t* text;
        } print_text;

        struct __attribute__((packed)) {
            uint8_t line;
            uint8_t column;
        } set_pos;

        CharAttrib char_attrib;

        uint8_t colors[PALETTE_SZ * 3];

        CursorAttrib cursor_attrib;

        struct __attribute__((packed)) KeyStroke {
            uint8_t* data;
            uint8_t  length;
        } keystroke;

        FP_KeyEvent key;
    };
} FP_Command;

typedef int (*FP_SendFunction)(uint8_t*, size_t);
typedef int (*FP_RecvFunction)(uint8_t*, size_t);

int fp_send(FP_Command cmds[], size_t n_cmds, FP_SendFunction sendf, FP_RecvFunction recv_function);

#endif //FORTUNA_PROTOCOL_H_
