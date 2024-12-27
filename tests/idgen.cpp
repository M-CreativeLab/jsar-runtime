#define CATCH_CONFIG_MAIN
#include "./catch2/catch_amalgamated.hpp"
#include <idgen.hpp>

TEST_CASE("TrIdGeneratorBase generates IDs correctly for uint32_t", "[TrIdGeneratorBase]") {
  TrIdGeneratorBase<uint32_t> idGen(0, 100);

  REQUIRE(idGen.get() == 0);
  REQUIRE(idGen.get() == 1);
  REQUIRE(idGen.get() == 2);

  for (uint32_t i = 3; i < 100; ++i) {
    idGen.get();
  }

  REQUIRE(idGen.get() == 0); // Reset to initial ID after reaching max
}

TEST_CASE("TrIdGeneratorBase generates IDs correctly for uint64_t", "[TrIdGeneratorBase]") {
  TrIdGeneratorBase<uint64_t> idGen(0, 100);

  REQUIRE(idGen.get() == 0);
  REQUIRE(idGen.get() == 1);
  REQUIRE(idGen.get() == 2);

  for (uint64_t i = 3; i < 100; ++i) {
    idGen.get();
  }

  REQUIRE(idGen.get() == 0); // Reset to initial ID after reaching max
}

TEST_CASE("TrIdGeneratorBase generates IDs correctly for uint16_t", "[TrIdGeneratorBase]") {
  TrIdGeneratorBase<uint16_t> idGen(0, 100);

  REQUIRE(idGen.get() == 0);
  REQUIRE(idGen.get() == 1);
  REQUIRE(idGen.get() == 2);

  for (uint16_t i = 3; i < 100; ++i) {
    idGen.get();
  }

  REQUIRE(idGen.get() == 0); // Reset to initial ID after reaching max
}

TEST_CASE("TrIdGeneratorBase generates IDs correctly for uint8_t", "[TrIdGeneratorBase]") {
  TrIdGeneratorBase<uint8_t> idGen(0, 100);

  REQUIRE(idGen.get() == 0);
  REQUIRE(idGen.get() == 1);
  REQUIRE(idGen.get() == 2);

  for (uint8_t i = 3; i < 100; ++i) {
    idGen.get();
  }

  REQUIRE(idGen.get() == 0); // Reset to initial ID after reaching max
}

TEST_CASE("TrIdGeneratorBase handles edge cases correctly", "[TrIdGeneratorBase]") {
  TrIdGeneratorBase<uint32_t> idGen(0, 3);

  REQUIRE(idGen.get() == 0);
  REQUIRE(idGen.get() == 1);
  REQUIRE(idGen.get() == 2);
  REQUIRE(idGen.get() == 0); // Reset to initial ID after reaching max
  REQUIRE(idGen.get() == 1);
  REQUIRE(idGen.get() == 2);
  REQUIRE(idGen.get() == 0); // Reset to initial ID after reaching max
}
