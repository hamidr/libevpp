#pragma once

#include "../event_loop/socket_watcher.h"
#include <memory>

namespace libevpp {
  namespace event_loop
  {
    typedef std::function<bool ()> timer_action;

    class event_loop_ev
    {
    public:
      using socket_identifier_t = std::unique_ptr<socket_watcher>;

    private:
      struct timer_watcher
      {
        timer_action timeout_cb;
        ev_timer timer;
        double time_;
        struct ev_loop* loop_;

        timer_watcher(struct ev_loop* loop, double time, timer_action&& cb)
          : timeout_cb(std::move(cb)), time_(time), loop_(loop)
        {
          ev_timer_init (&timer, &event_loop_ev::timer_handler, time, 0.);
          timer.data = this;
        }

        void start() {
          ev_timer_start (loop_, &timer);
        }

        void repeat() {
          timer.repeat = time_;
          ev_timer_again(loop_, &timer);
        }

        ~timer_watcher() {
          ev_timer_stop(loop_, &timer);
        }
      };


    public:
      event_loop_ev();
      event_loop_ev(struct ev_loop *);

      void run();

      socket_identifier_t watch(int);
      void unwatch(socket_identifier_t&);

      bool async_write(socket_identifier_t& id, action&& cb);
      bool async_read(socket_identifier_t& id, action&& cb);
      void async_timeout(double time, timer_action&& cb );

    private:
      static void timer_handler(EV_P_ ev_timer* w, int revents);

    private:
      struct ev_loop* loop_;
    };
  }
}
