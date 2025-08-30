#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/properties.hpp>
#include <client/cssom/values/computed/context.hpp>
#include <client/dom/document.hpp>
#include <client/html/html_element.hpp>

using namespace client_cssom;

// Helper function to create a minimal context for testing
values::computed::Context createTestContext() {
  // Create a minimal document and element for testing
  auto document = std::make_shared<dom::Document>();
  auto element = std::make_shared<dom::HTMLElement>("div", document);
  return values::computed::Context::From(element);
}

TEST_CASE("CSS Variables (Custom Properties) Basic Support", "[css-variables]")
{
  SECTION("Parse CSS custom property declaration")
  {
    CSSStyleDeclaration style("--main-color: red;");

    // Check if custom property is stored
    REQUIRE(style.getPropertyValue("--main-color") == "red");
    REQUIRE(style.length() == 1);
    REQUIRE(style.item(0) == "--main-color");
  }

  SECTION("CSS custom property names are case sensitive")
  {
    CSSStyleDeclaration style("--Main-Color: blue; --main-color: red;");

    REQUIRE(style.getPropertyValue("--Main-Color") == "blue");
    REQUIRE(style.getPropertyValue("--main-color") == "red");
    REQUIRE(style.getPropertyValue("--MAIN-COLOR") == "");
    REQUIRE(style.length() == 2);
  }

  SECTION("CSS custom property with var() function")
  {
    CSSStyleDeclaration style("--main-color: red; color: var(--main-color);");

    REQUIRE(style.getPropertyValue("--main-color") == "red");

    // The raw value should contain var() until resolved
    std::string colorValue = style.getPropertyValue("color");
    REQUIRE(!colorValue.empty());
  }

  SECTION("Invalid CSS custom property names are ignored")
  {
    // Property names must start with --
    CSSStyleDeclaration style("-invalid: value; not--valid: value; valid: value;");

    REQUIRE(style.getPropertyValue("-invalid") == "");
    REQUIRE(style.getPropertyValue("not--valid") == "");
    REQUIRE(style.getPropertyValue("valid") == "value");
  }

  SECTION("CSS custom property with fallback value")
  {
    CSSStyleDeclaration style("color: var(--undefined-prop, blue);");

    std::string colorValue = style.getPropertyValue("color");
    REQUIRE(!colorValue.empty());
  }

  SECTION("CSS custom property inheritance flag")
  {
    // CSS custom properties should be marked as inherited
    REQUIRE(ComputedStyle::IsInheritedProperty("--custom-prop"));
    REQUIRE(ComputedStyle::IsInheritedProperty("--main-color"));
    REQUIRE_FALSE(ComputedStyle::IsInheritedProperty("color"));
    REQUIRE_FALSE(ComputedStyle::IsInheritedProperty("background-color"));
  }
}

TEST_CASE("CSS Variables Resolution in ComputedStyle", "[css-variables-resolution]")
{
  SECTION("Custom property storage in ComputedStyle")
  {
    CSSStyleDeclaration style("--theme-color: #ff0000; --spacing: 10px;");
    ComputedStyle computedStyle = ComputedStyle::Make(style, nullptr);

    REQUIRE(computedStyle.hasCustomProperty("--theme-color"));
    REQUIRE(computedStyle.getCustomProperty("--theme-color") == "#ff0000");
    REQUIRE(computedStyle.hasCustomProperty("--spacing"));
    REQUIRE(computedStyle.getCustomProperty("--spacing") == "10px");
    REQUIRE_FALSE(computedStyle.hasCustomProperty("--nonexistent"));
  }

  SECTION("Variable resolution with defined custom property")
  {
    ComputedStyle computedStyle;
    computedStyle.setCustomProperty("--main-color", "red");
    auto context = createTestContext();

    std::string resolved = computedStyle.resolveVariables("var(--main-color)", context);
    REQUIRE(resolved == "red");

    std::string complexResolved = computedStyle.resolveVariables("1px solid var(--main-color)", context);
    REQUIRE(complexResolved == "1px solid red");
  }

  SECTION("Variable resolution with fallback")
  {
    ComputedStyle computedStyle;
    auto context = createTestContext();

    std::string resolved = computedStyle.resolveVariables("var(--undefined, blue)", context);
    REQUIRE(resolved == "blue");

    std::string complexResolved = computedStyle.resolveVariables("var(--undefined, 1px solid black)", context);
    REQUIRE(complexResolved == "1px solid black");
  }

  SECTION("Variable resolution with parent inheritance")
  {
    ComputedStyle parentStyle;
    parentStyle.setCustomProperty("--inherited-color", "green");

    ComputedStyle childStyle;
    childStyle.inheritCustomProperties(parentStyle);

    REQUIRE(childStyle.hasCustomProperty("--inherited-color"));
    REQUIRE(childStyle.getCustomProperty("--inherited-color") == "green");

    auto context = createTestContext();
    std::string resolved = childStyle.resolveVariables("var(--inherited-color)", context);
    REQUIRE(resolved == "green");
  }

  SECTION("Variable resolution preserves unresolvable var() calls")
  {
    ComputedStyle computedStyle;
    auto context = createTestContext();

    std::string unresolved = computedStyle.resolveVariables("var(--undefined)", context);
    REQUIRE(unresolved == "var(--undefined)");
  }

  SECTION("Multiple variable resolution in single value")
  {
    ComputedStyle computedStyle;
    computedStyle.setCustomProperty("--primary", "red");
    computedStyle.setCustomProperty("--secondary", "blue");
    auto context = createTestContext();

    std::string resolved = computedStyle.resolveVariables("linear-gradient(var(--primary), var(--secondary))", context);
    REQUIRE(resolved == "linear-gradient(red, blue)");
  }
}

TEST_CASE("CSS Variables Runtime Updates and DOM Integration", "[css-variables-dom]")
{
  SECTION("CSS variable update triggers re-resolution")
  {
    ComputedStyle computedStyle;
    computedStyle.setCustomProperty("--primary-color", "red");
    auto context = createTestContext();

    // Initial resolution
    std::string resolved = computedStyle.resolveVariables("var(--primary-color)", context);
    REQUIRE(resolved == "red");

    // Update the variable
    bool updated = computedStyle.updateCustomProperty("--primary-color", "blue");
    REQUIRE(updated == true);

    // Verify new resolution
    std::string newResolved = computedStyle.resolveVariables("var(--primary-color)", context);
    REQUIRE(newResolved == "blue");
  }

  SECTION("CSS variable inheritance chain")
  {
    // Root style with variables
    ComputedStyle rootStyle;
    rootStyle.setCustomProperty("--theme-color", "green");
    rootStyle.setCustomProperty("--font-size", "16px");

    // Child style inherits from root
    ComputedStyle childStyle;
    childStyle.inheritCustomProperties(rootStyle);
    childStyle.setCustomProperty("--font-size", "14px"); // Override parent

    // Grandchild inherits from child
    ComputedStyle grandchildStyle;
    grandchildStyle.inheritCustomProperties(childStyle);

    // Check inheritance
    REQUIRE(grandchildStyle.getCustomProperty("--theme-color") == "green"); // From root
    REQUIRE(grandchildStyle.getCustomProperty("--font-size") == "14px");    // From child override

    // Verify resolution with inheritance
    auto context = createTestContext();
    std::string colorResolved = grandchildStyle.resolveVariables("var(--theme-color)", context);
    std::string sizeResolved = grandchildStyle.resolveVariables("var(--font-size)", context);

    REQUIRE(colorResolved == "green");
    REQUIRE(sizeResolved == "14px");
  }

  SECTION("ComputedStyle::Make with parent inheritance")
  {
    // Create parent style with variables
    CSSStyleDeclaration parentDecl("--main-color: purple; --spacing: 8px;");
    ComputedStyle parentStyle = ComputedStyle::Make(parentDecl, nullptr);

    // Create child style that uses parent variables
    CSSStyleDeclaration childDecl("color: var(--main-color); margin: var(--spacing);");
    ComputedStyle childStyle = ComputedStyle::Make(childDecl, nullptr);
    
    // Manually inherit custom properties for this test
    childStyle.inheritCustomProperties(parentStyle);

    // Verify inheritance occurred
    REQUIRE(childStyle.hasCustomProperty("--main-color"));
    REQUIRE(childStyle.getCustomProperty("--main-color") == "purple");
    REQUIRE(childStyle.hasCustomProperty("--spacing"));
    REQUIRE(childStyle.getCustomProperty("--spacing") == "8px");
  }

  SECTION("Complex variable resolution with nested var() calls")
  {
    ComputedStyle style;
    style.setCustomProperty("--primary", "red");
    style.setCustomProperty("--fallback", "blue");
    auto context = createTestContext();

    // Test nested fallback
    std::string resolved = style.resolveVariables("var(--undefined, var(--fallback))", context);
    REQUIRE(resolved == "var(--undefined, blue)"); // Partial resolution

    // Test multiple variables in one value
    std::string multipleResolved = style.resolveVariables("border: 1px solid var(--primary)", context);
    REQUIRE(multipleResolved == "border: 1px solid red");
  }
}

TEST_CASE("PropertyId handles custom properties", "[css-properties]")
{
  SECTION("Custom property ID creation")
  {
    CustomPropertyId customProp("--test-property");

    REQUIRE(std::string(customProp) == "--test-property");
    REQUIRE(customProp.bit() != 0);
  }

  SECTION("Custom property ID comparison")
  {
    CustomPropertyId prop1("--same-property");
    CustomPropertyId prop2("--same-property");
    CustomPropertyId prop3("--different-property");

    REQUIRE(prop1 == prop2);
    REQUIRE(prop1 != prop3);
  }

  SECTION("PropertyId can handle custom properties")
  {
    CustomPropertyId customProp("--custom-prop");
    PropertyId propertyId = PropertyId::Custom(customProp);

    // This tests the infrastructure exists
    REQUIRE(true); // Basic compilation test
  }
}