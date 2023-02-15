#ifndef FORTUNA_H_
#define FORTUNA_H_

#include <fortuna-protocol.h>

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "../scene/scene.h"
#include "buffer/buffer.h"
#include "error/error.h"

FT_Result fortuna_init(Scene* scene);
FT_Result fortuna_process_pending_input(const uint8_t* buffer, size_t bufsz, Scene* scene, Buffer* output_buffer, size_t* bytes_processed);
FT_Result fortuna_terminal_event(FP_Message* message, Buffer* output_buffer);
void      fortuna_finalize();

#endif