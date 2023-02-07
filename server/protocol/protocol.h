#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../options/options.h"
#include "scene/scene.h"
#include "fortuna-protocol.h"

int  protocol_init(Options* options);
void protocol_process_pending_data(Scene* scene);
void protocol_terminal_event(FP_Command* command);

#endif //PROTOCOL_H_
