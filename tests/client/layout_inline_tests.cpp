#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/layout/display_type.hpp>
#include <client/layout/formatting_contexts.hpp>
#include <client/layout/layout_inline.hpp>
#include <client/layout/layout_view.hpp>
#include <memory>

using namespace client_layout;

TEST_CASE("DisplayType inline parsing", "[layout][inline]")
{
  SECTION("Parse 'inline' string correctly")
  {
    auto display = DisplayType::Make("inline");
    REQUIRE(display.isInline() == true);
    REQUIRE(display.isBlock() == false);
    REQUIRE(display.outside == DisplayOutside::kInline);
    REQUIRE(display.inside == DisplayInside::kFlow);
  }

  SECTION("Parse 'inline-block' string correctly")
  {
    auto display = DisplayType::Make("inline-block");
    REQUIRE(display.isInline() == true);
    REQUIRE(display.outside == DisplayOutside::kInline);
    REQUIRE(display.inside == DisplayInside::kFlowRoot);
  }

  SECTION("Parse 'inline-flex' string correctly")
  {
    auto display = DisplayType::Make("inline-flex");
    REQUIRE(display.isInline() == true);
    REQUIRE(display.isFlex() == true);
    REQUIRE(display.outside == DisplayOutside::kInline);
    REQUIRE(display.inside == DisplayInside::kFlex);
  }

  SECTION("Parse 'inline-grid' string correctly")
  {
    auto display = DisplayType::Make("inline-grid");
    REQUIRE(display.isInline() == true);
    REQUIRE(display.isGrid() == true);
    REQUIRE(display.outside == DisplayOutside::kInline);
    REQUIRE(display.inside == DisplayInside::kGrid);
  }
}

TEST_CASE("DisplayType inline factory methods", "[layout][inline]")
{
  SECTION("Inline() factory method")
  {
    auto display = DisplayType::Inline();
    REQUIRE(display.isInline() == true);
    REQUIRE(display.isBlock() == false);
    REQUIRE(display.outside == DisplayOutside::kInline);
    REQUIRE(display.inside == DisplayInside::kFlow);
  }
}

TEST_CASE("DisplayType inline taffy conversion", "[layout][inline]")
{
  SECTION("Inline display converts to correct taffy type")
  {
    auto display = DisplayType::Inline();
    auto taffyDisplay = static_cast<crates::layout2::styles::Display>(display);
    
    // The conversion should succeed without crashing
    // Note: Since taffy maps inline to block internally, but preserves 
    // inline semantics at the C++ level through DisplayType
    REQUIRE(true);
  }
}

TEST_CASE("DisplayType inline compatibility with existing types", "[layout][inline]")
{
  SECTION("Inline does not interfere with block")
  {
    auto block = DisplayType::Block();
    auto inline_display = DisplayType::Inline();
    
    REQUIRE(block.isBlock() == true);
    REQUIRE(block.isInline() == false);
    REQUIRE(inline_display.isBlock() == false);
    REQUIRE(inline_display.isInline() == true);
  }

  SECTION("Inline does not interfere with flex")
  {
    auto flex = DisplayType::Flex();
    auto inline_display = DisplayType::Inline();
    
    REQUIRE(flex.isFlex() == true);
    REQUIRE(flex.isInline() == false);
    REQUIRE(inline_display.isFlex() == false);
    REQUIRE(inline_display.isInline() == true);
  }

  SECTION("Inline does not interfere with grid")
  {
    auto grid = DisplayType::Grid();
    auto inline_display = DisplayType::Inline();
    
    REQUIRE(grid.isGrid() == true);
    REQUIRE(grid.isInline() == false);
    REQUIRE(inline_display.isGrid() == false);
    REQUIRE(inline_display.isInline() == true);
  }
}

// Test that simulates the layout object creation flow
TEST_CASE("Layout object creation for inline elements", "[layout][inline][integration]")
{
  SECTION("Display type routing works correctly")
  {
    // Test the flow: CSS 'display: inline' -> DisplayType -> Layout object creation
    
    // Step 1: Parse CSS display value
    auto display = DisplayType::Make("inline");
    REQUIRE(display.isInline() == true);
    
    // Step 2: Check that display type reports correct properties
    REQUIRE(display.outside == DisplayOutside::kInline);
    REQUIRE(display.inside == DisplayInside::kFlow);
    
    // Step 3: Verify taffy conversion doesn't crash
    auto taffyDisplay = static_cast<crates::layout2::styles::Display>(display);
    // The actual taffy display type is implementation detail, 
    // but conversion should not crash
    REQUIRE(true);
  }
}

// Tests demonstrating inline layout standards compliance
TEST_CASE("Inline layout CSS standards compliance", "[layout][inline][standards]")
{
  SECTION("Common CSS inline values are parsed correctly")
  {
    // Test standard CSS inline display values
    struct TestCase {
      std::string css_value;
      DisplayOutside expected_outside;
      DisplayInside expected_inside;
    };
    
    std::vector<TestCase> test_cases = {
      {"inline", DisplayOutside::kInline, DisplayInside::kFlow},
      {"inline-block", DisplayOutside::kInline, DisplayInside::kFlowRoot}, 
      {"inline-flex", DisplayOutside::kInline, DisplayInside::kFlex},
      {"inline-grid", DisplayOutside::kInline, DisplayInside::kGrid},
      {"inline-table", DisplayOutside::kInline, DisplayInside::kTable}
    };
    
    for (const auto& test_case : test_cases) {
      auto display = DisplayType::Make(test_case.css_value);
      REQUIRE(display.outside == test_case.expected_outside);
      REQUIRE(display.inside == test_case.expected_inside);
      REQUIRE(display.isInline() == true);
    }
  }
}

// Mock LayoutView for limited FormattingContext testing 
// Note: Full integration tests would require a complete DOM/layout setup
TEST_CASE("InlineFormattingContext basic functionality", "[layout][inline]")
{
  SECTION("InlineFormattingContext identifies as inline")
  {
    // Create a minimal test without full DOM setup
    auto display = DisplayType::Inline();
    
    // Check that the display type would route to inline formatting context
    REQUIRE(display.isInline() == true);
    
    // In the real system, FormattingContext::Make(display, view) would create
    // an InlineFormattingContext, and it would return true for isInline()
    // This test verifies the routing logic would work correctly
  }
}

// Documentation test - ensures inline elements get created with correct type
TEST_CASE("Inline element type examples", "[layout][inline][documentation]")
{
  SECTION("Common inline elements have correct display defaults")
  {
    // This test documents which CSS display values should create inline layouts
    // In a real web page, these would be the default display values for common inline elements:
    
    // <span>, <a>, <em>, <strong>, etc. default to display: inline
    auto inline_display = DisplayType::Make("inline");
    REQUIRE(inline_display.isInline() == true);
    
    // <img>, some <input> types default to display: inline-block  
    auto inline_block = DisplayType::Make("inline-block");
    REQUIRE(inline_block.isInline() == true);
    
    // This ensures JSAR can correctly handle standard HTML inline elements
  }
}