#pragma once

#include <libevpp/event_loop/event_loop_ev.h>

#include <libevpp/network/tcp_socket.hpp>
#include <libevpp/network/unix_socket.hpp>
#include <unordered_map>

namespace libevpp {
namespace examples {

  class tcp_server
  {
  public:
    using tcp_socket   = libevpp::network::tcp_socket;
    using async_socket = libevpp::network::async_socket;

    tcp_server(event_loop::event_loop_ev &event_loop)
      : loop_(event_loop), listener_(event_loop) {
    }

    void listen(int port) {
      listener_.set_reuseport();
      listener_.set_reuseaddr();

      if (!listener_.bind("127.0.0.1", port) || !listener_.listen())
        throw;

      listener_.async_accept(std::bind(&tcp_server::accept, this, std::placeholders::_1));
    }

    void accept(std::shared_ptr<async_socket>& socket) {
      conns_.emplace(socket, nullptr);
      socket->async_read(buffer_, max_buffer_length, std::bind(&tcp_server::chunk_received, this, std::placeholders::_1, socket));

      listener_.async_accept(std::bind(&tcp_server::accept, this, std::placeholders::_1));
    }

  private:
    void chunk_received(int len, std::shared_ptr<async_socket>& socket)
    {
      if (len <= 0) {
        return;
      }

      std::string command;

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


      if (command == "close") {
        socket->async_write("good bye!", [this, &socket](ssize_t l) {
            loop_.async_timeout(1, [this, socket]() {
                conns_.erase(socket);
              });
          });
        return; // dont read
      }

      auto receiver = std::bind(&tcp_server::chunk_received, this, std::placeholders::_1, socket);
      socket->async_read(buffer_, max_buffer_length, receiver);
    }

  private:
    using socket_t = std::shared_ptr<async_socket>;
    std::unordered_map<socket_t, void*> conns_;

    tcp_socket listener_;
    event_loop::event_loop_ev& loop_;
    enum { max_buffer_length = 1024 };
    char buffer_[max_buffer_length];
  };

}
}
