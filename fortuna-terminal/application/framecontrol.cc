#include "framecontrol.hh"

#include <thread>

#define WAIT_VSYNC 0

using namespace std::chrono_literals;
using hr = std::chrono::high_resolution_clock;

void FrameControl::start_frame()
{
    frame_start_ = hr::now();
}

void FrameControl::end_frame()
{
#if WAIT_VSYNC
    auto now = hr::now();
    if (now < (frame_start_ + 16.6ms))
        std::this_thread::sleep_for(frame_start_ + 16.6ms - now);
#endif
    duration_ += hr::now() - frame_start_;
    if (frame_count_ % DURATION_FRAMES == 0) {
        last_duration_ = duration_;
        duration_ = 0ms;
    }
    ++frame_count_;
}

Duration FrameControl::avg_fps() const
{
    return last_duration_ / DURATION_FRAMES;
}

