#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <client/html/html_image_element.hpp>
#include <client/builtin_scene/image.hpp>
#include <client/builtin_scene/xr.hpp>

using namespace dom;
using namespace builtin_scene;

TEST_CASE("HTMLImageElement supports spatial attribute", "[spatial][image]")
{
  SECTION("spatial attribute can be set and retrieved")
  {
    auto document = std::make_shared<Document>();
    HTMLImageElement img(document);
    
    // Test default state
    REQUIRE(img.spatial().empty());
    REQUIRE_FALSE(img.isSpatial());
    
    // Test setting spatial attribute
    img.setAttribute("spatial", "stereo", false);
    REQUIRE(img.spatial() == "stereo");
    REQUIRE(img.isSpatial());
    
    // Test unsetting spatial attribute
    img.setAttribute("spatial", "", false);
    REQUIRE(img.spatial().empty());
    REQUIRE_FALSE(img.isSpatial());
  }
}

TEST_CASE("Image2d component supports spatial images", "[spatial][image]")
{
  SECTION("spatial image component can be created")
  {
    std::string src = "test.jpg";
    std::shared_ptr<SkBitmap> bitmap = nullptr;
    
    // Test non-spatial image
    Image2d normalImage(src, bitmap);
    REQUIRE_FALSE(normalImage.isSpatial());
    
    // Test spatial image
    Image2d spatialImage(src, bitmap, true);
    REQUIRE(spatialImage.isSpatial());
    
    // Test setting spatial flag
    normalImage.setSpatial(true);
    REQUIRE(normalImage.isSpatial());
  }
  
  SECTION("spatial image provides eye-specific content")
  {
    // Create a test bitmap (mock implementation)
    std::string src = "spatial_test.jpg";
    std::shared_ptr<SkBitmap> bitmap = std::make_shared<SkBitmap>();
    
    // Mock a 200x100 bitmap (left-right side-by-side)
    SkImageInfo info = SkImageInfo::MakeN32Premul(200, 100);
    bitmap->allocPixels(info);
    
    Image2d spatialImage(src, bitmap, true);
    
    // Get eye-specific images
    auto leftImage = spatialImage.getLeftEyeImage();
    auto rightImage = spatialImage.getRightEyeImage();
    
    REQUIRE(leftImage != nullptr);
    REQUIRE(rightImage != nullptr);
    
    // Left image should be half the width of the original
    REQUIRE(leftImage->width() == 100);
    REQUIRE(leftImage->height() == 100);
    
    // Right image should be half the width of the original
    REQUIRE(rightImage->width() == 100);
    REQUIRE(rightImage->height() == 100);
  }
}

TEST_CASE("XRRenderingContext tracks current view", "[spatial][xr]")
{
  SECTION("XRRenderingContext can track eye state")
  {
    XRRenderingContext context;
    
    // Test initial state
    REQUIRE(context.currentViewIndex() == -1);
    REQUIRE_FALSE(context.isLeftEye());
    REQUIRE_FALSE(context.isRightEye());
    REQUIRE_FALSE(context.isStereoMode());
    
    // Test left eye
    context.setCurrentViewIndex(0);
    REQUIRE(context.currentViewIndex() == 0);
    REQUIRE(context.isLeftEye());
    REQUIRE_FALSE(context.isRightEye());
    REQUIRE(context.isStereoMode());
    
    // Test right eye
    context.setCurrentViewIndex(1);
    REQUIRE(context.currentViewIndex() == 1);
    REQUIRE_FALSE(context.isLeftEye());
    REQUIRE(context.isRightEye());
    REQUIRE(context.isStereoMode());
  }
}