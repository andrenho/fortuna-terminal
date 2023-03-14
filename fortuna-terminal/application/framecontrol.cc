#include "framecontrol.hh"

#define WAIT_VSYNC_MANUALLY 0

#if WAIT_VSYNC_MANUALLY
#  include <thread>
#endif

using namespace std::chrono_literals;

void FrameControl::start_frame()
{
    frame_start_ = Time::now();
}

void FrameControl::end_frame()
{
#if WAIT_VSYNC_MANUALLY
    auto now = Time::now();
    if (now < (frame_start_ + 16.6ms))
        std::this_thread::sleep_for(frame_start_ + 16.6ms - now);
#endif
    duration_ += (Time::now() - frame_start_);
    if (frame_count_ % FRAMES_BETWEEN_DURATION_CALC == 0) {
        last_duration_ = duration_;
        duration_ = 0ms;
    }
    ++frame_count_;
}

size_t FrameControl::avg_fps() const
{
    Duration duration = last_duration_ / FRAMES_BETWEEN_DURATION_CALC;
    double ms = (double) std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return (size_t) (1000.0 / ms);
}

