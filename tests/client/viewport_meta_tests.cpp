#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/html/html_meta_element.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>

using namespace dom;

TEST_CASE("HTMLMetaElement viewport parsing - basic width and height", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  // Set viewport meta attributes
  meta->setName("viewport");
  meta->setContent("width=640,height=480");
  
  REQUIRE(meta->isViewportMeta() == true);
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->width.has_value() == true);
  REQUIRE(viewport->height.has_value() == true);
  REQUIRE(*viewport->width == 640.0f);
  REQUIRE(*viewport->height == 480.0f);
  REQUIRE(viewport->device_width == false);
  REQUIRE(viewport->device_height == false);
}

TEST_CASE("HTMLMetaElement viewport parsing - device-width and device-height", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("viewport");
  meta->setContent("width=device-width,height=device-height");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->device_width == true);
  REQUIRE(viewport->device_height == true);
  REQUIRE(viewport->width.has_value() == false);
  REQUIRE(viewport->height.has_value() == false);
}

TEST_CASE("HTMLMetaElement viewport parsing - scale values", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("viewport");
  meta->setContent("initial-scale=1.0,minimum-scale=0.5,maximum-scale=2.0");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->initial_scale.has_value() == true);
  REQUIRE(viewport->minimum_scale.has_value() == true);
  REQUIRE(viewport->maximum_scale.has_value() == true);
  REQUIRE(*viewport->initial_scale == 1.0f);
  REQUIRE(*viewport->minimum_scale == 0.5f);
  REQUIRE(*viewport->maximum_scale == 2.0f);
}

TEST_CASE("HTMLMetaElement viewport parsing - user-scalable", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  // Test user-scalable=no
  meta->setName("viewport");
  meta->setContent("user-scalable=no");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->user_scalable.has_value() == true);
  REQUIRE(*viewport->user_scalable == false);
  
  // Test user-scalable=yes
  meta->setContent("user-scalable=yes");
  viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->user_scalable.has_value() == true);
  REQUIRE(*viewport->user_scalable == true);
}

TEST_CASE("HTMLMetaElement viewport parsing - complex content", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("viewport");
  meta->setContent("width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->device_width == true);
  REQUIRE(viewport->initial_scale.has_value() == true);
  REQUIRE(*viewport->initial_scale == 1.0f);
  REQUIRE(viewport->maximum_scale.has_value() == true);
  REQUIRE(*viewport->maximum_scale == 1.0f);
  REQUIRE(viewport->user_scalable.has_value() == true);
  REQUIRE(*viewport->user_scalable == false);
}

TEST_CASE("HTMLMetaElement viewport parsing - invalid values", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("viewport");
  meta->setContent("width=invalid,height=abc,initial-scale=99");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  // Invalid width and height should not be set
  REQUIRE(viewport->width.has_value() == false);
  REQUIRE(viewport->height.has_value() == false);
  // Invalid scale (outside 0.1-10.0 range) should not be set
  REQUIRE(viewport->initial_scale.has_value() == false);
}

TEST_CASE("HTMLMetaElement viewport parsing - whitespace handling", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("viewport");
  meta->setContent("  width = 320 , height = 568  ");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == true);
  REQUIRE(viewport->width.has_value() == true);
  REQUIRE(viewport->height.has_value() == true);
  REQUIRE(*viewport->width == 320.0f);
  REQUIRE(*viewport->height == 568.0f);
}

TEST_CASE("HTMLMetaElement viewport parsing - non-viewport meta", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("description");
  meta->setContent("width=640,height=480");
  
  REQUIRE(meta->isViewportMeta() == false);
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == false);
}

TEST_CASE("HTMLMetaElement viewport parsing - empty content", "[HTMLMetaElement][viewport]")
{
  auto context = std::make_shared<BrowsingContext>();
  auto doc = std::make_shared<Document>("text/html", DocumentType::kHTML, context);
  auto meta = std::make_shared<HTMLMetaElement>(doc);
  
  meta->setName("viewport");
  meta->setContent("");
  
  auto viewport = meta->parseViewportMeta();
  REQUIRE(viewport.has_value() == false);
}