#include <libevpp/event_loop/timer_watcher.h>

namespace libevpp {
namespace event_loop {

timer_watcher::timer_watcher(struct ev_loop* loop, double time, timer_action&& cb)
  : timeout_cb(std::move(cb)), time_(time), loop_(loop)
{
  ev_timer_init (&timer, &timer_watcher::timer_handler, time, 0.);
  timer.data = this;
}

timer_watcher::~timer_watcher() {
  stop();
}

void timer_watcher::start() {
  ev_timer_start (loop_, &timer);
}

void timer_watcher::stop() {
  ev_timer_stop(loop_, &timer);
}

void timer_watcher::repeat() {
  timer.repeat = time_;
  ev_timer_again(loop_, &timer);
}

void timer_watcher::timer_handler(EV_P_ ev_timer* w, int revents)
{
  timer_watcher *watcher = reinterpret_cast<timer_watcher*>(w->data);
  if (!watcher->timeout_cb()) {
    delete watcher;
    return;
  }
  watcher->repeat();
}

}
}
