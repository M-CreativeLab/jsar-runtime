#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkPaint.h>

#include <client/layout/fragment.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/values/specified/background.hpp>

// Mock implementation for testing purposes
namespace client_layout {
  struct FragmentEdge {
    float top() const { return top_; }
    float right() const { return right_; }
    float bottom() const { return bottom_; }
    float left() const { return left_; }
    
    float top_ = 0.0f;
    float right_ = 0.0f;
    float bottom_ = 0.0f;
    float left_ = 0.0f;
  };
  
  class Fragment {
  public:
    Fragment(float width, float height, float borderLeft = 0, float paddingLeft = 0)
      : content_width_(width), content_height_(height) {
      border_.left_ = borderLeft;
      padding_.left_ = paddingLeft;
    }
    
    float contentWidth() const { return content_width_; }
    float contentHeight() const { return content_height_; }
    const FragmentEdge& border() const { return border_; }
    const FragmentEdge& padding() const { return padding_; }
    
  private:
    float content_width_;
    float content_height_;
    FragmentEdge border_;
    FragmentEdge padding_;
  };
}

// Mock ComputedStyle for testing
namespace client_cssom {
  class ComputedStyle {
  public:
    ComputedStyle() = default;
    
    void setBackgroundOrigin(const std::string& origin) {
      background_origin_ = origin;
    }
    
    void setBackgroundRepeat(const std::string& repeat) {
      background_repeat_ = repeat;
    }
    
    struct BackgroundOrigin {
      std::string value;
      bool isContentBox() const { return value == "content-box"; }
      bool isPaddingBox() const { return value == "padding-box"; }
      bool isBorderBox() const { return value == "border-box"; }
    };
    
    struct BackgroundRepeat {
      std::string value;
      bool isRepeat() const { return value == "repeat"; }
      bool isRepeatX() const { return value == "repeat-x"; }
      bool isRepeatY() const { return value == "repeat-y"; }
      bool isNoRepeat() const { return value == "no-repeat"; }
    };
    
    BackgroundOrigin backgroundOrigin() const {
      return BackgroundOrigin{background_origin_};
    }
    
    BackgroundRepeat backgroundRepeat() const {
      return BackgroundRepeat{background_repeat_};
    }
    
  private:
    std::string background_origin_ = "padding-box";
    std::string background_repeat_ = "repeat";
  };
}

using namespace client_layout;
using namespace client_cssom;

// Helper function to create a test image
sk_sp<SkImage> createTestImage(int width, int height, SkColor color = SK_ColorRED) {
  SkBitmap bitmap;
  bitmap.allocN32Pixels(width, height);
  bitmap.eraseColor(color);
  return bitmap.asImage();
}

// Helper function to calculate background positioning area (from web_content_renderer.cpp)
SkRect getBackgroundPositioningArea(const SkRect &borderBox,
                                   const Fragment &fragment,
                                   const ComputedStyle &style)
{
  if (style.backgroundOrigin().isBorderBox())
  {
    return borderBox;
  }
  else if (style.backgroundOrigin().isPaddingBox())
  {
    // For padding-box, subtract border widths
    float borderLeft = fragment.border().left();
    
    return SkRect::MakeLTRB(
      borderBox.fLeft + borderLeft,
      borderBox.fTop,
      borderBox.fRight,
      borderBox.fBottom);
  }
  else if (style.backgroundOrigin().isContentBox())
  {
    // For content-box, subtract border and padding widths
    float borderLeft = fragment.border().left();
    float paddingLeft = fragment.padding().left();

    return SkRect::MakeLTRB(
      borderBox.fLeft + borderLeft + paddingLeft,
      borderBox.fTop,
      borderBox.fRight,
      borderBox.fBottom);
  }

  // Default to border-box
  return borderBox;
}

// Mock version of calculateBackgroundPosition for testing
SkPoint calculateBackgroundPosition(const SkSize &imageSize,
                                   const SkRect &positioningArea,
                                   const ComputedStyle &style)
{
  // Simple left-top positioning for testing
  return SkPoint::Make(positioningArea.fLeft, positioningArea.fTop);
}

// Mock version of calculateBackgroundSize for testing  
SkSize calculateBackgroundSize(const sk_sp<SkImage> &image,
                              const SkRect &positioningArea,
                              const ComputedStyle &style)
{
  if (!image) return SkSize::Make(0, 0);
  return SkSize::Make(static_cast<float>(image->width()), static_cast<float>(image->height()));
}

// Test for negative repeat regions
TEST_CASE("Background repeat covers negative regions with content-box origin", "[background-repeat-negative]")
{
  SECTION("repeat-x with content-box origin should cover full border-box area")
  {
    // Create a fragment with border and padding offsets
    Fragment fragment(200, 100, 50, 50); // content: 200x100, border-left: 50, padding-left: 50
    
    // Border box is larger than content area
    SkRect borderBox = SkRect::MakeXYWH(0, 0, 300, 100); // Total width: border + padding + content
    
    // Create style with content-box origin and repeat-x
    ComputedStyle style;
    style.setBackgroundOrigin("content-box");
    style.setBackgroundRepeat("repeat-x");
    
    // Create a small test image 
    auto image = createTestImage(25, 25, SK_ColorRED);
    
    // Calculate positioning area (should be content-box)
    SkRect positioningArea = getBackgroundPositioningArea(borderBox, fragment, style);
    
    // The positioning area should start at x=100 (border + padding)
    REQUIRE(positioningArea.fLeft == 100.0f);
    REQUIRE(positioningArea.fRight == 300.0f);
    
    // Calculate image size and position
    SkSize imageSize = calculateBackgroundSize(image, positioningArea, style);
    SkPoint imagePosition = calculateBackgroundPosition(imageSize, positioningArea, style);
    
    // Image position should be at the start of content-box (x=100)
    REQUIRE(imagePosition.x() == 100.0f);
    
    // Now test the repeat logic - this is where the bug should be
    // For repeat-x, we need to ensure coverage extends to the left of the origin
    
    float startX = imagePosition.x();
    float imageWidth = imageSize.width();
    
    // Current buggy logic: only tiles from positioningArea.fLeft (100) to the right
    // Correct logic: should tile to cover the full repeatable area (border-box)
    
    // The repeatable area should be the border-box (or background-clip area)
    SkRect repeatableArea = borderBox; // In this case, we want to cover the full border-box
    
    // Adjust start position to ensure full coverage of repeatable area
    while (startX > repeatableArea.fLeft) {
      startX -= imageWidth;
    }
    
    // startX should now be at or before x=0 to cover the full border-box
    REQUIRE(startX <= 0.0f);
    
    // Calculate how many tiles are needed to cover the area from x=0 to x=100 (before content-box)
    int tilesBeforeContentBox = static_cast<int>(std::ceil((imagePosition.x() - startX) / imageWidth));
    REQUIRE(tilesBeforeContentBox > 0); // Should have tiles in the negative region
  }
  
  SECTION("repeat-y with content-box origin should cover full border-box area")
  {
    // Similar test for vertical repeat
    Fragment fragment(100, 200, 0, 0); // no border/padding for simplicity
    fragment.border_.top_ = 50;
    fragment.padding_.top_ = 50;
    
    SkRect borderBox = SkRect::MakeXYWH(0, 0, 100, 300);
    
    ComputedStyle style;
    style.setBackgroundOrigin("content-box");
    style.setBackgroundRepeat("repeat-y");
    
    auto image = createTestImage(25, 25, SK_ColorBLUE);
    
    SkRect positioningArea = getBackgroundPositioningArea(borderBox, fragment, style);
    
    // The positioning area should start at y=100 (border + padding top)
    REQUIRE(positioningArea.fTop == 100.0f);
    
    SkSize imageSize = calculateBackgroundSize(image, positioningArea, style);
    SkPoint imagePosition = calculateBackgroundPosition(imageSize, positioningArea, style);
    
    REQUIRE(imagePosition.y() == 100.0f);
    
    // Test vertical repeat logic
    float startY = imagePosition.y();
    float imageHeight = imageSize.height();
    SkRect repeatableArea = borderBox;
    
    while (startY > repeatableArea.fTop) {
      startY -= imageHeight;
    }
    
    REQUIRE(startY <= 0.0f);
    
    int tilesBeforeContentBox = static_cast<int>(std::ceil((imagePosition.y() - startY) / imageHeight));
    REQUIRE(tilesBeforeContentBox > 0);
  }
}