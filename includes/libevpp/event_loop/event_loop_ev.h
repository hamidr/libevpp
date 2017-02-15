#pragma once

#include "../event_loop/socket_watcher.h"
#include "../event_loop/timer_watcher.h"
#include <memory>

namespace libevpp {
  namespace event_loop
  {

    class event_loop_ev
    {
    public:
      using socket_identifier_t = std::unique_ptr<socket_watcher>;

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
      struct ev_loop* loop_;
    };
  }
}
