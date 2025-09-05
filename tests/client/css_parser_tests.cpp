#include "../catch2/catch_amalgamated.hpp"
#include <client/cssom/parsers/css_parser.hpp>

using namespace client_cssom::css_parser;

TEST_CASE("CSS Parser Tests", "[css-parser]")
{
  SECTION("Parse simple style rule")
  {
    CSSParser parser("body { color: red; font-size: 16px; }");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 1);
    
    auto style_rule = dynamic_cast<CSSStyleRule*>(rules[0].get());
    REQUIRE(style_rule != nullptr);
    REQUIRE(style_rule->getType() == CSSRuleType::kStyle);
    REQUIRE(style_rule->selector_text == "body");
    REQUIRE(style_rule->declarations.size() == 2);
    REQUIRE(style_rule->declarations["color"] == "red");
    REQUIRE(style_rule->declarations["font-size"] == "16px");
  }

  SECTION("Parse multiple style rules")
  {
    CSSParser parser(R"(
      h1 { color: blue; }
      .class { margin: 10px; }
      #id { padding: 5px; }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 3);
    
    auto h1_rule = dynamic_cast<CSSStyleRule*>(rules[0].get());
    REQUIRE(h1_rule->selector_text == "h1");
    REQUIRE(h1_rule->declarations["color"] == "blue");
    
    auto class_rule = dynamic_cast<CSSStyleRule*>(rules[1].get());
    REQUIRE(class_rule->selector_text == ".class");
    REQUIRE(class_rule->declarations["margin"] == "10px");
    
    auto id_rule = dynamic_cast<CSSStyleRule*>(rules[2].get());
    REQUIRE(id_rule->selector_text == "#id");
    REQUIRE(id_rule->declarations["padding"] == "5px");
  }

  SECTION("Parse complex selectors")
  {
    CSSParser parser(R"(
      div.container > p:first-child { color: red; }
      nav ul li a:hover { text-decoration: underline; }
      .parent .child + .sibling { margin-left: 20px; }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 3);
    
    auto rule1 = dynamic_cast<CSSStyleRule*>(rules[0].get());
    REQUIRE(rule1->selector_text == "div.container > p:first-child");
    
    auto rule2 = dynamic_cast<CSSStyleRule*>(rules[1].get());
    REQUIRE(rule2->selector_text == "nav ul li a:hover");
    
    auto rule3 = dynamic_cast<CSSStyleRule*>(rules[2].get());
    REQUIRE(rule3->selector_text == ".parent .child + .sibling");
  }

  SECTION("Parse @media rule")
  {
    CSSParser parser(R"(
      @media screen and (max-width: 768px) {
        body { font-size: 14px; }
        .container { width: 100%; }
      }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 1);
    
    auto media_rule = dynamic_cast<CSSMediaRule*>(rules[0].get());
    REQUIRE(media_rule != nullptr);
    REQUIRE(media_rule->getType() == CSSRuleType::kMedia);
    REQUIRE(media_rule->media_query == "screen and (max-width: 768px)");
    REQUIRE(media_rule->rules.size() == 2);
    
    auto body_rule = dynamic_cast<CSSStyleRule*>(media_rule->rules[0].get());
    REQUIRE(body_rule->selector_text == "body");
    REQUIRE(body_rule->declarations["font-size"] == "14px");
  }

  SECTION("Parse @keyframes rule")
  {
    CSSParser parser(R"(
      @keyframes slideIn {
        opacity: 0;
        transform: translateX(-100%);
      }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 1);
    
    auto keyframe_rule = dynamic_cast<CSSKeyframeRule*>(rules[0].get());
    REQUIRE(keyframe_rule != nullptr);
    REQUIRE(keyframe_rule->getType() == CSSRuleType::kKeyframe);
    REQUIRE(keyframe_rule->name == "slideIn");
    // For simplicity, our parser stores all declarations in one block
    REQUIRE(keyframe_rule->declarations.size() >= 1);
  }

  SECTION("Parse @font-face rule")
  {
    CSSParser parser(R"(
      @font-face {
        font-family: 'CustomFont';
        src: url('font.woff2') format('woff2');
        font-weight: normal;
      }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 1);
    
    auto font_face_rule = dynamic_cast<CSSFontFaceRule*>(rules[0].get());
    REQUIRE(font_face_rule != nullptr);
    REQUIRE(font_face_rule->getType() == CSSRuleType::kFontFace);
    REQUIRE(font_face_rule->declarations.size() >= 2);
    REQUIRE(font_face_rule->declarations["font-family"] == "'CustomFont'");
  }

  SECTION("Parse @import rule")
  {
    CSSParser parser("@import url('external.css') screen;");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 1);
    
    auto import_rule = dynamic_cast<CSSImportRule*>(rules[0].get());
    REQUIRE(import_rule != nullptr);
    REQUIRE(import_rule->getType() == CSSRuleType::kImport);
    REQUIRE(!import_rule->url.empty());
    REQUIRE(import_rule->media_query == "screen");
  }

  SECTION("Parse mixed stylesheet")
  {
    CSSParser parser(R"(
      @import url('base.css');
      
      body { 
        margin: 0; 
        font-family: Arial, sans-serif; 
      }
      
      @media print {
        body { color: black; }
      }
      
      @font-face {
        font-family: 'Icon';
        src: url('icon.woff');
      }
      
      .container { 
        max-width: 1200px; 
        margin: 0 auto; 
      }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 5);
    
    // Check rule types
    REQUIRE(dynamic_cast<CSSImportRule*>(rules[0].get()) != nullptr);
    REQUIRE(dynamic_cast<CSSStyleRule*>(rules[1].get()) != nullptr);
    REQUIRE(dynamic_cast<CSSMediaRule*>(rules[2].get()) != nullptr);
    REQUIRE(dynamic_cast<CSSFontFaceRule*>(rules[3].get()) != nullptr);
    REQUIRE(dynamic_cast<CSSStyleRule*>(rules[4].get()) != nullptr);
  }

  SECTION("Parse single style declaration")
  {
    CSSParser parser("");
    auto result = parser.parseStyleDeclaration("color: red; font-size: 16px; margin: 10px 20px;");

    REQUIRE(result.valid);
    REQUIRE(result.properties.size() == 3);
    REQUIRE(result.properties["color"] == "red");
    REQUIRE(result.properties["font-size"] == "16px");
    REQUIRE(result.properties["margin"] == "10px 20px");
  }

  SECTION("Parse style declaration with functions")
  {
    CSSParser parser("");
    auto result = parser.parseStyleDeclaration(
      "background: linear-gradient(to right, red, blue); "
      "transform: rotate(45deg) scale(1.2); "
      "box-shadow: 0 2px 4px rgba(0,0,0,0.1);"
    );

    REQUIRE(result.valid);
    REQUIRE(result.properties.size() == 3);
    REQUIRE(result.properties["background"] == "linear-gradient(to right, red, blue)");
    REQUIRE(result.properties["transform"] == "rotate(45deg) scale(1.2)");
    REQUIRE(result.properties["box-shadow"] == "0 2px 4px rgba(0,0,0,0.1)");
  }

  SECTION("Parse style declaration with important")
  {
    CSSParser parser("");
    auto result = parser.parseStyleDeclaration("color: red !important; font-size: 16px;");

    REQUIRE(result.valid);
    REQUIRE(result.properties.size() == 2);
    REQUIRE(result.properties["color"] == "red !important");
    REQUIRE(result.properties["font-size"] == "16px");
  }

  SECTION("Handle malformed CSS gracefully")
  {
    CSSParser parser("body { color: ; font-size: 16px; invalid-property }");
    auto rules = parser.parseStylesheet();

    // Should still parse valid parts
    REQUIRE(rules.size() >= 1);
    auto style_rule = dynamic_cast<CSSStyleRule*>(rules[0].get());
    if (style_rule)
    {
      REQUIRE(style_rule->selector_text == "body");
      // Should have parsed the valid font-size declaration
      REQUIRE(style_rule->declarations.find("font-size") != style_rule->declarations.end());
    }
  }

  SECTION("Parse CSS with comments")
  {
    CSSParser parser(R"(
      /* This is a comment */
      body { 
        color: red; /* inline comment */
        font-size: 16px;
      }
      /* Another comment */
      .class { margin: 10px; }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 2);
    
    auto body_rule = dynamic_cast<CSSStyleRule*>(rules[0].get());
    REQUIRE(body_rule->selector_text == "body");
    REQUIRE(body_rule->declarations["color"] == "red");
    REQUIRE(body_rule->declarations["font-size"] == "16px");
  }

  SECTION("Parse empty and whitespace-only input")
  {
    CSSParser parser("");
    auto rules = parser.parseStylesheet();
    REQUIRE(rules.empty());

    CSSParser parser2("   \n\t  ");
    auto rules2 = parser2.parseStylesheet();
    REQUIRE(rules2.empty());
  }

  SECTION("Parse CSS with vendor prefixes")
  {
    CSSParser parser(R"(
      .element {
        -webkit-transform: rotate(45deg);
        -moz-transform: rotate(45deg);
        -ms-transform: rotate(45deg);
        transform: rotate(45deg);
      }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 1);
    
    auto style_rule = dynamic_cast<CSSStyleRule*>(rules[0].get());
    REQUIRE(style_rule->declarations.size() == 4);
    REQUIRE(style_rule->declarations["-webkit-transform"] == "rotate(45deg)");
    REQUIRE(style_rule->declarations["transform"] == "rotate(45deg)");
  }

  SECTION("Test toCSSText methods")
  {
    CSSParser parser("body { color: red; font-size: 16px; }");
    auto rules = parser.parseStylesheet();

    REQUIRE(rules.size() == 1);
    auto style_rule = dynamic_cast<CSSStyleRule*>(rules[0].get());
    
    string css_text = style_rule->toCSSText();
    REQUIRE(css_text.find("body") != string::npos);
    REQUIRE(css_text.find("color: red") != string::npos);
    REQUIRE(css_text.find("font-size: 16px") != string::npos);
  }

  SECTION("Parse attribute selectors")
  {
    CSSParser parser(R"(
      [data-type="button"] { cursor: pointer; }
      input[type="text"] { border: 1px solid gray; }
      a[href^="https"] { color: green; }
      img[alt*="icon"] { width: 16px; }
    )");
    auto rules = parser.parseStylesheet();

    REQUIRE(parser.isValid());
    REQUIRE(rules.size() == 4);
    
    auto rule1 = dynamic_cast<CSSStyleRule*>(rules[0].get());
    REQUIRE(rule1->selector_text == "[data-type=\"button\"]");
    
    auto rule2 = dynamic_cast<CSSStyleRule*>(rules[1].get());
    REQUIRE(rule2->selector_text == "input[type=\"text\"]");
    
    auto rule3 = dynamic_cast<CSSStyleRule*>(rules[2].get());
    REQUIRE(rule3->selector_text == "a[href^=\"https\"]");
    
    auto rule4 = dynamic_cast<CSSStyleRule*>(rules[3].get());
    REQUIRE(rule4->selector_text == "img[alt*=\"icon\"]");
  }
}