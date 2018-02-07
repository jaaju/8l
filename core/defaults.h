#pragma once

#include <iostream>
#include <algorithm>
#include <iterator>

namespace etEl {

template< class > class TConnection;

namespace defaults {

// A default implementation of template param0 (`ProcessorT`)
// of template `etEl::TConnection`.
// @tparam ProcessorT The parameter type that extends this class.
//
// An argument to param0 of `etEl::TConnection` may extend this
// class and override functionality selectively.
//
// This class may also be used directly as an argument. Such use
// gives a server that waits for a connected client to write some
// bytes. The connection is closed after reading the request bytes.
template< class ProcessorT = void >
class ProcessorD
{
public:
  // @interface-req DefaultConstructible of param0 of `etEl::TConnection`.
  ProcessorD() = default;

  typedef typename std::conditional<
      std::is_same< void, ProcessorT >::value,
      ProcessorD,
      ProcessorT >::type ProcessorType;

  // Method that receives notification of data received from
  // a connected client.
  // @interface-req Callable of param0 of `etEl::TConnection`.
  //
  // @param connection Reference to the connection that calls this method.
  // @param in Reference to input stream that contains client data.
  // @param nbytes Number of bytes in the stream.
  // @return bool A `false` indicates that the connection must stop further
  //  processing and close the connection.
  //
  // This method is called from a connection when there is data available
  // from the client. `ProcessorT` may override this method to get the
  // notifications.
  //
  // All data from a connection will be notified to the
  // same instance, and there will not be multiple concurrent calls to
  // any single instance. Any data left unread in `in` will be available
  // in subsequent calls to this method.
  //
  // Method `write` (see `etEl::TConnection`) on `connection` can be called
  // to send any data back to the connected client.
  bool process(TConnection< ProcessorType > &connection,
    std::istream &in,
    std::size_t nbytes) const
  {
    // Not reading any data since we return false to close the connection.
    return false;
  }
};

} // namespace defaults
} // namespace etEl
