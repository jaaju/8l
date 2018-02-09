#pragma once

#include <boost/asio.hpp>

namespace etEl {

namespace system = boost::system;
namespace asio = boost::asio;
namespace ip = asio::ip;
using ip::tcp;

namespace test {

class Client
{
public:
  Client();

  // Connects to host/service and sends "hello!\n".
  // Waits to read (anything) from the server, and completes.
  void run(const std::string &host, unsigned short port);
private:
  asio::io_service service_;
  tcp::socket socket_;
  char buf_[1024];
};

} // namespace test
} // namespace etEl
