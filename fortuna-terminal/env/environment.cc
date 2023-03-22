#include "environment.hh"

#define RUN_IN_OUT_PARALLEL 1

using namespace std::string_literals;
using namespace std::chrono_literals;

template class SyncQueue<uint8_t>;

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options)),
      scene_(options.mode),
      protocol_(options.mode, scene_),
      show_fps_counter_(options.terminal_options.show_fps_counter)
{
    if (options.welcome_message)
        protocol_.execute_inputs(welcome_message());
}

void Environment::execute_single_step(size_t avg_fps)
{
    scene_.text().update_blink();

    std::string data_to_send = protocol_.execute_outputs();
    std::string received_data = comm_->exchange(data_to_send);

    protocol_.execute_inputs(received_data);

    if (show_fps_counter_)
        show_fps_counter(avg_fps);
    if (comm_->is_overwhelmed())
        show_overwhelmed();
}

void Environment::show_fps_counter(size_t fps)
{
    if (fps > 999)
        fps = 999;
    scene_.text().write_text(
            scene_.text().lines() - 1,
            scene_.text().columns() - 9,
            " FPS " + std::to_string(fps) + " ",
            {COLOR_ORANGE, true, true});
}

void Environment::show_overwhelmed()
{
    scene_.text().write_text(0, scene_.text().columns() - 3, "OVH", { COLOR_RED, true, true });
}

void Environment::show_error(std::exception const &e)
{
    static std::string press_enter = "-- Press ENTER to continue or Ctrl+F12 to quit --";
    scene_.text().write_text(scene_.text().lines() - 2, 0, e.what(), {COLOR_RED, true, false});
    scene_.text().write_text(scene_.text().lines() - 1, 0, press_enter, {COLOR_RED, true, false});
}

void Environment::reset()
{
    scene_.reset();
    protocol_.reset();
}

void Environment::set_mode(Mode mode)
{
    scene_.set_mode(mode);
    protocol_.reset_mode();
}

std::string Environment::welcome_message() const
{
    std::stringstream ss;

    // version
    ss << "\e[2J\e[HFortuna terminal " VERSION "\n\n\r ";

    // colors
    for (size_t j = 30; j < 38; ++j)
        ss << "\e[2;7;" + std::to_string(j) + "m ";
    ss << "\e[0m\n\r ";
    for (size_t j = 30; j < 38; ++j)
        ss << "\e[0;7;" + std::to_string(j) + "m ";
    ss << "\e[0m\n\r ";
    for (size_t j = 30; j < 38; ++j)
        ss << "\e[1;7;" + std::to_string(j) + "m ";
    ss << "\e[0m\n\n\r";

    // communication module
    ss << "Communication module: \e[1;32m" + comm_->description() + "\e[0m\n\n\r";

    return ss.str();
}
