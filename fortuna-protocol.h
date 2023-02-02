#ifndef FORTUNA_PROTOCOL_H_
#define FORTUNA_PROTOCOL_H_

#include <stdint.h>

/**************************
 *                        *
 *        INPUT           *
 *                        *
 **************************/

typedef enum __attribute__((packed)) {
    FP_TEXT_PRINT_CHAR,           // 1 byte parameter
    FP_TEXT_MOVE_UP,              // 1 byte parameter
    FP_TEXT_MOVE_DOWN,            // 1 byte parameter
    FP_TEXT_MOVE_FORWARD,         // 1 byte parameter
    FP_TEXT_MOVE_BACKWARD,        // 1 byte parameter
    FP_TEXT_MOVE_TO,              // 2 byte parameter (line, column)
    FP_TEXT_RESET_FORMATTING,     // no parameter
    FP_TEXT_SET_COLOR,            // 1 byte parameter
    FP_TEXT_CLEAR_SCREEN,         // no parameter
    FP_TEXT_CLEAR_TO_BOL,         // no parameter
    FP_TEXT_CLEAR_TO_EOL,         // no parameter
    FP_TEXT_CLEAR_LINE,           // no parameter
    FP_TEXT_SET_SCROLL_REGION,    // 2 parameters (start and end line)
    FP_TEXT_DEL_CHAR_UNDER_CURSOR, // no parameter
} FP_InputCommandType;

typedef struct __attribute__((packed)) {
    FP_InputCommandType command : 8;
    uint8_t             p1      : 8;
    uint8_t             p2      : 8;
} FP_InputCommand;

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
