#include "core/server.h"
#include "examples/vadivelu/vadivelu.h"

int main()
{
  etEl::TServer< etEl::examples::Vadivelu > s("localhost", "8080");
  s.run();
}
