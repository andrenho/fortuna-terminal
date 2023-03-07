#include "environment.hh"

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options)),
      scene_(options.mode),
      protocol_(options.mode, scene_)
{

}
