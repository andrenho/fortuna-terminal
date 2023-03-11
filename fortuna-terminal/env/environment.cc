#include "environment.hh"

using namespace std::string_literals;
using namespace std::chrono_literals;

template class SyncQueue<uint8_t>;

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options)),
      scene_(options.mode),
      protocol_(options.mode, scene_, *output_queue_),
      runner_(*input_queue_, *output_queue_, *comm_),
      show_fps_counter_(options.terminal_options.show_fps_counter)
{
}

void Environment::run_io_threads()
{
    runner_.run_io_threads();
}

void Environment::finalize_threads()
{
    runner_.finalize_threads();
}

void Environment::execute_step(Duration duration)
{
    scene_.text().update_blink();
    protocol_.execute_inputs(*input_queue_);   // TODO - parellize these two requests (?)
    protocol_.execute_outputs(*output_queue_);
    show_fps_counter(duration);
}

void Environment::show_fps_counter(Duration duration)
{
    if (!show_fps_counter_)
        return;
    double ms = (double) std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    size_t fps = (1000.0 / ms);
    if (fps > 999)
        fps = 999;
    scene_.text().write(scene_.text().lines() - 1, scene_.text().columns() - 8, "FPS " + std::to_string(fps), { COLOR_ORANGE, true, false });
}

void Environment::show_error(std::exception const &e)
{
    std::string message = "\e[0;31m"s + e.what() + "\r-- Press ENTER to continue or Ctrl+F12 to quit --\r\e[0m";
    std::vector<uint8_t> v(message.begin(), message.end());
    output_queue_->push_all(v);
    std::this_thread::sleep_for(10ms);
}

void Environment::reset()
{
    // TODO
}

void Environment::set_mode(Mode mode)
{
    scene_.set_mode(mode);
}