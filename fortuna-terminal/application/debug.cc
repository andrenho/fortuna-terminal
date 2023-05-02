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

void Debug::bytes(std::string_view received, std::string_view sent)
{
    if (debug_verbosity_ < V_COMM)
        return;

    std::vector<std::tuple<std::string_view, int, const char*>> blocks { { received, 33, "<<" }, { sent, 32, ">>" } };
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
