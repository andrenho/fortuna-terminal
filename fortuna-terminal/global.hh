#ifndef GLOBAL_HH_
#define GLOBAL_HH_

#include <memory>

#include "scene/scene.hh"
#include "event/inputevent.hh"
#include "event/outputevent.hh"

extern class Options options;

extern std::unique_ptr<class CommunicationModule> communication_module;
extern std::unique_ptr<class Protocol>            protocol;
extern InputQueue                                 input_queue;
extern Scene                                      scene;
extern std::unique_ptr<class Terminal>            terminal;
extern OutputQueue                                output_queue;

#endif //GLOBAL_HH_
