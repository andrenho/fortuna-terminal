#include "fdcomm.hh"
#include "../exceptions/libcexception.hh"
#include "../exceptions/fortunaexception.hh"

#include <unistd.h>

static void duration_to_timeval(std::chrono::duration<double> duration, timeval& tv)
{
    double durationSeconds = duration.count();

    time_t seconds = static_cast<time_t>(durationSeconds);
    long microseconds = static_cast<long>((durationSeconds - seconds) * 1000000);

    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;
}

std::vector<uint8_t> FDComm::read_blocking(size_t n)
{
    return std::vector<uint8_t>();
}

std::vector<uint8_t> FDComm::read_for(std::chrono::duration<double> duration)
{
    return std::vector<uint8_t>();
}

void FDComm::write(std::vector<uint8_t> const &data)
{
    int n = ::write(fd_, data.data(), data.size());
    if (n == 0)
        action_on_rw_zero();
    else if (n < 0)
        throw LibcException("Error writing to file descriptor");
}
