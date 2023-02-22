#ifndef KEYS_HH_
#define KEYS_HH_

enum class SpecialKey {
    ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, TAB, CAPSLOCK, WIN,
    INSERT, HOME, END, PAGEUP, PAGEDOWN, UP, DOWN, LEFT, RIGHT, ENTER, BACKSPACE,
    DELETE, PRINTSCREEN, PAUSEBREAK,
};

struct KeyMod {
    bool shift;
    bool control;
    bool alt;
};

#endif //KEYS_HH_
