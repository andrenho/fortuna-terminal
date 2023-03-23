#include "environment.hh"

#define RUN_IN_OUT_PARALLEL 1

using namespace std::string_literals;
using namespace std::chrono_literals;

template class SyncQueue<uint8_t>;

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options)),
      scene_(options.mode),
      protocol_(options.mode, scene_),
      show_timining_info_(options.debug_time)
{
    if (options.welcome_message)
        protocol_.execute_inputs(welcome_message());
}

void Environment::execute_single_step(FrameControl& frame_control)
{
    scene_.text().update_blink();

    frame_control.start_event(FrameControl::Event::ExecuteOutputs);
    std::string data_to_send = protocol_.execute_outputs();

    frame_control.start_event(FrameControl::Event::Communicate);
    std::string received_data = comm_->exchange(data_to_send);

    frame_control.start_event(FrameControl::Event::ExecuteInputs);
    protocol_.execute_inputs(received_data);

    frame_control.start_event(FrameControl::Event::DebuggingInfo);
    if (show_timining_info_)
        display_timing_info(frame_control);
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

void Environment::display_timing_info(FrameControl const &frame_control)
{
    std::map<FrameControl::Event, double> events = frame_control.last_events();
    if (events.empty())
        return;

    const size_t x = scene_.text().columns() - 23;

    auto print_event = [&](size_t y, std::string const& text, FrameControl::Event event) {
        char buf[64];
        snprintf(buf, sizeof buf, "%-16s%5.2fms", text.c_str(), events.at(event));
        scene_.text().write_text(y, x, buf, {COLOR_ORANGE, false, true});
        return y + 1;
    };

    size_t y = scene_.text().lines() - 12;
    y = print_event(y, "ControlQueue", FrameControl::Event::ControlQueue);
    y = print_event(y, "ExecuteOutputs", FrameControl::Event::ExecuteOutputs);
    y = print_event(y, "Communicate", FrameControl::Event::Communicate);
    y = print_event(y, "ExecuteInputs", FrameControl::Event::ExecuteInputs);
    y = print_event(y, "DebuggingInfo", FrameControl::Event::DebuggingInfo);
    y = print_event(y, "VSYNC", FrameControl::Event::VSYNC);
    y = print_event(y, "UserEvents", FrameControl::Event::UserEvents);
    y = print_event(y, "Draw", FrameControl::Event::Draw);
    y = print_event(y, "Wait", FrameControl::Event::Wait);
    y = print_event(y, "Interframe", FrameControl::Event::Interframe);

    scene_.text().write_text(
            scene_.text().lines() - 1,
            scene_.text().columns() - 9,
            " FPS " + std::to_string(frame_control.avg_fps()) + " ",
            {COLOR_ORANGE, true, true});

    if (comm_->is_overwhelmed()) {
        scene_.text().write_text(0, scene_.text().columns() - 3, "OVH", { COLOR_RED, true, true });
    }
}
