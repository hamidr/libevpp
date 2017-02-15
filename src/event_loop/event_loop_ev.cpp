#include <libevpp/event_loop/event_loop_ev.h>

namespace libevpp {
namespace event_loop {

event_loop_ev::event_loop_ev()
  : loop_(EV_DEFAULT), loop_owner_(true)
{
}

event_loop_ev::event_loop_ev(struct ev_loop* loop)
  : loop_(loop), loop_owner_(false)
{
}

event_loop_ev::~event_loop_ev()
{
  if (loop_owner_)
    ev_loop_destroy(loop_);
}

void event_loop_ev::run()
{
  ev_run(loop_, 0);
}

bool event_loop_ev::async_write(socket_identifier_t& watcher, action&& cb)
{
  if (!watcher)
    return false;

  watcher->start_writing_with(std::move(cb));
  return true;
}

bool event_loop_ev::async_read(socket_identifier_t& watcher, action&& cb)
{
  if (!watcher)
    return false;

  watcher->start_reading_with(std::move(cb));
  return true;
}

void event_loop_ev::async_timeout(double time, timer_action&& cb )
{
  timer_watcher *w = new timer_watcher(loop_, time, std::move(cb));
  w->start();
}

event_loop_ev::socket_identifier_t event_loop_ev::watch(int fd)
{
  return std::make_unique<socket_watcher>(loop_, fd);
}

void event_loop_ev::unwatch(socket_identifier_t& id)
{
  if(id)
    id->stop();
}

}}
