#include "fortuna.h"

#include <stdio.h>

static Scene* scene_ = NULL;

FT_Result fortuna_init(Scene* scene)
{
    scene_ = scene;
    return FT_OK;
}

static void execute_input_message(FP_Message* msg, Scene* scene)
{
    switch (msg->command) {
        case FP_TEXT_PRINT_CHAR:
            text_add_char(&scene->text, msg->chr);
            break;
        default:
            fprintf(stderr, "Invalid message command received: 0x%02X.\n", msg->command);
    }
}

FT_Result fortuna_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene, Buffer* output_buffer, size_t* bytes_processed)
{
    uint8_t content_sz;
    uint8_t msg_sz;

    if (bufsz == 0) {
        *bytes_processed = 0;
        return FT_OK;
    } else if (bufsz > 0 && buffer[0] != FP_FRAME_START) {
        *bytes_processed = 1;  // we're receiving garbage, ignore until we receive a frame start
        return FT_OK;
    } else if (bufsz > 1) {
        content_sz = buffer[1];
        msg_sz = content_sz + 4;
        if (bufsz < msg_sz) {
            *bytes_processed = 0;
            return FT_OK;
        }
    }

    FP_Message msg;
    FP_Result r = fp_msg_unserialize(buffer, msg_sz, &msg);

    uint8_t response = FP_RESPONSE_OK;
    if (r == FP_ERR_INCORRECT_CHECKSUM)
        response = FP_RESPONSE_INVALID_CHECKSUM;
    else if (r != FP_OK)
        response = FP_RESPONSE_BROKEN;  // check for number of attempts

    uint8_t rbuf[] = { response, response, response };
    buffer_add_bytes(output_buffer, rbuf, 3);

    if (r == FP_OK)
        execute_input_message(&msg, scene);

    *bytes_processed = msg_sz;
    return FT_OK;
}

FT_Result fortuna_terminal_event(FP_Message* message, Buffer* output_buffer)
{
    return FT_OK;
}

void fortuna_finalize()
{
}
