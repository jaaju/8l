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

  // This method blocks the calling thread.
  // It connects to host/service and sends "hello!\n".
  // Then it waits to read (anything) from the server, and completes.
  void run(const std::string &host, unsigned short port);

  const std::string &server_response() const;
private:
  asio::io_service service_;
  tcp::socket socket_;
  std::string response_;
};

inline const std::string &Client::server_response() const
{
  return response_;
}

} // namespace test
} // namespace etEl
