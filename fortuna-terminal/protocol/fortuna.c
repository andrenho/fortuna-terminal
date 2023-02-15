#include "fortuna.h"

static Scene* scene_ = NULL;

FT_Result fortuna_init(Scene* scene)
{
    scene_ = scene;
    return FT_OK;
}

FT_Result fortuna_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene, size_t* bytes_processed)
{
    uint8_t content_sz = 0;
    uint8_t msg_sz = 0;

    if (bufsz <= 1) {
        *bytes_processed = 0;
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
