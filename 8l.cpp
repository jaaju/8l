#include "core/server.h"

int main()
{
  etEl::TServer< > s("localhost", "8080");
  s.run();
}
