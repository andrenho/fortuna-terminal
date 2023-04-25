#include "environment.hh"

#define RUN_IN_OUT_PARALLEL 1

using namespace std::string_literals;
using namespace std::chrono_literals;

template class SyncQueue<uint8_t>;

Environment::Environment(Options const &options)
    : Environment(options, options.comm_type)
{
}

Environment::Environment(Options const &options, CommType comm_type)
        : comm_(CommunicationModule::create(comm_type, options)),
          scene_(),
          protocol_(scene_),
          show_timining_info_(options.debug_time)
{
    debug().info("Environment created for %s.", comm_->description().c_str());

    if (options.welcome_message)
        protocol_.execute_inputs(welcome_message());
}

void Environment::execute_single_step(TimingDebug& timing_debug)
{
    scene_.text().update_blink();

    timing_debug.start_event(TimingDebug::Event::ExecuteOutputs);
    std::string data_to_send = protocol_.execute_outputs();

    timing_debug.start_event(TimingDebug::Event::Communicate);
    std::string received_data = comm_->exchange(data_to_send);
    debug().bytes(received_data, data_to_send);

    timing_debug.bytes_sent = data_to_send.length();
    timing_debug.bytes_received = received_data.length();

    timing_debug.start_event(TimingDebug::Event::ExecuteInputs);
    protocol_.execute_inputs(received_data);

    timing_debug.start_event(TimingDebug::Event::DebuggingInfo);
    if (show_timining_info_)
        display_timing_info(timing_debug);
}

void Environment::show_error(std::exception const &e) const
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

void Environment::display_timing_info(TimingDebug const &timing_debug) const
{
    std::map<TimingDebug::Event, double> events = timing_debug.last_events();
    if (events.empty())
        return;

    const size_t x = scene_.text().columns() - 25;

    auto print_event = [&](size_t y, std::string const& text, TimingDebug::Event event) {
        char buf[64];
        snprintf(buf, sizeof buf, " %-16s%5.2fms ", text.c_str(), events.at(event));
        scene_.text().write_text(y, x, buf, {COLOR_ORANGE, true, true});
        return y + 1;
    };

    auto print_int = [&](size_t y, std::string const& text, size_t bytes) {
        char buf[64];
        snprintf(buf, sizeof buf, " %-16s%6zu ", text.c_str(), bytes);
        scene_.text().write_text(y, x, buf, {COLOR_ORANGE, true, true});
        return y + 1;
    };

    size_t y = scene_.text().lines() - 15;
    y = print_event(y, "ControlQueue", TimingDebug::Event::ControlQueue);
    y = print_event(y, "ExecuteOutputs", TimingDebug::Event::ExecuteOutputs);
    y = print_event(y, "Communicate", TimingDebug::Event::Communicate);
    y = print_event(y, "ExecuteInputs", TimingDebug::Event::ExecuteInputs);
    y = print_event(y, "DebuggingInfo", TimingDebug::Event::DebuggingInfo);
    y = print_event(y, "VSYNC", TimingDebug::Event::VSYNC);
    y = print_event(y, "UserEvents", TimingDebug::Event::UserEvents);
    y = print_event(y, "Draw", TimingDebug::Event::Draw);
    y = print_event(y, "Wait", TimingDebug::Event::Wait);
    y = print_event(y, "Interframe", TimingDebug::Event::Interframe);
    y = print_int(y + 1, "Bytes received", timing_debug.bytes_received);
    y = print_int(y, "Bytes sent", timing_debug.bytes_sent);

    scene_.text().write_text(
            scene_.text().lines() - 1,
            scene_.text().columns() - 10,
            "  FPS " + std::to_string(timing_debug.avg_fps()) + "  ",
            {COLOR_ORANGE, true, true});

    if (comm_->is_overwhelmed()) {
        scene_.text().write_text(0, scene_.text().columns() - 3, "OVH", { COLOR_RED, true, true });
    }
}