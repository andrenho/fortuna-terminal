#include <stdlib.h>
#include <string.h>
#include "fortuna-protocol.h"

#define FP_P_VARIABLE (-1)
#define FP_P_UNDEFINED (-2)
#define FP_P_INVALID_CMD (-3)

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

/*******************
 *                 *
 *  SERIALIZATION  *
 *                 *
 *******************/

static int fp_param_sz(FP_Command command)
{
    switch (command) {

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

static int fp_msg_size(const FP_Message* msg)
{
    int sz;
    switch (msg->command) {
        case FP_TEXT_PRINT_TEXT:
            sz = (int) strlen((const char *) msg->text) + 2;
            break;
        case FP_EVENT_KEYSTROKE:
            sz = msg->keystroke.length + 1;
            break;
        default:
            sz = fp_param_sz(msg->command) + 1;
    }
    return sz;
}

uint8_t fp_calculate_checksum(const uint8_t* buffer, size_t sz)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < sz; ++i)
        checksum ^= buffer[i];
    return checksum;
}

int fp_msg_serialize(const FP_Message* inmsg, uint8_t outbuf[FP_MSG_SZ])
{
    size_t i = 0;
    uint8_t msg_sz = fp_msg_size(inmsg);
    if (msg_sz > FP_MSG_CONTENTS_SZ)
        abort();

    outbuf[i++] = FP_FRAME_START;
    outbuf[i++] = msg_sz;
    memcpy(&outbuf[i], inmsg, msg_sz);   // TODO - swap multi-byte fields to proper endianess
    i += msg_sz;
    outbuf[i++] = fp_calculate_checksum(&outbuf[2], msg_sz);
    outbuf[i++] = FP_FRAME_END;

    return (int) i;
}

int fp_msg_unserialize(const uint8_t inbuf[FP_MSG_SZ], FP_Message* outmsg)
{
    uint8_t msg_sz = inbuf[1];

    if (inbuf[0] != FP_FRAME_START)
        return FP_RESPONSE_BROKEN;
    if (inbuf[msg_sz + 4] != FP_FRAME_END)
        return FP_RESPONSE_BROKEN;

    uint8_t checksum = fp_calculate_checksum(&inbuf[2], msg_sz);
    if (inbuf[msg_sz + 3] != checksum)
        return FP_RESPONSE_INVALID_CHECKSUM;

    memcpy(outmsg, &inbuf[2], msg_sz);     // TODO - swap multi-byte fields to proper endianess

    return 0;
}

/*******************
 *                 *
 *  COMMUNICATION  *
 *                 *
 *******************/

int fp_msg_send(const FP_Message* msg, FP_SendFunction sendf, FP_RecvFunction recvf)
{
    uint8_t buffer[FP_MSG_SZ];

    int r = fp_msg_serialize(msg, buffer);
    if (r < 0)
        return r;

    for (size_t i = 0; i < FP_SEND_ATTEMPTS; ++i) {

        // send message
        r = sendf(buffer, r);
        if (r < 0)
            return r;

        // receive response
        uint8_t rbuf[3];
        r = recvf(rbuf, 3);
        if (r < 0)
            return r;
        uint8_t response = rbuf[0];
        if (rbuf[0] != rbuf[1] && rbuf[0] != rbuf[2]) {
            if (rbuf[1] == rbuf[2])
                response = rbuf[1];
            else
                return -FP_RESPONSE_ERROR;
        }

        // parse response
        if (response == FP_RESPONSE_OK)
            break;
        else if (response != FP_RESPONSE_BROKEN && response != FP_RESPONSE_INVALID_CHECKSUM)
            return -FP_RESPONSE_ERROR;
    }

    return 0;
}

static int fp_send_response(FP_SendFunction sendf, uint8_t value)
{
    uint8_t buf[3] = { value, value, value };
    int r = sendf(buf, 3);
    if (r < 0)
        return r;
    return -value;
}

int fp_msg_recv(FP_Message* cmd, FP_SendFunction sendf, FP_RecvFunction recvf)
{
    for (size_t i = 0; i < FP_RECV_ATTEMPTS; ++i) {
        uint8_t c;
        int r = recvf(&c, 1);
        if (r < 0)
            return r;
        if (c == FP_FRAME_START)
            goto frame_start_found;
    }
    return fp_send_response(sendf, FP_RESPONSE_ERROR);

frame_start_found:;
    uint8_t msg_sz;
    int r = recvf(&msg_sz, 1);
    if (r < 0)
        return r;
    if (msg_sz > FP_MSG_SZ)
        return fp_send_response(sendf, FP_RESPONSE_ERROR);

    uint8_t buf[FP_MSG_SZ] = { FP_FRAME_START, msg_sz };
    r = recvf(&buf[2], msg_sz + 2);   // message size, checksum and frame stop
    if (r < 0)
        return r;

    FP_Message message;
    r = fp_msg_unserialize(buf, &message);
    if (r != 0)
        return fp_send_response(sendf, -r);

    fp_send_response(sendf, FP_RESPONSE_OK);
    return 0;
}