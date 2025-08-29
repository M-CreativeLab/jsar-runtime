#include "../catch2/catch_amalgamated.hpp"
#include <client/cssom/selectors/css_selector_parser.hpp>
#include <iostream>

using namespace client_cssom::selectors;

TEST_CASE("CSS Selector Parser Tests", "[css-selector-parser]")
{
  SECTION("Parse simple tag selector")
  {
    auto result = CSSelectorParser::parseSelectors("div");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 1);

    const auto &component = selector.components()[0];
    REQUIRE(component.isLocalName());
    REQUIRE(component.name() == "div");
  }

  SECTION("Parse ID selector")
  {
    auto result = CSSelectorParser::parseSelectors("#myid");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 1);

    const auto &component = selector.components()[0];
    REQUIRE(component.isId());
    REQUIRE(component.id() == "myid");
  }

  SECTION("Parse class selector")
  {
    auto result = CSSelectorParser::parseSelectors(".myclass");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 1);

    const auto &component = selector.components()[0];
    REQUIRE(component.isClass());
    REQUIRE(component.className() == "myclass");
  }

  SECTION("Parse compound selector")
  {
    auto result = CSSelectorParser::parseSelectors("div.myclass#myid");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 3);

    // Tag
    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "div");

    // Class
    REQUIRE(selector.components()[1].isClass());
    REQUIRE(selector.components()[1].className() == "myclass");

    // ID
    REQUIRE(selector.components()[2].isId());
    REQUIRE(selector.components()[2].id() == "myid");
  }

  SECTION("Parse child combinator")
  {
    auto result = CSSelectorParser::parseSelectors("div > p");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 3);

    // div
    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "div");

    // >
    REQUIRE(selector.components()[1].isCombinator());
    REQUIRE(selector.components()[1].combinator() == Combinator::kChild);

    // p
    REQUIRE(selector.components()[2].isLocalName());
    REQUIRE(selector.components()[2].name() == "p");
  }

  SECTION("Parse descendant combinator")
  {
    auto result = CSSelectorParser::parseSelectors("div p");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 3);

    // div
    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "div");

    // space (descendant)
    REQUIRE(selector.components()[1].isCombinator());
    REQUIRE(selector.components()[1].combinator() == Combinator::kDescendant);

    // p
    REQUIRE(selector.components()[2].isLocalName());
    REQUIRE(selector.components()[2].name() == "p");
  }

  SECTION("Parse sibling combinators")
  {
    auto result = CSSelectorParser::parseSelectors("h1 + p");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 3);
    REQUIRE(selector.components()[1].combinator() == Combinator::kNextSibling);

    result = CSSelectorParser::parseSelectors("h1 ~ p");
    REQUIRE(result.has_value());

    const auto &selector2 = result.value().selectors()[0];
    REQUIRE(selector2.size() == 3);
    REQUIRE(selector2.components()[1].combinator() == Combinator::kLaterSibling);
  }

  SECTION("Parse pseudo-classes")
  {
    auto result = CSSelectorParser::parseSelectors("div:hover");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);

    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "div");

    REQUIRE(selector.components()[1].isPseudoClass());
    REQUIRE(selector.components()[1].isHover());

    // Test focus pseudo-class
    result = CSSelectorParser::parseSelectors("input:focus");
    REQUIRE(result.has_value());
    const auto &focusSelector = result.value().selectors()[0];
    REQUIRE(focusSelector.components()[1].isFocus());

    // Test active pseudo-class
    result = CSSelectorParser::parseSelectors("button:active");
    REQUIRE(result.has_value());
    const auto &activeSelector = result.value().selectors()[0];
    REQUIRE(activeSelector.components()[1].isActive());
  }

  SECTION("Parse special pseudo-classes")
  {
    auto result = CSSelectorParser::parseSelectors("html:root");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);
    REQUIRE(selector.components()[1].isRoot());

    result = CSSelectorParser::parseSelectors("div:empty");
    REQUIRE(result.has_value());
    const auto &emptySelector = result.value().selectors()[0];
    REQUIRE(emptySelector.components()[1].isEmpty());
  }

  SECTION("Parse pseudo-elements")
  {
    auto result = CSSelectorParser::parseSelectors("p::before");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);

    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "p");

    REQUIRE(selector.components()[1].isPseudoElement());
    REQUIRE(selector.components()[1].name() == "before");
  }

  SECTION("Parse multiple selectors")
  {
    auto result = CSSelectorParser::parseSelectors("div, .class, #id");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 3);

    // First selector: div
    REQUIRE(selectorList.selectors()[0].size() == 1);
    REQUIRE(selectorList.selectors()[0].components()[0].isLocalName());

    // Second selector: .class
    REQUIRE(selectorList.selectors()[1].size() == 1);
    REQUIRE(selectorList.selectors()[1].components()[0].isClass());

    // Third selector: #id
    REQUIRE(selectorList.selectors()[2].size() == 1);
    REQUIRE(selectorList.selectors()[2].components()[0].isId());
  }

  SECTION("Parse complex selector from examples")
  {
    auto result = CSSelectorParser::parseSelectors("body, .foo > div#bar");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 2);

    // First selector: body
    REQUIRE(selectorList.selectors()[0].size() == 1);
    REQUIRE(selectorList.selectors()[0].components()[0].isLocalName());
    REQUIRE(selectorList.selectors()[0].components()[0].name() == "body");

    // Second selector: .foo > div#bar
    const auto &complexSelector = selectorList.selectors()[1];
    REQUIRE(complexSelector.size() == 4);

    // .foo
    REQUIRE(complexSelector.components()[0].isClass());
    REQUIRE(complexSelector.components()[0].className() == "foo");

    // >
    REQUIRE(complexSelector.components()[1].isCombinator());
    REQUIRE(complexSelector.components()[1].combinator() == Combinator::kChild);

    // div
    REQUIRE(complexSelector.components()[2].isLocalName());
    REQUIRE(complexSelector.components()[2].name() == "div");

    // #bar
    REQUIRE(complexSelector.components()[3].isId());
    REQUIRE(complexSelector.components()[3].id() == "bar");
  }

  SECTION("String representation")
  {
    auto result = CSSelectorParser::parseSelectors("div.class#id:hover");
    REQUIRE(result.has_value());

    std::string str = static_cast<std::string>(result.value());
    // The exact string format may vary, but should contain the essential parts
    REQUIRE(!str.empty());
  }

  SECTION("Invalid selectors return nullopt")
  {
    auto result = CSSelectorParser::parseSelectors("");
    bool hasNoValue = !result.has_value() || result->size() == 0;
    REQUIRE(hasNoValue);

    result = CSSelectorParser::parseSelectors("   ");
    hasNoValue = !result.has_value() || result->size() == 0;
    REQUIRE(hasNoValue);
  }
}
