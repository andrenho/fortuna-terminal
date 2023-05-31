#include "debug.hh"

#include <cstdio>
#include <memory>
#include <vector>
#include <cstdarg>

#include "options.hh"

static std::unique_ptr<Debug> debug_ = nullptr;

Debug& debug()
{
    return *debug_;
}

void Debug::initialize(DebugVerbosity debug_verbosity)
{
    debug_ = std::unique_ptr<Debug>(new Debug(debug_verbosity));
}

void Debug::bytes(std::vector<uint8_t> const& received, std::vector<uint8_t> const& sent)
{
    if (debug_verbosity_ < V_COMM)
        return;

    std::vector<std::tuple<std::vector<uint8_t>, int, const char*>> blocks { { received, 33, "<<" }, { sent, 32, ">>" } };
    for (auto const& [str, color, direction] : blocks) {
        if (!str.empty()) {
#if COLOR_TERMINAL
            printf("\e[0;%dm", color);
#else
            printf("%s ", direction);
#endif
            for (uint8_t c : str) {
                if (c < 32 || c > 127)
                    printf("[%02X]", c);
                else
                    printf("%c", c);
            }
#if COLOR_TERMINAL
            printf("\e[0m");
#endif
        }
    }
    if((received.empty() && sent.empty()) == false)
        printf("\n");
    fflush(stdout);
}

Debug::Debug(DebugVerbosity debug_verbosity)
    : debug_verbosity_(debug_verbosity)
{
    info("Debugging started with level %d.", debug_verbosity);
}

void Debug::log_(DebugVerbosity verbosity, char const *fmt, ...)
{
    if (verbosity <= debug_verbosity_) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
    }
    if (debug_verbosity_ >= V_DEBUG)
        fflush(stdout);
}

void Debug::log_(std::string const& color, DebugVerbosity verbosity, char const *fmt, ...)
{
    (void) color;

    if (verbosity <= debug_verbosity_) {
        va_list args;
        va_start(args, fmt);
#if COLOR_TERMINAL
        printf("\e[%sm", color.c_str());
#endif
        vprintf(fmt, args);
#if COLOR_TERMINAL
        printf("\e[0m");
#endif
        va_end(args);
        printf("\n");
    }
    if (debug_verbosity_ >= V_DEBUG)
        fflush(stdout);
}

static std::string key_mod(KeyMod mod)
{
    std::string k;
    if (mod.control) k += "CTRL + ";
    if (mod.alt)     k += "SHIFT + ";
    if (mod.shift)   k += "ALT + ";
    return k;
}

std::string Debug::key_name(uint8_t key, KeyMod mod)
{
    if (key >= 32 && key <= 126)
        return key_mod(mod) + (char) key;
    else
        return { key_mod(mod) + "[" + std::to_string(key) + "]" };
}

std::string Debug::key_name(SpecialKey key, KeyMod mod)
{
    std::string k = key_mod(mod);
    switch (key) {
        case SpecialKey::ESC: k += "ESC"; break;
        case SpecialKey::F1: k += "F1"; break;
        case SpecialKey::F2: k += "F2"; break;
        case SpecialKey::F3: k += "F3"; break;
        case SpecialKey::F4: k += "F4"; break;
        case SpecialKey::F5: k += "F5"; break;
        case SpecialKey::F6: k += "F6"; break;
        case SpecialKey::F7: k += "F7"; break;
        case SpecialKey::F8: k += "F8"; break;
        case SpecialKey::F9: k += "F9"; break;
        case SpecialKey::F10: k += "F10"; break;
        case SpecialKey::F11: k += "F11"; break;
        case SpecialKey::F12: k += "F12"; break;
        case SpecialKey::TAB: k += "TAB"; break;
        case SpecialKey::CAPSLOCK: k += "CAPSLOCK"; break;
        case SpecialKey::WIN: k += "WIN"; break;
        case SpecialKey::INSERT: k += "INSERT"; break;
        case SpecialKey::HOME: k += "HOME"; break;
        case SpecialKey::END: k += "END"; break;
        case SpecialKey::PAGEUP: k += "PAGEUP"; break;
        case SpecialKey::PAGEDOWN: k += "PAGEDOWN"; break;
        case SpecialKey::UP: k += "UP"; break;
        case SpecialKey::DOWN: k += "DOWN"; break;
        case SpecialKey::LEFT: k += "LEFT"; break;
        case SpecialKey::RIGHT: k += "RIGHT"; break;
        case SpecialKey::ENTER: k += "ENTER"; break;
        case SpecialKey::BACKSPACE: k += "BACKSPACE"; break;
        case SpecialKey::DELETE: k += "DELETE"; break;
        case SpecialKey::PRINTSCREEN: k += "PRINTSCREEN"; break;
        case SpecialKey::PAUSEBREAK: k += "PAUSEBREAK"; break;
    }
    return k;
}
