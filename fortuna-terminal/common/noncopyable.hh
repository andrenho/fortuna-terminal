#ifndef NONCOPYABLE_HH_
#define NONCOPYABLE_HH_

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable & operator=(NonCopyable&) = default;
    NonCopyable & operator=(const NonCopyable&&) = delete;
    NonCopyable(const NonCopyable&) = delete;
};

#endif //NONCOPYABLE_HH_
