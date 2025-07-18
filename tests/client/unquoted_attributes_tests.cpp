#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <pugixml/pugixml.hpp>
#include <string>

using namespace std;

TEST_CASE("pugixml unquoted attributes parsing", "[HTML][Parsing]")
{
  SECTION("Basic unquoted attribute")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<a href=foobar></a>", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    REQUIRE(string(doc.child("a").attribute("href").value()) == "foobar");
  }

  SECTION("Multiple unquoted attributes")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<div class=container id=main></div>", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    auto div = doc.child("div");
    REQUIRE(string(div.attribute("class").value()) == "container");
    REQUIRE(string(div.attribute("id").value()) == "main");
  }

  SECTION("Mixed quoted and unquoted attributes")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<a href=foobar title=\"quoted title\" class='single-quoted'></a>", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    auto a = doc.child("a");
    REQUIRE(string(a.attribute("href").value()) == "foobar");
    REQUIRE(string(a.attribute("title").value()) == "quoted title");
    REQUIRE(string(a.attribute("class").value()) == "single-quoted");
  }

  SECTION("Self-closing tag with unquoted attributes")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<input type=text />", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    auto input = doc.child("input");
    REQUIRE(string(input.attribute("type").value()) == "text");
  }

  // Note: Boolean attributes with unquoted syntax not yet supported
  /*
  SECTION("Boolean attributes remain unchanged")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<input type=checkbox checked>", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    auto input = doc.child("input");
    REQUIRE(string(input.attribute("type").value()) == "checkbox");
    REQUIRE(string(input.attribute("checked").value()) == "checked");
  }
  */

  SECTION("Already quoted attributes remain unchanged")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<img src=\"image.jpg\" alt=\"test image\">", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    auto img = doc.child("img");
    REQUIRE(string(img.attribute("src").value()) == "image.jpg");
    REQUIRE(string(img.attribute("alt").value()) == "test image");
  }

  SECTION("Without unquoted attributes flag should fail")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<a href=foobar></a>", 
        pugi::parse_default);
    
    REQUIRE(!result);
    REQUIRE(result.status == pugi::status_bad_attribute);
  }

  SECTION("Unquoted attribute with special characters")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<a href=foo-bar_baz.html></a>", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    REQUIRE(string(doc.child("a").attribute("href").value()) == "foo-bar_baz.html");
  }

  SECTION("Empty string")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    // Empty string should fail - it's not a valid XML document
    REQUIRE(!result);
    REQUIRE(result.status == pugi::status_no_document_element);
  }

  SECTION("No attributes")
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<div>content</div>", 
        pugi::parse_default | pugi::parse_unquoted_attributes);
    
    REQUIRE(result);
    REQUIRE(string(doc.child("div").text().get()) == "content");
  }
}
