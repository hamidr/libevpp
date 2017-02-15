#pragma once

#include <ev.h>
#include <functional>

namespace libevpp {
namespace event_loop {

typedef std::function<bool ()> timer_action;

class timer_watcher
{
 public:
  timer_watcher(struct ev_loop* loop, double time, timer_action&& cb);
  ~timer_watcher();

  void start();
  void repeat();
  void stop();

 private:
  static void timer_handler(EV_P_ ev_timer* w, int revents);

 private:
  timer_action&& timeout_cb;
  ev_timer timer;
  double time_;
  struct ev_loop* loop_;
};


}
}
