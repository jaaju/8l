#include "client.h"

#include <iostream>

namespace etEl {
namespace test {

Client::Client()
  : service_(),
  socket_(service_)
{}

void Client::run(const std::string &host, unsigned short port)
{
  tcp::resolver resolver(service_);
  auto i = resolver.resolve({host, std::to_string(port)});
  asio::connect(socket_, i);

  // Oops! Need '\n' since server is line-based.
  std::string m("hello!\n");
  asio::write(
    socket_,
    asio::buffer(m.c_str(), m.length()));

  system::error_code error;
  char buf[1024] = { 0 };
  while (!error) {
    std::size_t sz = socket_.read_some(asio::buffer(buf, 1024), error);
    response_.append(buf, sz);
  }
}

} // namespace test
} // namespace etEl
