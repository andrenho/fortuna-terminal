#include "application.hh"

#include <cstdio>
#include <optional>
#include <sstream>

#include "control.hh"

Application::Application(int argc, char* argv[])
    : options_(argc, argv),
      terminal_(options_.terminal_options),
      timing_debug_(options_.debug_time)
{
    environments.emplace_back(options_);
    current_env_idx = 0;

#ifdef COMM_PTY
    // environments.emplace_back(options_, CommType::PTY, Mode::Text);
#endif

    control_queue.emplace(ControlCommand::SetMode, options_.mode);

    gpio_.reset();  // restart computer
}

void Application::run()
{
retry:
    try {

        while (execute_single_step() == ExecutionStatus::Continue) {}

    } catch (std::exception& exception) {

        if (on_error(exception) == ExecutionStatus::Continue)
            goto retry;
    }
}

ExecutionStatus Application::execute_single_step()
{
    frame_start_ = Time::now();

    Environment& current_environment = environments.at(current_env_idx);

    timing_debug_.start_event(TimingDebug::Event::ControlQueue);
    execute_control_queue();

    for (auto& environment: environments)
        environment.execute_single_step(timing_debug_);

    timing_debug_.start_event(TimingDebug::Event::VSYNC);
    gpio_.vsync();

    timing_debug_.start_event(TimingDebug::Event::UserEvents);
    ExecutionStatus execution_status = terminal_.process_user_events(current_environment.events_interface());

    timing_debug_.start_event(TimingDebug::Event::Draw);
    terminal_.draw(current_environment.scene());

    timing_debug_.start_event(TimingDebug::Event::Wait);
    wait_until_end_of_frame();

    timing_debug_.end_frame();

    return execution_status;
}

ExecutionStatus Application::on_error(std::exception const& e)
{
    fprintf(stderr, "\e[1;31m%s\e0m\n", e.what());

    for (auto& environment: environments)
        environment.show_error(e);

    terminal_.draw(environments.at(current_env_idx).scene());

    ExecutionStatus execution_status = terminal_.wait_for_enter();

    if (execution_status == ExecutionStatus::Continue)
        control_queue.emplace(ControlCommand::Reset);

    return execution_status;
}

void Application::execute_control_queue()
{
    Environment& currrent_env = environments.at(current_env_idx);

    std::optional<Control> cc;
    while ((cc = control_queue.pop_nonblock()).has_value()) {

        switch (cc->command) {

            case ControlCommand::Reset:
                for (auto& environment: environments) {
                    terminal_.reset(environment.scene().unique_id());
                    environment.reset();
                }
                break;

            case ControlCommand::ResetProtocol:
                terminal_.reset(currrent_env.scene().unique_id());
                currrent_env.reset();
                break;

            case ControlCommand::SetMode:
                currrent_env.set_mode(cc->mode);
                terminal_.resize_window(currrent_env.scene());
                break;

            case ControlCommand::SetMouseActive:
                terminal_.set_mouse_active(cc->active);
                break;

            case ControlCommand::SetMouseMoveReport:
                terminal_.set_mouse_register_move(cc->active);
                break;

            case ControlCommand::SetJoystickEmulation:
                terminal_.set_joystick_emulation(cc->active);
                break;

            case ControlCommand::ResetComputer:
                gpio_.reset();
                break;
        }
    }
}

void Application::wait_until_end_of_frame()
{
    Duration frame_time = std::chrono::microseconds(1000000 / FPS);

    // since the sleep is not very precise, we sleep less than we need and let SDL's VSYNC take care of the rest
    Duration idle_wait_time = frame_time - 8ms;

    auto now = Time::now();
    if (now < (frame_start_ + idle_wait_time))
        std::this_thread::sleep_for(frame_start_ + idle_wait_time - now);

    while (Time::now() < (frame_start_ + frame_time))
        std::this_thread::yield();
}

