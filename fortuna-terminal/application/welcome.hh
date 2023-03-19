#ifndef WELCOME_HH_
#define WELCOME_HH_

#include <string>

#include "common/syncqueue.hh"

void welcome_message(SyncQueueByte& input_queue, std::string const& comm_desc);

#endif //WELCOME_HH_