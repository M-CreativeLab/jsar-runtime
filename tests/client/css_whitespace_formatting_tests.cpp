#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/computed/text.hpp>

using namespace client_cssom::values;

// Test helper function - we replicate the formatText logic here for testing
std::string formatText(const std::string &text, const computed::WhiteSpace &whiteSpace)
{
  if (text.empty())
    return "";

  std::string formatted = text;

  // Handle space and tab collapsing
  if (whiteSpace.shouldCollapseSpaces())
  {
    // Replace sequences of spaces/tabs with single space
    std::string result;
    bool lastWasSpace = false;
    for (char c : formatted)
    {
      if (c == ' ' || c == '\t')
      {
        if (!lastWasSpace)
        {
          result += ' ';
          lastWasSpace = true;
        }
      }
      else
      {
        result += c;
        lastWasSpace = false;
      }
    }
    formatted = result;
  }

  // Handle newline collapsing
  if (whiteSpace.shouldCollapseNewlines())
  {
    // Replace newlines with spaces (which may then be collapsed)
    std::string result;
    for (char c : formatted)
    {
      if (c == '\n' || c == '\r')
      {
        result += ' ';
      }
      else
      {
        result += c;
      }
    }
    formatted = result;
  }

  // Handle leading/trailing whitespace for collapsing modes
  if (whiteSpace.shouldCollapseSpaces())
  {
    size_t start = formatted.find_first_not_of(" \t");
    if (start == std::string::npos)
    {
      return ""; // Entire string is whitespace
    }

    size_t end = formatted.find_last_not_of(" \t");
    formatted = formatted.substr(start, end - start + 1);
  }

  return formatted;
}

TEST_CASE("WhiteSpace text formatting behavior", "[css-white-space-formatting]")
{
  SECTION("normal: collapse spaces and newlines")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::Normal();
    
    REQUIRE(formatText("hello  world", whiteSpace) == "hello world");
    REQUIRE(formatText("hello\nworld", whiteSpace) == "hello world");
    REQUIRE(formatText("hello\tworld", whiteSpace) == "hello world");
    REQUIRE(formatText("  hello  world  ", whiteSpace) == "hello world");
    REQUIRE(formatText("hello   \n   world", whiteSpace) == "hello world");
    REQUIRE(formatText("   ", whiteSpace) == "");
  }

  SECTION("nowrap: collapse spaces and newlines, no wrapping")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::Nowrap();
    
    REQUIRE(formatText("hello  world", whiteSpace) == "hello world");
    REQUIRE(formatText("hello\nworld", whiteSpace) == "hello world");
    REQUIRE(formatText("  hello  world  ", whiteSpace) == "hello world");
  }

  SECTION("pre: preserve all whitespace")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::Pre();
    
    REQUIRE(formatText("hello  world", whiteSpace) == "hello  world");
    REQUIRE(formatText("hello\nworld", whiteSpace) == "hello\nworld");
    REQUIRE(formatText("hello\tworld", whiteSpace) == "hello\tworld");
    REQUIRE(formatText("  hello  world  ", whiteSpace) == "  hello  world  ");
  }

  SECTION("pre-line: preserve newlines, collapse spaces")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::PreLine();
    
    REQUIRE(formatText("hello  world", whiteSpace) == "hello world");
    REQUIRE(formatText("hello\nworld", whiteSpace) == "hello\nworld");
    REQUIRE(formatText("hello\tworld", whiteSpace) == "hello world");
    REQUIRE(formatText("  hello  world  ", whiteSpace) == "hello world");
    REQUIRE(formatText("hello  \n  world", whiteSpace) == "hello\nworld");
  }

  SECTION("pre-wrap: preserve spaces and newlines")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::PreWrap();
    
    REQUIRE(formatText("hello  world", whiteSpace) == "hello  world");
    REQUIRE(formatText("hello\nworld", whiteSpace) == "hello\nworld");
    REQUIRE(formatText("hello\tworld", whiteSpace) == "hello\tworld");
    REQUIRE(formatText("  hello  world  ", whiteSpace) == "  hello  world  ");
  }

  SECTION("break-spaces: preserve all, including breaking at spaces")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::BreakSpaces();
    
    REQUIRE(formatText("hello  world", whiteSpace) == "hello  world");
    REQUIRE(formatText("hello\nworld", whiteSpace) == "hello\nworld");
    REQUIRE(formatText("hello\tworld", whiteSpace) == "hello\tworld");
    REQUIRE(formatText("  hello  world  ", whiteSpace) == "  hello  world  ");
  }

  SECTION("Edge cases")
  {
    computed::WhiteSpace normal = computed::WhiteSpace::Normal();
    computed::WhiteSpace pre = computed::WhiteSpace::Pre();
    
    REQUIRE(formatText("", normal) == "");
    REQUIRE(formatText("", pre) == "");
    REQUIRE(formatText("single", normal) == "single");
    REQUIRE(formatText("single", pre) == "single");
  }
}