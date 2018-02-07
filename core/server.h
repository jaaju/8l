#pragma once

#include "defaults.h"

#include <boost/asio.hpp>

#include <memory>
#include <string>
#include <iostream>
#include <type_traits>
#include <functional>
#include <queue>
#include <tuple>

namespace etEl {

namespace system = boost::system;
namespace asio = boost::asio;
namespace ip = asio::ip;
using ip::tcp;

template< class > class TServer;
namespace detail { class WriteBufferEntry; }

// Signature of function called when a write completes.
// See `TConnection< >::write`.
typedef std::function< void (
  const system::error_code &, // Any error when writing to client.
  std::size_t                 // Number of bytes successfully written.
) > DoneFunction;

// Connection Type.
// @tparam ProcessorT Connection processor type.
//  @interface-req
//    DefaultConstructible
//    Callable
//      Signature `process(TConnection< ProcessorT > &,
//                         std::istream &,
//                         std::size_t)`
//
// This class represents a connection from a client.
// Each connection holds a default-constructed  instance of `ProcessorT`.
//
// Data received over the connection is notified by a call to the method
// `ProcessorT::process` on this instance.
// A return of `false` from this method shall stop further processing
// of the connection and the underlying connection will be closed.
//
// No instance of `ProcessorT` shall receive multiple concurrent
// notifications, but there can be concurrent calls to multiple
// instances. So, any class-static data must be protected from concurrent
// accesses while instance data need not be.
//
// See `defaults::ProcessorD` for interface constraints imposed on `ProcessorT`.
template< class ProcessorT = defaults::ProcessorD< > >
class TConnection final
  : public std::enable_shared_from_this< TConnection< ProcessorT > >
{
public:
  typedef std::tuple< ProcessorT > ParameterTypes;

  TConnection(const TConnection &) = delete;
  TConnection &operator=(const TConnection &) = delete;

  typedef TConnection< ProcessorT > ThisType;

  static_assert(std::is_default_constructible< ProcessorT >::value,
    "Connection processor type must be default constructible.");

  // Method to write bytes to connected client.
  // @param bytes Pointer to bytes to write to client.
  // @param nbytes Number of bytes to copy from `bytes`.
  // @param done Function to call when write completes.
  //
  // This method must only be called from `ProcessorT::process` or
  // a write completion function, i.e. one provided as the `done`
  // argument to `write` in an earlier call.
  void write(const char *bytes, std::size_t nbytes, DoneFunction done);
private:
  std::shared_ptr< tcp::socket > socket_;
  asio::streambuf inbuf_;
  asio::io_service::strand strand_;
  ProcessorT processor_;

  TConnection(asio::io_service &, std::shared_ptr< tcp::socket >);
  void serve();
  friend class TServer< ProcessorT >;

  // These are used to serialize writes in order called by the processor.
  std::queue< detail::WriteBufferEntry * > chunks_;
  void write();
};

// Server.
// @tparam ProcessorT Connection processor type.
//  @interface-req Suitable for param0 of `TConnection`.
//
// This type provides a TCP server. Processing of a connection
// can be parameterized by the `ProcessorT` template parameter.
template< class ProcessorT = defaults::ProcessorD< > >
class TServer
{
public:
  typedef TConnection< ProcessorT > ConnectionType;

  // Construct.
  // @param host The host name or ip address on which to listen.
  // @param service The service ("http", e.g.) or port number as string ("80", for "http")
  //  omn which to listen.
  TServer(const std::string &host, const std::string &service);

  // Starts accepting connections.
  // This method blocks until the server is stopped (which is presently not supported).
  void run();
private:
  void init(const std::string &, const std::string &);
  void accept();

  asio::io_service service_;
  tcp::acceptor acceptor_;
  std::shared_ptr< tcp::socket > socket_;
}; // class server

} // namespace etEl

#include "impl/server.h"
