#ifndef FORTUNA_PROTOCOL_H_
#define FORTUNA_PROTOCOL_H_

#include <stdint.h>

/**************************
 *                        *
 *        INPUT           *
 *                        *
 **************************/

typedef enum __attribute__((packed)) {

    //
    // GENERIC COMMANDS
    //

    FP_RESET                        = 0x00, // pulls the RESET line down, effectively resetting the computer
    FP_GRAPHICAL_MODE               = 0x01, // sets the mode to either text-opnly 80-columns (80) or 40 columns with graphics (40)
    FP_ACTIVATE_MOUSE               = 0x02, // enable (1) or disable (0) mouse - will show pointer on the screen, and receive moves and clicks as events

    //
    // TEXT COMMANDS
    //

    // statements

    FP_TEXT_PRINT_CHAR              = 0x10, // print one character
    FP_TEXT_PRINT_TEXT              = 0x11, // print the following sequence of characters
    FP_TEXT_MOVE_UP                 = 0x12, // move the cursor up by n lines
    FP_TEXT_MOVE_DOWN               = 0x13, // move the cursor down by n lines
    FP_TEXT_MOVE_FORWARD            = 0x14, // move the cursor right by n characters
    FP_TEXT_MOVE_BACKWARD           = 0x15, // move the cursor left by n characters
    FP_TEXT_MOVE_TO                 = 0x16, // move the character to this y/x position
    FP_TEXT_RESET_FORMATTING        = 0x17, // reset the formatting to the original
    FP_TEXT_SET_COLOR               = 0x18, // set the text color to the following (in palette index)
    FP_TEXT_SET_PALETTE             = 0x19, // update the text palette with the following 16 colors
    FP_TEXT_CLEAR_SCREEN            = 0x1a, // clear the screen
    FP_TEXT_CLEAR_TO_BOL            = 0x1b, // clear from the cursor to the beginning of the line
    FP_TEXT_CLEAR_TO_EOL            = 0x1c, // clear from the cursor to the end of the line
    FP_TEXT_SET_SCROLL_REGION       = 0x1d, // set the scroll region to begin from line p1 to line p2
    FP_TEXT_CLEAR_LINE              = 0x1e, // clear the line under the cursor
    FP_TEXT_CLEAR_CHAR              = 0x1f, // clear the char under the cursor
    FP_TEXT_INSERTION_MODE          = 0x20, // set insertion mode inactive (0) or active (1)

    // queries

    FP_TEXT_GET_CURSOR_POS          = 0x50, // return the position of the cursor
    FP_KEYMOD_STATE                 = 0x5d, // return the state of the key modifiers (Ctrl, Alt, Shift)
    FP_MOUSE_STATE                  = 0x5e, // return the state of the mouse (position and buttons)
    FP_JOYSTICK_STATE               = 0x5f, // return the state of the joystick n

    // events (terminal to host)

    FP_EVENT_KEY_PRESS              = 0x60, // a key was pressed on the keyboard
    FP_EVENT_KEY_RELEASE            = 0x61, // a key was released on the keyboard
    FP_EVENT_KEYSTROKE              = 0x62, // a keystroke was registered (can be repeated if the user keeps pressing the key)
    FP_EVENT_JOYSTICK_PRESS         = 0x62, // a key was pressed on the joystick
    FP_EVENT_JOYSTICK_RELEASE       = 0x63, // a key was released on the joystick
    FP_EVENT_MOUSE_MOVE             = 0x64, // mouse was moved
    FP_EVENT_MOUSE_CLICK            = 0x65, // mouse was clicked

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

typedef struct __attribute__((packed)) {
    FP_CommandType command;
    union {
        uint8_t        p1;
        uint8_t        p2;
    };
} FP_Command;

// return values (not used yet)
typedef enum __attribute__((packed)) {
    FP_OK, FP_INVALID_COMMAND, FP_INVALID_PARAMETER,
} FP_Response;

/**************************
 *                        *
 *        OUTPUT          *
 *                        *
 **************************/

typedef enum __attribute__((packed)) {
    FP_KEY_PRESS,
    FP_KEY_RELEASE,
    FP_SPECIAL_KEY_PRESS,
    FP_SPECIAL_KEY_RELEASE,
} FP_OutputEventType;

typedef enum __attribute__((packed)) {
    Esc, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, Tab, CapsLock, Win,
    Insert, Home, End, PageUp, PageDown, Up, Down, Left, Right, Enter, Backspace,
    Delete, PrintScreen, PauseBreak,
} SpecialKey;

typedef struct __attribute__((packed)) {
    bool shift   : 1;
    bool control : 1;
    bool alt     : 1;
} KeyMod;

typedef struct __attribute__((packed)) {
    FP_OutputEventType event : 8;
    union {
        uint8_t        key         : 8;
        SpecialKey     special_key : 8;
    };
    KeyMod             mod;
} FP_OutputEvent;

#endif //FORTUNA_PROTOCOL_H_
