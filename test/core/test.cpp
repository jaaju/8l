#include "core/test.h"
#include "common/tester.h"
#include "common/client.h"

#include "core/server.h"

#include <thread>

namespace etEl {
namespace test {

// The 'done' function must be called when write completes.
bool testDoneFunction()
{

  class TestProcessor: public defaults::ProcessorD< TestProcessor >
  {
  public:
    TestProcessor(bool &called): called_(called) {}
    bool &called_;

    bool process(ConnectionType &connection, std::istream &, std::size_t)
    {
      // write something to client and set flag when 'done' is called.
      std::shared_ptr< std::string > m(new std::string("hello!"));
      connection.write(m->c_str(), m->length(),
        [this, m](const system::error_code &, std::size_t) { called_ = true; });
      return false;
    }

    class FactoryType
    {
    public:
      FactoryType(const FactoryType &) = default;
      FactoryType(bool &called):
        called_(called) {}
      bool &called_;
      TestProcessor operator ()() const
      {
        return TestProcessor(called_);
      }
    };

  };

  // Start server and client in two different threads.
  bool called = false;
  TServer< TestProcessor > server(
    "localhost", "0",
    TestProcessor::FactoryType(called));
  std::thread tserver(&TServer< TestProcessor >::run, std::ref(server));

  using std::placeholders::_1;
  Client client;
  std::thread tclient(std::bind(&Client::run, _1, "localhost", server.port()),
                      std::ref(client));

  // Wait for client to complete, which should write something to server
  // and read server's response.
  tclient.join();

  // Stop server, and wait.
  kill(getpid(), SIGUSR1);
  tserver.join();

  // 'called' must be set.
  etElAssert(called, "Done function not called!");
  return true;
}

bool runTests(bool stopOnFailure)
{
  using test::test;
  return test(stopOnFailure,
              "Test 'DoneFunction'", testDoneFunction);
}

} // namespace test
} // namespace etEl
