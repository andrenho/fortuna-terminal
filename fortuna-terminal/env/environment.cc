#include "environment.hh"

#define RUN_IN_OUT_PARALLEL 1

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

void Environment::execute_single_step(size_t avg_fps)
{
    scene_.text().update_blink();
#ifdef RUN_IN_OUT_PARALLEL
    std::thread ti([this]() { protocol_.execute_inputs(*input_queue_); });
    std::thread to([this]() { protocol_.execute_outputs(); });
    ti.join();
    to.join();
#else
    protocol_.execute_inputs(*input_queue_);
    protocol_.execute_outputs();
#endif
    show_fps_counter(avg_fps);
}

void Environment::show_fps_counter(size_t fps)
{
    if (show_fps_counter_) {
        if (fps > 999)
            fps = 999;
        scene_.text().write_text(
                scene_.text().lines() - 1,
                scene_.text().columns() - 9,
                " FPS " + std::to_string(fps) + " ",
                {COLOR_ORANGE, true, false});
    }
}

void Environment::show_error(std::exception const &e)
{
    static std::string press_enter = "-- Press ENTER to continue or Ctrl+F12 to quit --";
    scene_.text().write_text(scene_.text().lines() - 2, 0, e.what(), {COLOR_RED, true, false});
    scene_.text().write_text(scene_.text().lines() - 1, 0, press_enter, {COLOR_RED, true, false});
}

void Environment::reset()
{
    input_queue_->clear();
    output_queue_->clear();
    scene_.reset();
    protocol_.reset();
}

void Environment::set_mode(Mode mode)
{
    scene_.set_mode(mode);
}