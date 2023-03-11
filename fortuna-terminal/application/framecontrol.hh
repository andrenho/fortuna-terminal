#ifndef FRAMECONTROL_HH_
#define FRAMECONTROL_HH_

#include "common/time.hh"

using namespace std::chrono_literals;

class FrameControl {
public:
    void start_frame();
    void end_frame();

    [[nodiscard]] size_t avg_fps() const;

private:
    TimePoint frame_start_;
    Duration  duration_ = 0ms;
    Duration  last_duration_ = 0ms;
    size_t    frame_count_ = 0;

    static constexpr int FRAMES_BETWEEN_DURATION_CALC = 30;
};

#endif //FRAMECONTROL_HH_
