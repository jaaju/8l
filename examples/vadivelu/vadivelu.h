#pragma once

#include "core/defaults.h"

namespace etEl {
namespace examples {

// This processor simply echoes back whatever it hears, like Vadivelu.
// Until it hears "stop".
class Vadivelu
  : public etEl::defaults::ProcessorD< Vadivelu >
{
public:
  bool process(TConnection< Vadivelu > &, std::istream &, std::size_t sz);
};

inline bool Vadivelu::process(TConnection< Vadivelu > &connection,
  std::istream &in,
  std::size_t sz)
{
  in >> std::noskipws;
  std::shared_ptr< std::string > s(new std::string());
  s->reserve(sz);
  std::copy(std::istream_iterator< char >(in),
    std::istream_iterator< char >(),
    std::back_inserter(*s));

  // Include '\n' since server is line-based and calls processor
  // only when seeing a newline.
  bool stop = (*s == "stop\n");

  connection.write(s->c_str(), s->size(),
    [s, stop](const etEl::system::error_code &, std::size_t)
    {
      return !stop;
    });
  return !stop;
}

} // namespace examples
} // namespace etEl
