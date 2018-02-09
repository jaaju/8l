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
  std::shared_ptr< std::string > m(new std::string("hello!\n"));
  asio::async_write(
    socket_,
    asio::buffer(m->c_str(), m->length()),
    [m](const system::error_code &, std::size_t) {});

  socket_.async_read_some(
    asio::buffer(buf_, 1024),
    [this](const system::error_code &, std::size_t) {});
  service_.run();
}

} // namespace test
} // namespace etEl
