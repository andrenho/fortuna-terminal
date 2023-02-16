
#include "terminal.hh"

unsigned int Terminal::add_scene()
{
    scenes_.emplace_back();
    return scenes_.size() - 1;
}

void Terminal::update_scene(unsigned int scene_n, FP_Message &&message)
{

}

void Terminal::do_events(SyncQueue<FP_Message> &event, bool *quit) const
{

}

void Terminal::draw() const
{

}

