#include "fortuna.h"

static Scene* scene_ = NULL;

FT_Result fortuna_init(Scene* scene)
{
    scene_ = scene;
    return FT_OK;
}

FT_Result fortuna_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene, size_t* bytes_processed)
{
    return FT_OK;
}

FT_Result fortuna_terminal_event(FP_Message* message, Buffer* output_buffer)
{
    return FT_OK;
}

void fortuna_finalize()
{
}
