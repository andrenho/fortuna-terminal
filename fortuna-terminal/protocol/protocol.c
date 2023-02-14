#include "protocol.h"

#include <string.h>

#include "../comm/comm.h"
#include "ansi.h"
#include "error/error.h"

typedef struct {
    ssize_t (* process_pending_input)(const uint8_t* buffer, size_t bufsz, Scene* scene);
    int     (* terminal_event)(FP_Message* command, Buffer* output_buffer_);
    int     (* finalize)();
} ProtocolFunctions;
static ProtocolFunctions protocol_f = { NULL, NULL, NULL };

static Buffer* output_buffer_ = NULL;

#define BUFFER_SZ (32 * 1024)

FT_Result protocol_init(Buffer* output_buffer, Scene* scene)
{
    output_buffer_ = output_buffer;

    switch (options.protocol) {
        case PR_ANSI:
            protocol_f = (ProtocolFunctions) { ansi_process_pending_input, ansi_terminal_event, ansi_finalize };
            ansi_init(scene);
            return 0;
        default:
            E_CHECK(FT_ERR_APP, "Protocol not implemented");
    }

    return FT_OK;
}

void protocol_process_input(Buffer* input_buffer, Scene* scene)
{
    uint8_t protocol_input_buffer[BUFFER_SZ];

    ssize_t sz = buffer_move_data_to_array(input_buffer, protocol_input_buffer, BUFFER_SZ);
    if (sz > 0)
        protocol_f.process_pending_input(protocol_input_buffer, sz, scene);
}

void protocol_terminal_event(FP_Message* message)
{
    E_UI(protocol_f.terminal_event(message, output_buffer_));
}

int protocol_finalize()
{
    return protocol_f.finalize();
}
