#include <event_loop/event_loop_ev.h>

#include <network/tcp_socket.hpp>
#include <network/unix_socket.hpp>
#include "../examples/tcp_server.hpp"


int main(int argc, char** args)
{
  libevpp::event_loop::event_loop_ev loop;

  libevpp::examples::tcp_server server(loop);
  server.listen(9090);


  loop.run();
  return 0;
}
