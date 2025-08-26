#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/html/html_meta_element.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>
#include <client/browser/window.hpp>
#include <client/per_process.hpp>

using namespace dom;
using namespace browser;

TEST_CASE("Viewport meta integration - document parsing applies viewport", "[integration][viewport]")
{
  // Set up a mock client context
  auto client_context = TrClientContextPerProcess::Get();
  
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto window = std::make_shared<Window>(client_context);
  
  // Get initial window dimensions
  float initial_width = window->innerWidth();
  float initial_height = window->innerHeight();
  
  // Create a viewport meta element
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  meta->setName("viewport");
  meta->setContent("width=800,height=600");
  
  // Add the meta element to the document (this should trigger viewport application)
  doc->appendChild(meta);
  
  // Verify the viewport meta was parsed correctly
  REQUIRE(meta->isViewportMeta() == true);
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->width.has_value() == true);
  REQUIRE(viewport->height.has_value() == true);
  REQUIRE(*viewport->width == 800.0f);
  REQUIRE(*viewport->height == 600.0f);
}

TEST_CASE("Viewport meta integration - dynamic updates", "[integration][viewport]")
{
  auto client_context = TrClientContextPerProcess::Get();
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  
  // Create and add viewport meta element
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  meta->setName("viewport");
  meta->setContent("width=320,height=568");
  doc->appendChild(meta);
  
  // Change the content and verify it parses the new values
  meta->setContent("width=1024,height=768");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->width.has_value() == true);
  REQUIRE(viewport->height.has_value() == true);
  REQUIRE(*viewport->width == 1024.0f);
  REQUIRE(*viewport->height == 768.0f);
}

TEST_CASE("Viewport meta integration - multiple meta tags", "[integration][viewport]")
{
  auto client_context = TrClientContextPerProcess::Get();
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  
  // Add first viewport meta
  auto meta1 = std::make_shared<HTMLMetaElement>(doc);
  meta1->setName("viewport");
  meta1->setContent("width=640,height=480");
  doc->appendChild(meta1);
  
  // Add second viewport meta (typically the last one would take effect)
  auto meta2 = std::make_shared<HTMLMetaElement>(doc);
  meta2->setName("viewport");
  meta2->setContent("width=1920,height=1080");
  doc->appendChild(meta2);
  
  // Both should parse correctly
  auto viewport1 = meta1->parseViewportMeta();
  auto viewport2 = meta2->parseViewportMeta();
  
  REQUIRE(viewport1.has_value() == true);
  REQUIRE(viewport2.has_value() == true);
  REQUIRE(*viewport1->width == 640.0f);
  REQUIRE(*viewport2->width == 1920.0f);
}

TEST_CASE("Viewport meta integration - non-viewport meta ignored", "[integration][viewport]")
{
  auto client_context = TrClientContextPerProcess::Get();
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  
  // Add non-viewport meta element
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  meta->setName("description");
  meta->setContent("width=640,height=480");  // This should be ignored
  doc->appendChild(meta);
  
  REQUIRE(meta->isViewportMeta() == false);
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == false);
}