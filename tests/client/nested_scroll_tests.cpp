#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/scroll/scrollable_area.hpp>

using namespace client_scroll;

TEST_CASE("ScrollableArea canScrollInDirection with no overflow", "[ScrollableArea]")
{
  ScrollableArea area;
  
  // Test with no overflow rect (no scrolling possible)
  REQUIRE_FALSE(area.canScrollInDirection(10.0f, 0.0f));
  REQUIRE_FALSE(area.canScrollInDirection(0.0f, 10.0f));
  REQUIRE_FALSE(area.canScrollInDirection(-10.0f, 0.0f));
  REQUIRE_FALSE(area.canScrollInDirection(0.0f, -10.0f));
}

// Note: More comprehensive integration tests would require setting up
// a full DOM tree with nested scroll containers and layout calculations.
// The core logic for nested scroll container handling is in the
// DocumentEventDispatcher methods which depend on complex DOM and layout setup.
// The boundary checking in ScrollableArea is the foundation that enables
// the nested scroll behavior to work correctly.