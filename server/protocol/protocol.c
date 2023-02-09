#include "protocol.h"

#include <string.h>

#include "../comm/comm.h"
#include "ansi.h"
#include "error/error.h"

#define INPUTBUF_SZ  (32 * 1024)
#define OUTPUTBUF_SZ (32 * 1024)

typedef struct {
    ssize_t (* process_pending_input)(const uint8_t* buffer, size_t bufsz, Scene* scene);
    int     (* terminal_event)(FP_Command* command, Buffer* output_buffer_);
} ProtocolFunctions;
static ProtocolFunctions protocol_f = { NULL, NULL };

static uint8_t         input_buf_[INPUTBUF_SZ];
static size_t          input_buf_sz_ = 0;

static Buffer* output_buffer_ = NULL;

int protocol_init(Options* options, Buffer* output_buffer)
{
    output_buffer_ = output_buffer;

    switch (options->protocol) {
        case PR_ANSI:
            protocol_f = (ProtocolFunctions) { ansi_process_pending_input, ansi_terminal_event };
            return 0;
        default:
            return ERR_NOT_IMPLEMENTED;
    }
}

void protocol_process_input_queue(Scene* scene)
{
    // process pending inputs
    size_t sz = comm_unload_input_queue(input_buf_, INPUTBUF_SZ);
    if (sz > 0) {
        input_buf_sz_ += sz;
        ssize_t bytes_processed = protocol_f.process_pending_input(input_buf_, input_buf_sz_, scene);
        if (bytes_processed < 0)
            error_check(bytes_processed);
        memmove(input_buf_, &input_buf_[bytes_processed], bytes_processed);
        input_buf_sz_ -= bytes_processed;
    }
}

void protocol_terminal_event(FP_Command* command)
{
    error_check(protocol_f.terminal_event(command, output_buffer_));
}