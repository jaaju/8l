#include "core/test.h"

int main(int argc, char *[])
{
  if (!etEl::test::runTests(1 < argc)) {
    return 1;
  }
  return 0;
}
