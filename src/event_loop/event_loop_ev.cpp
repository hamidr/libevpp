#include <libevpp/event_loop/event_loop_ev.h>

namespace libevpp {
namespace event_loop {

event_loop_ev::event_loop_ev()
  : loop_(EV_DEFAULT)
{
}

event_loop_ev::event_loop_ev(struct ev_loop* loop)
  : loop_(loop)
{
}

void event_loop_ev::run()
{
  ev_run (loop_, 0);
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

void event_loop_ev::async_timeout(double time, action&& cb )
{
  timer_watcher *w = new timer_watcher(time, cb);
  ev_timer_start (loop_, &w->timer);
}

void event_loop_ev::timer_handler(EV_P_ ev_timer* w, int revents)
{
  timer_watcher *watcher = reinterpret_cast<timer_watcher*>(w);
  watcher->timeout_cb();
  delete watcher;
}


event_loop_ev::socket_identifier_t event_loop_ev::watch(int fd)
{
  return std::make_shared<socket_watcher>(loop_, fd);
}

void event_loop_ev::unwatch(socket_identifier_t& id)
{
  if(id)
    id->stop();
}

}}
