#ifndef NONCOPYABLE_HH_
#define NONCOPYABLE_HH_

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable & operator=(NonCopyable const&) = delete;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

#endif //NONCOPYABLE_HH_
