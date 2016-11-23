#include <event_loop/event_loop_ev.h>

#include <string>
#include <memory>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <network/tcp_socket.hpp>

template<typename InputOutputHandler>
class tcp_server
{
public:
  using tcp_socket   = io::network::tcp_socket<InputOutputHandler>;

  tcp_server(InputOutputHandler &event_loop)
    : loop_(event_loop) {
    listener_ = std::make_shared<tcp_socket>(event_loop);
  }

  void listen(int port) {
    if (!listener_->bind("0.0.0.0", port) || !listener_->listen())
      throw;

    auto receiver = std::bind(&tcp_server::accept, this, std::placeholders::_1);
    listener_->template async_accept<tcp_socket>(receiver);
  }

  void accept(std::shared_ptr<tcp_socket> socket) {
    auto receiver = std::bind(&tcp_server::chunk_received, this, std::placeholders::_1, socket);
    socket->async_read(buffer_, max_buffer_length, receiver);

    conns_.emplace(socket, nullptr);
  }

private:
  void chunk_received(int len, std::shared_ptr<tcp_socket>& socket)
  {
    std::string command;

    if (len <= 0) {
      conns_.erase(socket);
      return;
    }

    for(int n = 0; n < len; ++n) {

      char c = buffer_[n];
      switch(c)
      {
      case '\r':
      case '\n':
        continue;
        break;

      default:
        command.push_back(c);
      }
    }

    fprintf(stdout, ("cmd: " + command + "\n").data());
    fflush(stdout);

    if (command == "close") {
      socket->async_write("good bye!", [this, &socket](ssize_t len) {
          loop_.async_timeout(1, [this, &socket]() {
              conns_.erase(socket);
            });
        });
      return; // dont read
    }

    auto receiver = std::bind(&tcp_server::chunk_received, this, std::placeholders::_1, socket);
    socket->async_read(buffer_, max_buffer_length, receiver);
  }

private:
  using socket_t = std::shared_ptr<tcp_socket>;

  socket_t listener_;
  InputOutputHandler& loop_;
  std::unordered_map<socket_t, void*> conns_;
  enum { max_buffer_length = 1024 };
  char buffer_[max_buffer_length];
};

int main(int argc, char** args)
{
   if (argc != 2)
     return 0;

   io::event_loop::event_loop_ev loop;

   tcp_server<decltype(loop)> server(loop);
   int port  = std::stoi(args[1]);
   server.listen(port);

   loop.run();

  return 0;
}
