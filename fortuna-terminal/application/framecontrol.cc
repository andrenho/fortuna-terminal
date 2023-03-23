#include "framecontrol.hh"

#include <thread>

using namespace std::chrono_literals;

FrameControl::FrameControl()
{
    for (size_t i = 0; i < static_cast<size_t>(Event::N_EVENTS); ++i)
        events_[static_cast<Event>(i)] = 0ms;
}

void FrameControl::start_frame(Event event)
{
    frame_start_ = Time::now();
    last_event_time_ = frame_start_;

    start_event(event);
}

void FrameControl::end_frame()
{
    wait_for_end_of_frame(30);

    if (frame_count_ % FRAMES_BETWEEN_DURATION_CALC == 0) {
        last_events_ = events_;
        for (auto& [_, duration] : events_)
            duration = 0ms;
    }
    ++frame_count_;

    start_event(Event::Interframe);
}

void FrameControl::start_event(FrameControl::Event event)
{
    auto now = Time::now();
    events_.at(current_event_) += now - last_event_time_;
    last_event_time_ = now;
    current_event_ = event;
}

size_t FrameControl::avg_fps() const
{
    Duration duration = 0ms;
    for (auto const& [_, dur] : last_events_)
        duration += dur;
    double ms = (double) std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return (size_t) (1000.0 / (ms / FRAMES_BETWEEN_DURATION_CALC));
}

std::map<FrameControl::Event, double> FrameControl::last_events() const
{
    std::map<Event, double> r;
    for (auto const& [ev, duration]: last_events_)
        r[ev] = ((double) std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0) / (double) FRAMES_BETWEEN_DURATION_CALC;
    return r;
}

void FrameControl::wait_for_end_of_frame(int fps)
{
    start_event(Event::Wait);

    Duration frame_time = std::chrono::microseconds(1000000 / fps);

    // since the sleep is not very precise, we sleep less than we need and let SDL's VSYNC take care of the rest
    Duration idle_wait_time = frame_time - 8ms;

    auto now = Time::now();
    if (now < (frame_start_ + idle_wait_time))
        std::this_thread::sleep_for(frame_start_ + idle_wait_time - now);

    while (Time::now() < (frame_start_ + frame_time))
        std::this_thread::yield();
}
