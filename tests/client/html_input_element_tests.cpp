#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/html/html_input_element.hpp>
#include <client/dom/document.hpp>

using namespace dom;

TEST_CASE("HTMLInputElement checkbox functionality", "[HTMLInputElement]")
{
  auto document = std::make_shared<Document>();
  HTMLInputElement checkbox(document);
  
  // Set type to checkbox
  checkbox.setType("checkbox");
  REQUIRE(checkbox.type() == "checkbox");
  
  // Test initial unchecked state
  REQUIRE(checkbox.checked() == false);
  REQUIRE(checkbox.indeterminate() == false);
  
  // Test setting checked state
  checkbox.setChecked(true);
  REQUIRE(checkbox.checked() == true);
  
  // Test indeterminate state
  checkbox.setIndeterminate(true);
  REQUIRE(checkbox.indeterminate() == true);
  
  // Test that click toggles checked state when not disabled
  checkbox.setChecked(false);
  checkbox.handleClick();
  REQUIRE(checkbox.checked() == true);
  
  // Test that click clears indeterminate state
  checkbox.setIndeterminate(true);
  checkbox.handleClick();
  REQUIRE(checkbox.indeterminate() == false);
  
  // Test disabled state prevents interaction
  checkbox.setDisabled(true);
  checkbox.setChecked(false);
  checkbox.handleClick();
  REQUIRE(checkbox.checked() == false); // Should not change when disabled
}

TEST_CASE("HTMLInputElement radio functionality", "[HTMLInputElement]")
{
  auto document = std::make_shared<Document>();
  HTMLInputElement radio(document);
  
  // Set type to radio
  radio.setType("radio");
  REQUIRE(radio.type() == "radio");
  
  // Test initial unchecked state
  REQUIRE(radio.checked() == false);
  
  // Test that click sets checked state for radio
  radio.handleClick();
  REQUIRE(radio.checked() == true);
  
  // Test that clicking checked radio doesn't uncheck it
  radio.handleClick();
  REQUIRE(radio.checked() == true); // Radio should remain checked
  
  // Test disabled state prevents interaction
  radio.setDisabled(true);
  radio.setChecked(false);
  radio.handleClick();
  REQUIRE(radio.checked() == false); // Should not change when disabled
}

TEST_CASE("HTMLInputElement validation with checkbox/radio", "[HTMLInputElement]")
{
  auto document = std::make_shared<Document>();
  HTMLInputElement checkbox(document);
  
  checkbox.setType("checkbox");
  checkbox.setRequired(true);
  
  // Required checkbox should be invalid when unchecked
  REQUIRE(checkbox.checkValidity() == false);
  
  // Required checkbox should be valid when checked
  checkbox.setChecked(true);
  REQUIRE(checkbox.checkValidity() == true);
}