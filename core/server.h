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
// See 'TConnection< >::write'.
typedef std::function< void (
  const system::error_code &, // Any error when writing to client.
  std::size_t                 // Number of bytes successfully written.
) > DoneFunction;

// Connection Type.
// @tparam ProcessorT Connection processor type.
//  @interface-req
//    Typename 'FactoryType'
//      Provides member type (nested class or typedef) 'FactoryType'.
//      This type will be used as the factory of 'ProcessorT'.
//    FactoryDefaultConstructible or FactoryCopyConstructible
//      'FactoryType' must be default constructible.
//    DefaultConstructibleViaFactory
//      if 'f' is an instance of 'FctoryType', 'f()' must return an instance of 'ProcessorT'.
//    CopyConstructible
//    Callable
//      Signature 'process(TConnection< ProcessorT > &,
//                         std::istream &,
//                         std::size_t)'
//
// This class represents a connection from a client.
// Each connection holds a default-constructed  instance of 'ProcessorT'.
//
// Data received over the connection is notified by a call to the method
// 'ProcessorT::process' on this instance.
// A return of 'false' from this method shall stop further processing
// of the connection and the underlying connection will be closed.
//
// No instance of 'ProcessorT' shall receive multiple concurrent
// notifications, but there can be concurrent calls to multiple
// instances. So, any class-static data must be protected from concurrent
// accesses while instance data need not be.
//
// See 'defaults::ProcessorD' for interface constraints imposed on 'ProcessorT'.
template< class ProcessorT = defaults::ProcessorD< > >
class TConnection final
  : public std::enable_shared_from_this< TConnection< ProcessorT > >
{
public:
  typedef std::tuple< ProcessorT > ParameterTypes;

  TConnection(const TConnection &) = delete;
  TConnection &operator=(const TConnection &) = delete;

  typedef TConnection< ProcessorT > ThisType;

  static_assert(std::is_copy_constructible< ProcessorT >::value,
    "Connection processor type must be copyable.");

  typedef typename ProcessorT::FactoryType ProcessorFactory;
  static_assert(std::is_default_constructible< ProcessorFactory >::value ||
                std::is_copy_constructible< ProcessorFactory >::value,
    "Processor factory type must be default/copy constructible.");

  // Method to write bytes to connected client.
  // @param bytes Pointer to bytes to write to client.
  // @param nbytes Number of bytes to copy from 'bytes'.
  // @param done Function to call when write completes.
  //
  // This method must only be called from 'ProcessorT::process' or
  // a write completion function, i.e. one provided as the 'done'
  // argument to 'write' in an earlier call.
  //
  // The buffer pointed to by 'bytes' must be available until the asynchronous
  // write completes, and it is the caller's responsibility to keep it around.
  // Typically, the caller would release the buffer in the 'done' callback.
  void write(const char *bytes, std::size_t nbytes, DoneFunction done);
private:
  std::shared_ptr< tcp::socket > socket_;
  asio::streambuf inbuf_;
  asio::io_service::strand strand_;
  ProcessorT processor_;

  TConnection(asio::io_service &, std::shared_ptr< tcp::socket >, const ProcessorFactory &);
  void serve();
  friend class TServer< ProcessorT >;

  // These are used to serialize writes in order called by the processor.
  std::queue< detail::WriteBufferEntry * > chunks_;
  void write();
};

// Server.
// @tparam ProcessorT Connection processor type.
//  @interface-req Suitable for param0 of 'TConnection'.
//
// This type provides a TCP server. Processing of a connection
// can be parameterized by the 'ProcessorT' template parameter.
template< class ProcessorT = defaults::ProcessorD< > >
class TServer final
{
public:
  typedef TConnection< ProcessorT > ConnectionType;

  TServer(const TServer &) = delete;
  TServer &operator=(const TServer &) = delete;

  typedef typename ProcessorT::FactoryType ProcessorFactory;

  // Construct.
  // @param host The host name or ip address on which to listen.
  // @param service The service ("http", e.g.) or port number as string ("80", for "http")
  //  on which to listen. If this parameter is privided an argument of "0" the server should
  //  listen on an available local port. The port the server listens on can be obtained by
  //  calling the 'port()' method.
  TServer(const std::string &host, const std::string &service,
          const ProcessorFactory &factory = ProcessorFactory());

  // Starts accepting connections.
  // This method blocks until the server is stopped (which is presently not supported).
  void run();

  // Returns listening port.
  unsigned short port() const;
private:
  void init(const std::string &, const std::string &);
  void accept();

  asio::io_service service_;
  tcp::acceptor acceptor_;
  std::shared_ptr< tcp::socket > socket_;
  asio::signal_set signals_;

  ProcessorFactory factory_;
}; // class server

} // namespace etEl

#include "impl/server.h"
