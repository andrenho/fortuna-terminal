#ifndef DEBUG_HH_
#define DEBUG_HH_

#include <cstdint>
#include <string>
#include <vector>

#include "scene/ievent.hh"

enum DebugVerbosity { V_NORMAL = 1, V_INFO = 2, V_DEBUG = 3, V_COMM = 4, MAX_DEBUG_VERBOSITY = 5 };

class Debug {
public:
    template <typename ...Args>
    void log(const char* fmt, Args&&... args) { log_(V_INFO, fmt, std::forward<Args>(args)...); }

    template <typename ...Args>
    void info(const char* fmt, Args&&... args) { log_(V_INFO, fmt, std::forward<Args>(args)...); }

    template <typename ...Args>
    void warning(const char* fmt, Args&&... args) { log_("1;31", V_NORMAL, fmt, std::forward<Args>(args)...); }

    template <typename ...Args>
    void debug(const char* fmt, Args&&... args) { log_(V_DEBUG, fmt, std::forward<Args>(args)...); }

    void bytes(std::vector<uint8_t> const& received, std::vector<uint8_t> const& sent);

    [[nodiscard]] DebugVerbosity debug_verbosity() const { return debug_verbosity_; }

    static void initialize(DebugVerbosity debug_verbosity);

    static std::string key_name(uint8_t key, KeyMod mod);
    static std::string key_name(SpecialKey key, KeyMod mod);

private:
    explicit Debug(DebugVerbosity debug_verbosity);

    void log_(DebugVerbosity verbosity, const char* fmt, ...);
    void log_(std::string const& color, DebugVerbosity verbosity, const char* fmt, ...);

    DebugVerbosity debug_verbosity_;
};

Debug& debug();

#endif //DEBUG_HH_
