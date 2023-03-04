#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include "comm/comm.hh"
#include "scene/scene.hh"
#include "common/syncqueue.hh"
#include "ansi.hh"
#include "keys.hh"
#include "gpio/gpio.hh"
#include "extra.hh"
#include "mode.hh"
#include "common/duration.hh"

class Protocol : public NonCopyable {
public:
    explicit Protocol(Mode mode, std::unique_ptr<CommunicationModule> comm, GPIO& gpio);

    void run();
    void execute_inputs();
    void vsync_tasks();
    void reset();

    void show_error(std::exception const& e);

    void finalize_threads();

    void event_text_input(std::string const& text);
    void event_key(uint8_t key, bool is_down, KeyMod mod);
    void event_key(SpecialKey key, bool is_down, KeyMod mod);
    void event_mouse_button(int button, int x, int y, bool is_down);
    void event_mouse_move(int button, int x, int y);
    void event_joystick(size_t joystick_number, size_t button, bool is_down);
    void event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value);

    void show_fps_counter(Duration duration);

    Scene const& scene() const { return scene_; }
    Scene&       scene() { return scene_; }

    void set_debug_comm(bool debug_comm) { debug_comm_ = debug_comm; }
    void set_mode(Mode mode);

private:
    std::unique_ptr<CommunicationModule> comm_ {};
    Scene scene_;
    ANSI  ansi_;
    Extra extra_;

    std::unique_ptr<std::thread> read_thread_ = nullptr;
    std::unique_ptr<std::thread> output_thread_ = nullptr;
    bool                         threads_active_ = true;

    std::unique_ptr<SyncQueue<uint8_t>> input_queue_ = std::make_unique<SyncQueue<uint8_t>>();
    std::unique_ptr<SyncQueue<uint8_t>> output_queue_ = std::make_unique<SyncQueue<uint8_t>>();

    bool debug_comm_ = false;
    Mode mode_;

    void debug_byte(bool is_input, uint8_t byte);

    struct JoystickState {
        bool up = false, down = false, left = false, right = false;
    };
    std::unordered_map<size_t, JoystickState> joy_state_ {};
};

#endif //PROTOCOL_HH_
