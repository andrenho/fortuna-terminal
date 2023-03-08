#include "environment.hh"

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options)),
      scene_(options.mode),
      protocol_(options.mode, scene_)
{

}

void Environment::run_io_threads()
{

}

void Environment::execute_step(Duration duration)
{

}

void Environment::show_error(std::exception const &e)
{

}

void Environment::blink_cursor()
{

}
