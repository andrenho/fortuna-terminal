#include "terminal.hh"

Terminal::Terminal()
{

}

Terminal::~Terminal()
{

}

unsigned int Terminal::add_scene()
{
    scenes_.emplace_back();
    return scenes_.size() - 1;
}

void Terminal::do_events(SyncQueue<FP_Message> &event, bool *quit) const
{

}

void Terminal::draw() const
{

}

void Terminal::update_scene(SyncQueue<SceneEvent> &events)
{

}
