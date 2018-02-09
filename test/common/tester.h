#pragma once

#define _etEl_QUOTE_STRING(x) #x
#define etEl_QUOTE_STRING(x) _etEl_QUOTE_STRING(x)
#define etEl_LINE_NUMBER etEl_QUOTE_STRING(__LINE__)

#define etElAssert(condition, message)                            \
  if (!(condition)) {                                             \
    throw std::logic_error(message ", file: " __FILE__            \
                                   ", line: " etEl_LINE_NUMBER);  \
  }

#include <boost/core/demangle.hpp>

#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include <sstream>

namespace etEl {
namespace test {

// Simple Test Runner.
//
// @tparam TestTs Parameter pack of tests to run, deduced from @param 'tests'.
//  The pack must be given an even number of arguments. Each pair must be
//  a string literal that names a test and a callable that is the test.
// @param stopOnFailure If true, the function shall return false upon the first
//  test failure. More tests in 'tests' will not be run.
//  If false, all tests will be run regardless of any failure. The return will
//  be true if all tests pass and false otherwise.
// @param tests Named tests. See @tparam TestTs.
//
// Example.
// bool testFoo() { return true; }
// bool testBar() { return false; }
// bool testBaz() { return true; }
// //...
// test(true,
//      "Test Foo", testFoo,
//      "Test Bar", testBar,
//      "Test Baz", testBaz);
//
//  The above call must return false after executing 'testFoo()' and 'testBar()'.
//  If the first argument (param 'stopOnFailure') is false, then 'testBaz()' will
//  also be run.
//  The function will print the name of the failing test(s) to stderr.
template< typename... TestTs >
bool test(bool stopOnFailure, TestTs... tests);

namespace detail {

inline void report(std::ostream &err, const char *name, const std::exception &e)
{
  err << std::endl
      << "Test '" << name
      << "' failed, exception: " << boost::core::demangle(typeid(e).name())
      << ", message: " << e.what() << std::endl;
}

inline bool test(std::ostream &out,
                 std::ostream &,
                 int nTests,
                 int nFailed,
                 bool)
{
  out << std::endl
      << "Tests : " << nTests << std::endl
      << "Passed: " << (nTests - nFailed) << std::endl
      << "Failed: " << nFailed << std::endl;
  return (0 == nFailed);
}

template < typename TestT, typename... TestTs >
inline bool test(std::ostream &out,
                 std::ostream &err,
                 int nTests,
                 int nFailed,
                 bool stopOnFailure,
                 const char *name,
                 TestT theTest,
                 TestTs... tests)
{
  bool failed = false;
  try {
    out << '.' << std::flush;
    if (!theTest()) {
      report(err, name, std::logic_error(name));
      ++nFailed;
      failed = true;
    } else {
      out << "ok" << std::flush;
    }
  } catch (const std::exception &e) {
    report(err, name, e);
    ++nFailed;
    failed = true;
  } catch (...) {
    report(err, name, std::runtime_error("unknown exception"));
    ++nFailed;
    failed = true;
  }

  if (failed && stopOnFailure) {
    return test(out, err, ++nTests, nFailed, stopOnFailure);
  } else {
    return test(out, err, ++nTests, nFailed, stopOnFailure, tests...);
  }
}

template< typename... TestTs >
inline bool test(int nTests, int nFailed, bool stopOnFailure, TestTs... tests)
{
  std::streambuf *out = std::cout.rdbuf();
  std::streambuf *err = std::cerr.rdbuf();
  std::streambuf *log = std::clog.rdbuf();

  std::stringstream sout, serr, slog;
  std::cout.rdbuf(sout.rdbuf());
  std::cerr.rdbuf(serr.rdbuf());
  std::clog.rdbuf(slog.rdbuf());

  std::stringstream tout;
  tout.std::basic_ios< char >::rdbuf(out);
  std::stringstream terr;
  terr.std::basic_ios< char >::rdbuf(err);

  bool result = test(tout, terr, nTests, nFailed, stopOnFailure, tests...);

  std::cout.rdbuf(out);
  std::cerr.rdbuf(err);
  std::clog.rdbuf(log);

  std::cout << sout.str();
  std::cerr << serr.str();
  std::clog << slog.str();

  return result;
}
} // namespace detail

template< typename... TestTs >
inline bool test(bool stopOnFailure, TestTs... tests)
{
  return detail::test(0, 0, stopOnFailure, tests...);
}

} // namespace test
} // namespace etEl
