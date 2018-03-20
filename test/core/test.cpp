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

  static const char *chunks[3] = { "one", "two", "three" };
  std::string server_response;
  std::for_each(chunks, chunks + sizeof(chunks)/sizeof(chunks[0]),
    [&server_response](const char *r)
    { server_response.append(r); });

  class TestProcessor: public defaults::ProcessorD< TestProcessor >
  {
  public:
    TestProcessor(int &called): called_(called) {}
    int &called_;

    bool process(ConnectionType &connection, std::istream &, std::size_t)
    {
      // write something to client and set flag when 'done' is called.
      write(connection, 0);
      return false;
    }

    void write(ConnectionType &connection, int i) const
    {
      if (3 == i) return;
      connection.write(chunks[i], strlen(chunks[i]),
        [this, &connection, i](const system::error_code &, std::size_t)
        {
          ++called_;
          write(connection, i + 1);
        });
    }

    class FactoryType
    {
    public:
      FactoryType(const FactoryType &) = default;
      FactoryType(int &called):
        called_(called) {}
      int &called_;
      TestProcessor operator ()() const
      {
        return TestProcessor(called_);
      }
    };

  };

  // Start server and client in two different threads.
  int called = 0;
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
  etElAssert(3 == called, "Done function not called!");

  etElAssert(client.server_response() == server_response,
             "Unexpected response from server!");
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
