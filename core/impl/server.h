#include <iterator>
#include <algorithm>

namespace etEl {

namespace detail {
class WriteBufferEntry
{
public:
  const char *p_;
  std::size_t count_;
  DoneFunction done_;
  WriteBufferEntry(const char *p, std::size_t count, DoneFunction done)
    : p_(p),
    count_(count),
    done_(done)
  {}
};
} // namespace detail

template< class ProcessorT >
inline TConnection< ProcessorT >::TConnection(
  asio::io_service &service,
  std::shared_ptr< tcp::socket > socket)
  : socket_(socket),
  strand_(service)
{}

template< class ProcessorT >
inline void TConnection< ProcessorT >::serve()
{
  auto self(this->shared_from_this());
  asio::async_read_until(
    *socket_.get(),
    inbuf_,
    '\n',
    strand_.wrap([this, self](const system::error_code &error, std::size_t sz)
    {
      if (error) {
        return;
      }

      std::istream in(&inbuf_);
      if (processor_.process(*this, in, sz)) {
        serve();
      }
    }));
}

template< class ProcessorT >
inline void TConnection< ProcessorT >::write(const char *p,
  std::size_t sz,
  DoneFunction done)
{
  // Constraints on calling write guarantee no concurrent access.
  chunks_.push(new detail::WriteBufferEntry(p, sz, done));
  if (1 == chunks_.size()) {
    write();
  }
  // else
  // There is a pending write, upon completion of which a next
  // should be picked from the queue.
}

template< class ProcessorT >
inline void TConnection< ProcessorT >::write()
{
  assert(!chunks_.empty());

  auto self(this->shared_from_this());
  detail::WriteBufferEntry *b = chunks_.front();

  asio::async_write(*socket_.get(),
    asio::buffer(b->p_, b->count_),
    strand_.wrap([this, self, b](const system::error_code &e, std::size_t sz)
    {
      assert(!chunks_.empty());
      detail::WriteBufferEntry *b2 = chunks_.front();
      assert(b == b2);
      b->done_(e, sz);
      chunks_.pop();
      delete b;
      if (!chunks_.empty()) {
        write();
      }
    }));
}

template< class ProcessorT >
inline TServer< ProcessorT >::TServer(const std::string &host, const std::string &service)
  : service_(),
    acceptor_(service_),
    socket_()
{
  init(host, service);
  accept();
}

template< class ProcessorT >
inline void TServer< ProcessorT >::init(const std::string &host, const std::string &service)
{
  tcp::resolver resolver(service_);
  tcp::endpoint endpoint(*resolver.resolve({host, service}));
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
}

template< class ProcessorT >
inline void TServer< ProcessorT >::accept()
{
  socket_.reset(new tcp::socket(service_));
  acceptor_.async_accept(
    *socket_.get(),
    [this](const system::error_code &error)
    {
      if (!error) {
        std::shared_ptr< ConnectionType > c(new ConnectionType(service_, socket_));
        c->serve();
      }
      accept();
    });
}

template< class ProcessorT >
inline void TServer< ProcessorT >::run()
{
  service_.run();
}

} // namespace etEl
