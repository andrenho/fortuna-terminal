#include "protocol.hh"

Protocol::Protocol(Mode mode, Scene& scene)
    : ansi_(mode, scene),
      extra_(mode, scene)
{
}
