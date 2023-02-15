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

FP_Result fp_msg_serialize(const FP_Message* inmsg, uint8_t outbuf[FP_MSG_SZ], uint8_t* msg_sz)
{
    size_t i = 0;
    *msg_sz = fp_msg_size(inmsg);
    if (*msg_sz > FP_MSG_CONTENTS_SZ)
        return FP_ERR_MESSAGE_TOO_LARGE;

    outbuf[i++] = FP_FRAME_START;
    outbuf[i++] = *msg_sz;
    memcpy(&outbuf[i], inmsg, *msg_sz);   // TODO - swap multi-byte fields to proper endianess
    i += *msg_sz;
    outbuf[i++] = fp_calculate_checksum(&outbuf[2], *msg_sz);
    outbuf[i++] = FP_FRAME_END;

    *msg_sz += 4;

    return FP_OK;
}

FP_Result fp_msg_unserialize(const uint8_t inbuf[FP_MSG_SZ], size_t bufsz, FP_Message* outmsg)
{
    if (bufsz < 1)
        return FP_ERR_MESSAGE_TOO_SHORT;

    uint8_t msg_sz = inbuf[1];

    if (bufsz < msg_sz)
        return FP_ERR_MESSAGE_TOO_SHORT;

    if (inbuf[0] != FP_FRAME_START)
        return FP_ERR_INVALID_MESSAGE;
    if (inbuf[msg_sz + 3] != FP_FRAME_END)
        return FP_ERR_INVALID_MESSAGE;

    uint8_t checksum = fp_calculate_checksum(&inbuf[2], msg_sz);
    if (inbuf[msg_sz + 2] != checksum)
        return FP_ERR_INCORRECT_CHECKSUM;

    memcpy(outmsg, &inbuf[2], msg_sz);     // TODO - swap multi-byte fields to proper endianess

    return FP_OK;
}

/*******************
 *                 *
 *  COMMUNICATION  *
 *                 *
 *******************/

FP_Result fp_msg_send(const FP_Message* msg, FP_SendFunction sendf, FP_RecvFunction recvf, int* comm_error)
{
    uint8_t buffer[FP_MSG_SZ];

    uint8_t msg_sz;
    FP_Result result = fp_msg_serialize(msg, buffer, &msg_sz);
    if (result != FP_OK)
        return result;

    for (size_t i = 0; i < FP_SEND_ATTEMPTS; ++i) {

        // send message
        *comm_error = sendf(buffer, msg_sz);
        if (*comm_error < 0)
            return FP_ERR_SEND_FAILED;

        // receive response
        uint8_t rbuf[3];
        *comm_error = recvf(rbuf, 3);
        if (*comm_error < 0)
            return FP_ERR_RECV_FAILED;

        uint8_t response = rbuf[0];
        if (rbuf[0] != rbuf[1] && rbuf[0] != rbuf[2]) {
            if (rbuf[1] == rbuf[2])
                response = rbuf[1];
            else
                return FP_ERR_UNEVEN_RESPONSE;
        }

        // parse response
        if (response == FP_RESPONSE_OK)
            return FP_OK;
        else if (response != FP_RESPONSE_BROKEN && response != FP_RESPONSE_INVALID_CHECKSUM)
            return FP_ERR_INVALID_MESSAGE;
    }

    return FP_ERR_TOO_MANY_FAILED_ATTEMPTS;
}

static FP_Result fp_send_response(FP_SendFunction sendf, uint8_t value, FP_Result result, int* comm_error)
{
    uint8_t buf[3] = { value, value, value };
    *comm_error = sendf(buf, 3);
    if (*comm_error < 0)
        return FP_ERR_SEND_FAILED;
    return result;
}

FP_Result fp_msg_recv(FP_Message* cmd, FP_SendFunction sendf, FP_RecvFunction recvf, int* comm_error)
{
    for (size_t i = 0; i < FP_RECV_ATTEMPTS; ++i) {
        uint8_t c;
        *comm_error = recvf(&c, 1);
        if (* comm_error < 0)
            return FP_ERR_RECV_FAILED;
        if (c == FP_FRAME_START)
            goto frame_start_found;
    }

    return fp_send_response(sendf, FP_RESPONSE_ERROR, FP_ERR_FRAME_START_NOT_RECEIVED, comm_error);

frame_start_found:;
    uint8_t msg_sz;
    *comm_error = recvf(&msg_sz, 1);
    if (*comm_error < 0)
        return FP_ERR_RECV_FAILED;
    if (msg_sz > FP_MSG_SZ) {
        return fp_send_response(sendf, FP_RESPONSE_ERROR, FP_ERR_MESSAGE_TOO_LARGE, comm_error);
    }

    uint8_t buf[FP_MSG_SZ] = { FP_FRAME_START, msg_sz };
    *comm_error = recvf(&buf[2], msg_sz + 2);   // message size, checksum and frame stop
    if (*comm_error < 0)
        return FP_ERR_RECV_FAILED;

    FP_Result result = fp_msg_unserialize(buf, sizeof buf, cmd);
    if (result == FP_ERR_INCORRECT_CHECKSUM)
        return fp_send_response(sendf, FP_RESPONSE_INVALID_CHECKSUM, result, comm_error);
    else if (result == FP_OK)
        return fp_send_response(sendf, FP_RESPONSE_OK, result, comm_error);
    else
        return fp_send_response(sendf, FP_RESPONSE_BROKEN, result, comm_error);
}