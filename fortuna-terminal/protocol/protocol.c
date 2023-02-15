#include "protocol.h"

#include <string.h>

#include "ansi.h"
#include "fortuna.h"

typedef struct {
    FT_Result (* process_pending_input)(const uint8_t* buffer, size_t bufsz, Scene* scene, size_t* bytes_processed);
    FT_Result (* terminal_event)(FP_Message* command, Buffer* output_buffer_);
    void      (* finalize)();
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
            return FT_OK;
        case PR_FORTUNA:
            protocol_f = (ProtocolFunctions ) { fortuna_process_pending_input, fortuna_terminal_event, fortuna_finalize };
            fortuna_init(scene);
            return FT_OK;
        default:
            E_CHECK(FT_ERR_APP, "Protocol not implemented");
    }
}

void protocol_process_input(Buffer* input_buffer, Scene* scene)
{
    uint8_t protocol_input_buffer[BUFFER_SZ];

    ssize_t sz = buffer_move_data_to_array(input_buffer, protocol_input_buffer, BUFFER_SZ);
    size_t bytes_processed;
    if (sz > 0) {
        protocol_f.process_pending_input(protocol_input_buffer, sz, scene, &bytes_processed);

        // re-add bytes there were not processed back into the input buffer
        ssize_t bytes_not_processed = sz - (ssize_t) bytes_processed;
        if (bytes_not_processed > 0)
            buffer_add_bytes(input_buffer, &protocol_input_buffer[bytes_processed], bytes_not_processed);
    }
}

void protocol_terminal_event(FP_Message* message)
{
    E_UI(protocol_f.terminal_event(message, output_buffer_));
}

void protocol_finalize()
{
    protocol_f.finalize();
}
