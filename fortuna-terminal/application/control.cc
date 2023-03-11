#include "control.hh"
#include "common/syncqueue.hh"

SyncQueue<Control> control;

template class SyncQueue<Control>;