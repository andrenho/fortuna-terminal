#include "timingdebug.hh"

#include <thread>

using namespace std::chrono_literals;

TimingDebug::TimingDebug(bool debug_time)
        : debug_time_(debug_time)
{
    for (size_t i = 0; i < static_cast<size_t>(Event::N_EVENTS); ++i)
        events_[static_cast<Event>(i)] = 0ms;
}

void TimingDebug::end_frame()
{
    if (!debug_time_)
        return;

    if (frame_count_ % FRAMES_BETWEEN_DURATION_CALC == 0) {
        last_events_ = events_;
        for (auto& [_, duration] : events_)
            duration = 0ms;
    }
    ++frame_count_;

    start_event(Event::Interframe);
}

void TimingDebug::start_event(TimingDebug::Event event)
{
    if (!debug_time_)
        return;

    auto now = Time::now();
    events_.at(current_event_) += now - last_event_time_;
    last_event_time_ = now;
    current_event_ = event;
}

size_t TimingDebug::avg_fps() const
{
    Duration duration = 0ms;
    for (auto const& [_, dur] : last_events_)
        duration += dur;
    double ms = (double) std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return (size_t) (1000.0 / (ms / FRAMES_BETWEEN_DURATION_CALC));
}

std::map<TimingDebug::Event, double> TimingDebug::last_events() const
{
    if (frame_count_ <= 30)  // avoid incorrect data in the first 0.5 seconds
        return {};

    std::map<Event, double> r;
    for (auto const& [ev, duration]: last_events_)
        r[ev] = ((double) std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0) / (double) FRAMES_BETWEEN_DURATION_CALC;
    return r;
}