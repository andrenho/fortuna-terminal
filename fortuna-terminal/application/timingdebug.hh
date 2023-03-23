#ifndef TIMINGDEBUG_HH_
#define TIMINGDEBUG_HH_

#include <map>

#include "common/types/time.hh"

using namespace std::chrono_literals;

class TimingDebug {
public:
    enum class Event {
        ControlQueue, ExecuteOutputs, Communicate, ExecuteInputs,
        DebuggingInfo, VSYNC, UserEvents, Draw, Wait, Interframe, N_EVENTS
    };

    explicit TimingDebug(bool debug_time);

    void start_event(Event event);
    void end_frame();

    [[nodiscard]] size_t avg_fps() const;
    [[nodiscard]] std::map<Event, double> last_events() const;

private:
    bool      debug_time_;

    size_t    frame_count_ = 0;

    TimePoint last_event_time_;
    Event current_event_ = Event::ControlQueue;
    std::map<Event, Duration> events_ {};
    std::map<Event, Duration> last_events_ {};

    static constexpr int FRAMES_BETWEEN_DURATION_CALC = 30;
};

#endif //TIMINGDEBUG_HH_
