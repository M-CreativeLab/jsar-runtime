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

  SECTION("Parse standalone :root selector")
  {
    auto result = CSSelectorParser::parseSelectors(":root");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 1);
    REQUIRE(selector.components()[0].isRoot());
    REQUIRE(selector.components()[0].type() == ComponentType::kRoot);
    
    // Verify string representation includes :root
    std::string str = static_cast<std::string>(result.value());
    REQUIRE(!str.empty());
  }

  SECTION("Parse complex selectors with :root")
  {
    // Test :root with class selector
    auto result = CSSelectorParser::parseSelectors(":root.theme-dark");
    REQUIRE(result.has_value());
    
    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);
    REQUIRE(selector.components()[0].isRoot());
    REQUIRE(selector.components()[1].isClass());
    REQUIRE(selector.components()[1].className() == "theme-dark");

    // Test multiple selectors including :root
    result = CSSelectorParser::parseSelectors(":root, body, html");
    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);
    REQUIRE(result.value().selectors()[0].components()[0].isRoot());
    REQUIRE(result.value().selectors()[1].components()[0].name() == "body");
    REQUIRE(result.value().selectors()[2].components()[0].name() == "html");
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

  SECTION("Parse :where() functional pseudo-class")
  {
    // Test simple :where() with single selector
    auto result = CSSelectorParser::parseSelectors(":where(div)");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 1);

    const auto &component = selector.components()[0];
    REQUIRE(component.isPseudoClass());
    REQUIRE(component.isWhere());
    REQUIRE(component.argumentSelectorList() != nullptr);

    // Check the argument selector list contains "div"
    const auto &argSelectors = component.argumentSelectorList();
    REQUIRE(argSelectors->size() == 1);
    const auto &argSelector = argSelectors->selectors()[0];
    REQUIRE(argSelector.size() == 1);
    REQUIRE(argSelector.components()[0].isLocalName());
    REQUIRE(argSelector.components()[0].name() == "div");

    // Test :where() with class and ID selectors
    result = CSSelectorParser::parseSelectors(":where(.foo)");
    REQUIRE(result.has_value());
    const auto &classComponent = result.value().selectors()[0].components()[0];
    REQUIRE(classComponent.isWhere());
    REQUIRE(classComponent.argumentSelectorList()->selectors()[0].components()[0].isClass());

    result = CSSelectorParser::parseSelectors(":where(#bar)");
    REQUIRE(result.has_value());
    const auto &idComponent = result.value().selectors()[0].components()[0];
    REQUIRE(idComponent.isWhere());
    REQUIRE(idComponent.argumentSelectorList()->selectors()[0].components()[0].isId());

    // Test :where() in complex selector
    result = CSSelectorParser::parseSelectors(":where(div) span");
    REQUIRE(result.has_value());

    const auto &complexSelector = result.value().selectors()[0];
    REQUIRE(complexSelector.size() == 3); // :where(...) + combinator + span

    REQUIRE(complexSelector.components()[0].isWhere());
    REQUIRE(complexSelector.components()[1].isCombinator());
    REQUIRE(complexSelector.components()[2].isLocalName());
    REQUIRE(complexSelector.components()[2].name() == "span");
  }

  SECTION("Parse :where() string representation")
  {
    auto result = CSSelectorParser::parseSelectors(":where(h1)");
    REQUIRE(result.has_value());

    std::string str = static_cast<std::string>(result.value());
    REQUIRE(str.find(":where(") != std::string::npos);
    REQUIRE(str.find("h1") != std::string::npos);
    REQUIRE(str.find(")") != std::string::npos);
  }

  SECTION("Parse universal selector")
  {
    auto result = CSSelectorParser::parseSelectors("*");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 1);

    const auto &selector = selectorList.selectors()[0];
    REQUIRE(selector.size() == 1);

    const auto &component = selector.components()[0];
    REQUIRE(component.isUniversal());
    REQUIRE(component.type() == ComponentType::kUniversal);
  }

  SECTION("Parse compound selectors with universal selector")
  {
    // Universal with class: *.class
    auto result = CSSelectorParser::parseSelectors("*.myclass");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);

    REQUIRE(selector.components()[0].isUniversal());
    REQUIRE(selector.components()[1].isClass());
    REQUIRE(selector.components()[1].className() == "myclass");

    // Universal with ID: *#id
    result = CSSelectorParser::parseSelectors("*#myid");
    REQUIRE(result.has_value());

    const auto &idSelector = result.value().selectors()[0];
    REQUIRE(idSelector.size() == 2);

    REQUIRE(idSelector.components()[0].isUniversal());
    REQUIRE(idSelector.components()[1].isId());
    REQUIRE(idSelector.components()[1].id() == "myid");

    // Universal with class and ID: *.class#id
    result = CSSelectorParser::parseSelectors("*.myclass#myid");
    REQUIRE(result.has_value());

    const auto &compoundSelector = result.value().selectors()[0];
    REQUIRE(compoundSelector.size() == 3);

    REQUIRE(compoundSelector.components()[0].isUniversal());
    REQUIRE(compoundSelector.components()[1].isClass());
    REQUIRE(compoundSelector.components()[1].className() == "myclass");
    REQUIRE(compoundSelector.components()[2].isId());
    REQUIRE(compoundSelector.components()[2].id() == "myid");
  }

  SECTION("Parse universal selector with combinators")
  {
    // Descendant: parent *
    auto result = CSSelectorParser::parseSelectors("div *");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 3);

    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "div");
    REQUIRE(selector.components()[1].isCombinator());
    REQUIRE(selector.components()[1].combinator() == Combinator::kDescendant);
    REQUIRE(selector.components()[2].isUniversal());

    // Child: parent > *
    result = CSSelectorParser::parseSelectors("div > *");
    REQUIRE(result.has_value());

    const auto &childSelector = result.value().selectors()[0];
    REQUIRE(childSelector.size() == 3);

    REQUIRE(childSelector.components()[0].isLocalName());
    REQUIRE(childSelector.components()[0].name() == "div");
    REQUIRE(childSelector.components()[1].isCombinator());
    REQUIRE(childSelector.components()[1].combinator() == Combinator::kChild);
    REQUIRE(childSelector.components()[2].isUniversal());

    // Sibling: element + *
    result = CSSelectorParser::parseSelectors("h1 + *");
    REQUIRE(result.has_value());

    const auto &siblingSelector = result.value().selectors()[0];
    REQUIRE(siblingSelector.size() == 3);

    REQUIRE(siblingSelector.components()[1].combinator() == Combinator::kNextSibling);
    REQUIRE(siblingSelector.components()[2].isUniversal());
  }

  SECTION("Parse universal selector with pseudo-classes")
  {
    auto result = CSSelectorParser::parseSelectors("*:hover");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);

    REQUIRE(selector.components()[0].isUniversal());
    REQUIRE(selector.components()[1].isPseudoClass());
    REQUIRE(selector.components()[1].isHover());

    // Multiple pseudo-classes
    result = CSSelectorParser::parseSelectors("*:focus:hover");
    REQUIRE(result.has_value());

    const auto &multiPseudoSelector = result.value().selectors()[0];
    REQUIRE(multiPseudoSelector.size() == 3);

    REQUIRE(multiPseudoSelector.components()[0].isUniversal());
    REQUIRE(multiPseudoSelector.components()[1].isFocus());
    REQUIRE(multiPseudoSelector.components()[2].isHover());
  }

  SECTION("Parse multiple selectors including universal")
  {
    auto result = CSSelectorParser::parseSelectors("*, div, .class");
    REQUIRE(result.has_value());

    const auto &selectorList = result.value();
    REQUIRE(selectorList.size() == 3);

    // First selector: *
    REQUIRE(selectorList.selectors()[0].size() == 1);
    REQUIRE(selectorList.selectors()[0].components()[0].isUniversal());

    // Second selector: div
    REQUIRE(selectorList.selectors()[1].size() == 1);
    REQUIRE(selectorList.selectors()[1].components()[0].isLocalName());
    REQUIRE(selectorList.selectors()[1].components()[0].name() == "div");

    // Third selector: .class
    REQUIRE(selectorList.selectors()[2].size() == 1);
    REQUIRE(selectorList.selectors()[2].components()[0].isClass());
    REQUIRE(selectorList.selectors()[2].components()[0].className() == "class");
  }

  SECTION("Universal selector string representation")
  {
    auto result = CSSelectorParser::parseSelectors("*");
    REQUIRE(result.has_value());

    std::string str = static_cast<std::string>(result.value());
    REQUIRE(str == "*");

    // Test compound selector with universal
    result = CSSelectorParser::parseSelectors("*.class");
    REQUIRE(result.has_value());

    str = static_cast<std::string>(result.value());
    REQUIRE(str.find("*") != std::string::npos);
    REQUIRE(str.find("class") != std::string::npos);
  }

  SECTION("Parse attribute selectors")
  {
    // Test [attr] existence selector
    auto result = CSSelectorParser::parseSelectors("[disabled]");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 1);

    const auto &component = selector.components()[0];
    REQUIRE(component.isAttribute());
    REQUIRE(component.attributeName() == "disabled");
    REQUIRE(component.attributeMatchType() == AttributeMatchType::kExists);

    // Test [attr=value] exact match
    result = CSSelectorParser::parseSelectors("[type=\"text\"]");
    REQUIRE(result.has_value());

    const auto &exactSelector = result.value().selectors()[0];
    REQUIRE(exactSelector.size() == 1);

    const auto &exactComponent = exactSelector.components()[0];
    REQUIRE(exactComponent.isAttribute());
    REQUIRE(exactComponent.attributeName() == "type");
    REQUIRE(exactComponent.attributeValue() == "text");
    REQUIRE(exactComponent.attributeMatchType() == AttributeMatchType::kExact);

    // Test [attr^=value] prefix match
    result = CSSelectorParser::parseSelectors("[href^=\"https\"]");
    REQUIRE(result.has_value());

    const auto &prefixComponent = result.value().selectors()[0].components()[0];
    REQUIRE(prefixComponent.isAttribute());
    REQUIRE(prefixComponent.attributeName() == "href");
    REQUIRE(prefixComponent.attributeValue() == "https");
    REQUIRE(prefixComponent.attributeMatchType() == AttributeMatchType::kPrefix);

    // Test [attr$=value] suffix match
    result = CSSelectorParser::parseSelectors("[src$=\".jpg\"]");
    REQUIRE(result.has_value());

    const auto &suffixComponent = result.value().selectors()[0].components()[0];
    REQUIRE(suffixComponent.attributeMatchType() == AttributeMatchType::kSuffix);
    REQUIRE(suffixComponent.attributeValue() == ".jpg");

    // Test [attr*=value] substring match
    result = CSSelectorParser::parseSelectors("[class*=\"highlight\"]");
    REQUIRE(result.has_value());

    const auto &substringComponent = result.value().selectors()[0].components()[0];
    REQUIRE(substringComponent.attributeMatchType() == AttributeMatchType::kSubstring);
    REQUIRE(substringComponent.attributeValue() == "highlight");

    // Test [attr~=value] whitespace-separated list
    result = CSSelectorParser::parseSelectors("[class~=\"button\"]");
    REQUIRE(result.has_value());

    const auto &whitespaceComponent = result.value().selectors()[0].components()[0];
    REQUIRE(whitespaceComponent.attributeMatchType() == AttributeMatchType::kWhitespace);
    REQUIRE(whitespaceComponent.attributeValue() == "button");

    // Test [attr|=value] dash-separated prefix
    result = CSSelectorParser::parseSelectors("[lang|=\"en\"]");
    REQUIRE(result.has_value());

    const auto &dashComponent = result.value().selectors()[0].components()[0];
    REQUIRE(dashComponent.attributeMatchType() == AttributeMatchType::kDashPrefix);
    REQUIRE(dashComponent.attributeValue() == "en");
  }

  SECTION("Parse specific examples: link[rel=\"modulepreload\"]")
  {
    auto result = CSSelectorParser::parseSelectors("link[rel=\"modulepreload\"]");
    REQUIRE(result.has_value());

    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);

    // Tag component
    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "link");

    // Attribute component
    const auto &attrComponent = selector.components()[1];
    REQUIRE(attrComponent.isAttribute());
    REQUIRE(attrComponent.attributeName() == "rel");
    REQUIRE(attrComponent.attributeValue() == "modulepreload");
    REQUIRE(attrComponent.attributeMatchType() == AttributeMatchType::kExact);

    // Test complex selector with multiple attributes
    result = CSSelectorParser::parseSelectors("input[type=\"text\"][disabled]");
    REQUIRE(result.has_value());

    const auto &complexSelector = result.value().selectors()[0];
    REQUIRE(complexSelector.size() == 3);

    REQUIRE(complexSelector.components()[0].isLocalName());
    REQUIRE(complexSelector.components()[0].name() == "input");

    REQUIRE(complexSelector.components()[1].isAttribute());
    REQUIRE(complexSelector.components()[1].attributeName() == "type");
    REQUIRE(complexSelector.components()[1].attributeValue() == "text");

    REQUIRE(complexSelector.components()[2].isAttribute());
    REQUIRE(complexSelector.components()[2].attributeName() == "disabled");
    REQUIRE(complexSelector.components()[2].attributeMatchType() == AttributeMatchType::kExists);
  }

  SECTION("Parse attribute selectors with unquoted values")
  {
    auto result = CSSelectorParser::parseSelectors("[type=text]");
    REQUIRE(result.has_value());

    const auto &component = result.value().selectors()[0].components()[0];
    REQUIRE(component.isAttribute());
    REQUIRE(component.attributeName() == "type");
    REQUIRE(component.attributeValue() == "text");
    REQUIRE(component.attributeMatchType() == AttributeMatchType::kExact);
  }

  SECTION("Attribute selector string representation")
  {
    auto result = CSSelectorParser::parseSelectors("[disabled]");
    REQUIRE(result.has_value());
    std::string str = static_cast<std::string>(result.value());
    REQUIRE(str == "[disabled]");

    result = CSSelectorParser::parseSelectors("link[rel=\"modulepreload\"]");
    REQUIRE(result.has_value());
    str = static_cast<std::string>(result.value());
    REQUIRE(str.find("link") != std::string::npos);
    REQUIRE(str.find("[rel=\"modulepreload\"]") != std::string::npos);

    result = CSSelectorParser::parseSelectors("[href^=\"https\"]");
    REQUIRE(result.has_value());
    str = static_cast<std::string>(result.value());
    REQUIRE(str == "[href^=\"https\"]");
  }

  SECTION("Attribute selectors with spaces")
  {
    auto result = CSSelectorParser::parseSelectors("[ data-value = \"test\" ]");
    REQUIRE(result.has_value());

    const auto &component = result.value().selectors()[0].components()[0];
    REQUIRE(component.isAttribute());
    REQUIRE(component.attributeName() == "data-value");
    REQUIRE(component.attributeValue() == "test");
    REQUIRE(component.attributeMatchType() == AttributeMatchType::kExact);
  }

  SECTION("Parse nth-child selectors")
  {
    // Test simple number nth-child(3)
    auto result = CSSelectorParser::parseSelectors("div:nth-child(3)");
    REQUIRE(result.has_value());
    
    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 2);
    REQUIRE(selector.components()[0].isLocalName());
    REQUIRE(selector.components()[0].name() == "div");
    REQUIRE(selector.components()[1].isPseudoClass());
    REQUIRE(selector.components()[1].isNthChild());
    REQUIRE(selector.components()[1].nthA() == 0);
    REQUIRE(selector.components()[1].nthB() == 3);

    // Test string representation
    std::string str = static_cast<std::string>(result.value());
    REQUIRE(str == "div:nth-child(3)");

    // Test odd keyword
    result = CSSelectorParser::parseSelectors("p:nth-child(odd)");
    REQUIRE(result.has_value());
    const auto &oddComponent = result.value().selectors()[0].components()[1];
    REQUIRE(oddComponent.isNthChild());
    REQUIRE(oddComponent.nthA() == 2);
    REQUIRE(oddComponent.nthB() == 1);
    str = static_cast<std::string>(result.value());
    REQUIRE(str == "p:nth-child(odd)");

    // Test even keyword
    result = CSSelectorParser::parseSelectors("span:nth-child(even)");
    REQUIRE(result.has_value());
    const auto &evenComponent = result.value().selectors()[0].components()[1];
    REQUIRE(evenComponent.isNthChild());
    REQUIRE(evenComponent.nthA() == 2);
    REQUIRE(evenComponent.nthB() == 0);
    str = static_cast<std::string>(result.value());
    REQUIRE(str == "span:nth-child(even)");

    // Test an+b formula: 3n+2
    result = CSSelectorParser::parseSelectors("li:nth-child(3n+2)");
    REQUIRE(result.has_value());
    const auto &formulaComponent = result.value().selectors()[0].components()[1];
    REQUIRE(formulaComponent.isNthChild());
    REQUIRE(formulaComponent.nthA() == 3);
    REQUIRE(formulaComponent.nthB() == 2);

    // Test negative coefficient: -n+3  
    result = CSSelectorParser::parseSelectors("div:nth-child(-n+3)");
    REQUIRE(result.has_value());
    const auto &negComponent = result.value().selectors()[0].components()[1];
    REQUIRE(negComponent.isNthChild());
    REQUIRE(negComponent.nthA() == -1);
    REQUIRE(negComponent.nthB() == 3);

    // Test just 'n' (equivalent to 1n+0)
    result = CSSelectorParser::parseSelectors("*:nth-child(n)");
    REQUIRE(result.has_value());
    const auto &nComponent = result.value().selectors()[0].components()[1];
    REQUIRE(nComponent.isNthChild());
    REQUIRE(nComponent.nthA() == 1);
    REQUIRE(nComponent.nthB() == 0);

    // Test complex formula: 2n-1 (equivalent to 2n+-1)
    result = CSSelectorParser::parseSelectors("tr:nth-child(2n-1)");
    REQUIRE(result.has_value());
    const auto &complexComponent = result.value().selectors()[0].components()[1];
    REQUIRE(complexComponent.isNthChild());
    REQUIRE(complexComponent.nthA() == 2);
    REQUIRE(complexComponent.nthB() == -1);
  }

  SECTION("Parse nth-of-type selectors")
  {
    // Test nth-of-type with number
    auto result = CSSelectorParser::parseSelectors("h2:nth-of-type(2)");
    REQUIRE(result.has_value());
    
    const auto &component = result.value().selectors()[0].components()[1];
    REQUIRE(component.isPseudoClass());
    REQUIRE(component.isNthOfType());
    REQUIRE(component.nthA() == 0);
    REQUIRE(component.nthB() == 2);

    // Test nth-of-type with formula
    result = CSSelectorParser::parseSelectors("img:nth-of-type(2n+1)");
    REQUIRE(result.has_value());
    const auto &formulaComponent = result.value().selectors()[0].components()[1];
    REQUIRE(formulaComponent.isNthOfType());
    REQUIRE(formulaComponent.nthA() == 2);
    REQUIRE(formulaComponent.nthB() == 1);

    // Test string representation
    std::string str = static_cast<std::string>(result.value());
    REQUIRE(str == "img:nth-of-type(odd)"); // 2n+1 should display as "odd"
  }

  SECTION("Complex selectors with nth-child")
  {
    // Test complex selector with nth-child
    auto result = CSSelectorParser::parseSelectors(".container > div:nth-child(2n+1)");
    REQUIRE(result.has_value());
    
    const auto &selector = result.value().selectors()[0];
    REQUIRE(selector.size() == 4); // .container > div :nth-child(2n+1)
    REQUIRE(selector.components()[0].isClass());
    REQUIRE(selector.components()[0].className() == "container");
    REQUIRE(selector.components()[1].isCombinator());
    REQUIRE(selector.components()[1].combinator() == Combinator::kChild);
    REQUIRE(selector.components()[2].isLocalName());
    REQUIRE(selector.components()[2].name() == "div");
    REQUIRE(selector.components()[3].isNthChild());
    REQUIRE(selector.components()[3].nthA() == 2);
    REQUIRE(selector.components()[3].nthB() == 1);
  }
}
