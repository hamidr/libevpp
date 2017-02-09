#include <libevpp/network/async_socket.hpp>

#include <sys/fcntl.h> // fcntl
#include <unistd.h> // close

namespace libevpp {
namespace network {

async_socket::async_socket(event_loop::event_loop_ev& io)
  : io_(io)
{ }

async_socket::~async_socket() {
  close();
}

bool async_socket::is_valid() {
  return fd_ != -1;
}

ssize_t async_socket::send(const string& data) {
  return send(data.data(), data.size());
}

ssize_t async_socket::send(const char *data, size_t len) {
  return ::send(fd_, data, len, 0);
}

bool async_socket::set_reuseport()
{
  int enable = 1;
  return (setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) != -1);
}

bool async_socket::set_reuseaddr()
{
  int enable = 1;
  return (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != -1);
}

ssize_t async_socket::receive(char *data, size_t len)
{
  return ::recv(fd_, data, len, 0);
}

bool async_socket::listen(int backlog)
{
  return ::listen(fd_, backlog) == 0;
}

int async_socket::accept() {
  return ::accept(fd_, nullptr, nullptr);
}

bool async_socket::close()
{
  if (!is_connected_)
    return true;

  io_.unwatch(id_);

  auto res = ::close(fd_) == 0;
  is_connected_ = false;
  fd_ = -1;
  return res;
}

bool async_socket::async_write(const string& data, ready_cb_t&& fn)
{
  if (!is_connected() || !data.size())
    return false;

  return io_.async_write(id_, std::bind(&async_socket::handle_write, this, data, std::move(fn)));
}


void async_socket::handle_write(const string& data, const ready_cb_t& cb)
{
  auto sent_chunk = send(data);

  if(sent_chunk == 0)
    close();

  if (sent_chunk < data.size() && sent_chunk != -1) {
    // async_write(data.substr(sent_chunk, data.size()), std::move(cb));
    return;
  }

  cb(sent_chunk);
}


bool async_socket::async_read(char *buffer, int max_len, recv_cb_t&& cb)
{
  if (!is_connected())
    return false;

  return io_.async_read(id_, std::bind(&async_socket::handle_read, this, buffer, max_len, std::move(cb)));
}

void async_socket::handle_read(char* buffer, int len, const recv_cb_t& cb)
{
  auto l = receive(buffer, len);
  if (!l)
    close();

  cb(l);
}

bool async_socket::async_accept(accept_cb_t&& cb)
{
  if (!is_connected())
    return false;

  return io_.async_read(id_, std::bind(&async_socket::handle_accept, this, std::move(cb)));
}

void async_socket::handle_accept(const accept_cb_t& cb)
{
  int fd = accept();
  auto s = std::make_shared<async_socket>(io_);
  s->set_fd_socket(fd);
  cb(s);
}

bool async_socket::is_connected() const
{
  return is_connected_;
}

void async_socket::set_fd_socket(int fd)
{
  fd_ = fd;
  is_connected_ = true;

  id_ = io_.watch(fd_);
}


void async_socket::create_socket(int domain)
{
  if (-1 == (fd_ = socket(domain, SOCK_STREAM, 0)))
    throw connect_socket_exception();

  if (-1 == fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK))
    throw nonblocking_socket_exception();

  id_ = io_.watch(fd_);
}

int async_socket::connect_to(async_socket::socket_t* socket_addr, int len)
{
  int ret = ::connect(fd_, socket_addr, len);
  if (!ret)
    is_connected_ = true;

  return ret;
}

int async_socket::bind_to(async_socket::socket_t* socket_addr, int len)
{
  int b = ::bind(fd_, socket_addr, len);
  if (!b)
    is_connected_ = true;

  return b;
}

}}
