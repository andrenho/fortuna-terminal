#include <string.h>
#include "fortuna-protocol.h"

#define FP_P_VARIABLE (-1)
#define FP_P_UNDEFINED (-2)
#define FP_P_INVALID_CMD (-3)

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

static int fp_param_count(FP_CommandType command_type)
{
    switch (command_type) {

        case FP_RESET:
        case FP_GRAPHICAL_MODE:
        case FP_ACTIVATE_MOUSE:
        case FP_TEXT_CLEAR_SCREEN:
        case FP_TEXT_BEEP:
        case FP_TEXT_GET_CURSOR_POS:
        case FP_KEYMOD_STATE:
        case FP_MOUSE_STATE:
        case FP_JOYSTICK_STATE:
            return 0;

        case FP_TEXT_PRINT_CHAR:
        case FP_TEXT_SET_CHAR_ATTRIB:
        case FP_TEXT_SET_CURSOR_ATTRIB:
            return 1;

        case FP_TEXT_SET_POS:
        case FP_EVENT_KEY_PRESS:
        case FP_EVENT_KEY_RELEASE:
            return 2;

        case FP_TEXT_SET_CHAR:
            return 4;

        case FP_TEXT_SET_PALETTE:
            return PALETTE_SZ * 3;

        case FP_TEXT_PRINT_TEXT:
        case FP_EVENT_KEYSTROKE:
            return FP_P_VARIABLE;

        case FP_EVENT_JOYSTICK_PRESS:
        case FP_EVENT_JOYSTICK_RELEASE:
        case FP_EVENT_MOUSE_MOVE:
        case FP_EVENT_MOUSE_CLICK:
            return FP_P_UNDEFINED;
    }

    return FP_P_INVALID_CMD;
}

#define FRAME_SZ 32

typedef struct {
    size_t   length;
    uint32_t bytes[FRAME_SZ];
    uint8_t  checksum;
} FP_Frame;

int fp_send(FP_Command cmds[], size_t n_cmds, FP_SendFunction sendf, FP_RecvFunction recv_function)
{
    if (n_cmds == 0)
        return 0;

    FP_Frame frame;
    frame.length = 0;
    frame.checksum = 0;

    uint8_t frame_current_pos = 0;
    uint8_t cmd_current_pos = 0;
    size_t cmd_i = 0;

    for (size_t i = 0; i < n_cmds; ++i) {
    }

    return 0;
}