#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../options/options.h"
#include "scene/scene.h"

int  protocol_init(Options* options);
void protocol_process_pending_input(Scene* scene);

#endif //PROTOCOL_H_
