#ifndef FRAMECONTROL_HH_
#define FRAMECONTROL_HH_

#include "common/duration.hh"

class FrameControl {
    using Time = decltype(std::chrono::high_resolution_clock::now());
public:
    void start_frame();
    void end_frame();

    [[nodiscard]] Duration avg_fps() const;

private:
    Time     frame_start_;
    Duration duration_ = std::chrono::milliseconds(0);
    Duration last_duration_ = std::chrono::milliseconds(0);
    size_t   frame_count_ = 0;

    static constexpr int DURATION_FRAMES = 30;
};

#endif //FRAMECONTROL_HH_
