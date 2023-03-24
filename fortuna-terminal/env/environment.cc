#include "environment.hh"

#define RUN_IN_OUT_PARALLEL 1

using namespace std::string_literals;
using namespace std::chrono_literals;

template class SyncQueue<uint8_t>;

Environment::Environment(Options const &options)
    : comm_(CommunicationModule::create(options)),
      scene_(options.mode),
      protocol_(options.mode, scene_),
      show_timining_info_(options.debug_time),
      debug_comm_(options.debug_comm)
{
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

    timing_debug.start_event(TimingDebug::Event::ExecuteInputs);
    protocol_.execute_inputs(received_data);

    timing_debug.start_event(TimingDebug::Event::DebuggingInfo);
    if (show_timining_info_)
        display_timing_info(timing_debug);
    if (debug_comm_)
        debug_bytes(received_data, data_to_send);
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

    size_t y = scene_.text().lines() - 12;
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

    scene_.text().write_text(
            scene_.text().lines() - 1,
            scene_.text().columns() - 10,
            "  FPS " + std::to_string(timing_debug.avg_fps()) + "  ",
            {COLOR_ORANGE, true, true});

    if (comm_->is_overwhelmed()) {
        scene_.text().write_text(0, scene_.text().columns() - 3, "OVH", { COLOR_RED, true, true });
    }
}

void Environment::debug_bytes(std::string_view received, std::string_view sent) const
{
    std::vector<std::tuple<std::string_view, int, const char*>> blocks { { received, 33, "<<" }, { sent, 32, ">>" } };
    for (auto const& [str, color, direction] : blocks) {
        if (!str.empty()) {
#if COLOR_TERMINAL
            printf("\e[0;%dm", color);
#else
            printf("%s ", direction);
#endif
            for (uint8_t c : str) {
                if (c < 32 || c > 127)
                    printf("[%02X]", c);
                else
                    printf("%c", c);
            }
#if COLOR_TERMINAL
            printf("\e[0m");
#endif
        }
    }
    if((received.empty() && sent.empty()) == false)
        printf("\n");
    fflush(stdout);
}
