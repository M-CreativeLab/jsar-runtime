#define CATCH_CONFIG_MAIN
#include "./catch2/catch_amalgamated.hpp"

int add(int a, int b)
{
  return a + b;
}

TEST_CASE("add", "[add]")
{
  REQUIRE(add(1, 2) == 3);
}
