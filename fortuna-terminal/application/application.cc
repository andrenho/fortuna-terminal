#include "application.hh"

#include <cstdio>
#include <optional>

#include "control.hh"

Application::Application(int argc, char* argv[])
    : options_(argc, argv),
      terminal_(options_.terminal_options)
{
    environments.emplace_back(options_);
    current_env_idx = 0;

    for (auto& environment: environments)
        environment.run_comm_threads(options_.debug_comm);

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

    for (auto& environment: environments)
        environment.finalize_comm_threads();
}

ExecutionStatus Application::execute_single_step()
{
    frame_control_.start_frame();

    Environment& currrent_environment = environments.at(current_env_idx);

    execute_control_queue();

    for (auto& environment: environments)
        environment.execute_single_step(frame_control_.avg_fps());

    gpio_.vsync();

    ExecutionStatus execution_status = terminal_.process_user_events(currrent_environment.events_interface());
    terminal_.draw(currrent_environment.scene());

    frame_control_.end_frame();

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

