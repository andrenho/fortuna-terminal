#include <string.h>
#include "fortuna-protocol.h"

#define FP_P_VARIABLE (-1)
#define FP_P_UNDEFINED (-2)
#define FP_P_INVALID_CMD (-3)

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

static int fp_param_sz(FP_CommandType command_type)
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

static int fp_msg_size(FP_Command* cmd)
{
    int sz;
    switch (cmd->command) {
        case FP_TEXT_PRINT_TEXT:
            sz = (int) strlen((const char *) cmd->text) + 2;
            break;
        case FP_EVENT_KEYSTROKE:
            sz = cmd->keystroke.length + 1;
            break;
        default:
            sz = fp_param_sz(cmd->command) + 1;
    }
    return sz;
}

#define FRAME_CMD_SZ 43
#define FRAME_START 0x5e
#define FRAME_END   0x6e

#define SEND_ATTEMPTS 8

typedef struct {
    size_t   length;
    uint32_t bytes[FRAME_CMD_SZ];
    uint8_t  checksum;
} FP_Frame;

uint8_t fp_calculate_checksum(const uint8_t* buffer, size_t sz)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < sz; ++i)
        checksum ^= buffer[i];
    return checksum;
}

int fp_send(FP_Command cmds[], size_t n_cmds, FP_SendFunction sendf, FP_RecvFunction recvf)
{
    if (n_cmds == 0)
        return 0;

    for (size_t i = 0; i < n_cmds; ++i) {

        size_t msg_index = i;

        // calculate how many messages will fit in one frame
        int bytes_left = FRAME_CMD_SZ;
        uint8_t message_count = 0;
        while (i < n_cmds) {
            int msg_size = fp_msg_size(&cmds[i]);
            if (bytes_left - msg_size < 0)
                break;
            bytes_left -= msg_size;
            ++message_count;
            ++i;
        }

        // create frame
        uint8_t frame_size = FRAME_CMD_SZ - bytes_left + 5;
        uint8_t message[frame_size];
        size_t k = 0;
        message[k++] = FRAME_START;
        message[k++] = FRAME_CMD_SZ - bytes_left;
        for (size_t j = 0; j < message_count; ++j) {
            uint8_t msg_sz = fp_msg_size(&cmds[msg_index + j]);
            memcpy(&message[k], &cmds[msg_index + j], msg_sz);
            k += msg_sz;
        }
        message[k++] = fp_calculate_checksum(&message[2], FRAME_CMD_SZ - bytes_left);
        message[k++] = FRAME_END;

        for (size_t j = 0; j < SEND_ATTEMPTS; ++j) {

            // send message
            int r = sendf(message, k);
            if (r < 0)
                return r;

            // receive output
            uint8_t rbuf[3];
            r = recvf(rbuf, 3);
            if (r < 0)
                return r;
            uint8_t response = rbuf[0];
            if (rbuf[0] != rbuf[1] && rbuf[0] != rbuf[2]) {
                if (rbuf[1] == rbuf[2])
                    response = rbuf[1];
                else
                    return FP_RESPONSE_ERROR;
            }

            // parse response
            if (response == FP_RESPONSE_OK)
                break;
            else if (response != FP_RESPONSE_BROKEN && response != FP_RESPONSE_INVALID_CHECKSUM)
                return FP_RESPONSE_ERROR;
        }
    }

    return 0;
}