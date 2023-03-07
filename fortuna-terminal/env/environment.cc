#include "environment.hh"

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options))
{

}
