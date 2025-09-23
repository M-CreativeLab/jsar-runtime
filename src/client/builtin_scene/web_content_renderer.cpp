#include <optional>
#include <assert.h>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <future>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkColor.h>
#include <skia/include/core/SkPathEffect.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/effects/SkDashPathEffect.h>
#include <skia/include/effects/SkGradientShader.h>

#include <client/layout/fragment.hpp>
#include <client/canvas/image_codec.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/values/generics/border.hpp>
#include <client/cssom/values/generics/image.hpp>

#include "./hierarchy.hpp"
#include "./transform.hpp"
#include "./instanced_mesh-inl.hpp"
#include "./mesh_material.hpp"
#include "./meshes.hpp"
#include "./materials.hpp"
#include "./web_content.hpp"
#include "./image.hpp"
#include "./xr.hpp"

namespace builtin_scene::web_renderer
{
  using namespace std;
  using namespace skia::textlayout;
  using namespace client_cssom;
  using namespace client_cssom::values;

  using BorderEdge = client_cssom::values::generics::BorderEdge;
  using BorderCorner = client_cssom::values::generics::BorderCorner;


  // Helper function to calculate background positioning area based on background-origin
  SkRect getBackgroundPositioningArea(const SkRRect &roundedRect,
                                      const client_layout::Fragment &fragment,
                                      const ComputedStyle &style)
  {
    const SkRect &borderBox = roundedRect.rect();

    if (style.backgroundOrigin().isBorderBox())
    {
      return borderBox;
    }
    else if (style.backgroundOrigin().isPaddingBox())
    {
      // For padding-box, subtract border widths
      float borderTop = fragment.border().top();
      float borderRight = fragment.border().right();
      float borderBottom = fragment.border().bottom();
      float borderLeft = fragment.border().left();

      return SkRect::MakeLTRB(
        borderBox.fLeft + borderLeft,
        borderBox.fTop + borderTop,
        borderBox.fRight - borderRight,
        borderBox.fBottom - borderBottom);
    }
    else if (style.backgroundOrigin().isContentBox())
    {
      // For content-box, subtract border and padding widths
      float borderTop = fragment.border().top();
      float borderRight = fragment.border().right();
      float borderBottom = fragment.border().bottom();
      float borderLeft = fragment.border().left();

      float paddingTop = fragment.padding().top();
      float paddingRight = fragment.padding().right();
      float paddingBottom = fragment.padding().bottom();
      float paddingLeft = fragment.padding().left();

      return SkRect::MakeLTRB(
        borderBox.fLeft + borderLeft + paddingLeft,
        borderBox.fTop + borderTop + paddingTop,
        borderBox.fRight - borderRight - paddingRight,
        borderBox.fBottom - borderBottom - paddingBottom);
    }

    // Default to border-box
    return borderBox;
  }

  // Helper function to calculate background size based on background-size property
  SkSize calculateBackgroundSize(const sk_sp<SkImage> &image,
                                 const SkRect &positioningArea,
                                 const ComputedStyle &style)
  {
    if (!image)
      return SkSize::Make(0, 0);

    float intrinsicWidth = static_cast<float>(image->width());
    float intrinsicHeight = static_cast<float>(image->height());
    float areaWidth = positioningArea.width();
    float areaHeight = positioningArea.height();

    const auto &backgroundSize = style.backgroundSize();

    if (backgroundSize.isAuto())
    {
      // Use intrinsic dimensions
      return SkSize::Make(intrinsicWidth, intrinsicHeight);
    }
    else if (backgroundSize.isCover())
    {
      // Scale to cover entire area, maintaining aspect ratio
      float scaleX = areaWidth / intrinsicWidth;
      float scaleY = areaHeight / intrinsicHeight;
      float scale = std::max(scaleX, scaleY);
      return SkSize::Make(intrinsicWidth * scale, intrinsicHeight * scale);
    }
    else if (backgroundSize.isContain())
    {
      // Scale to fit within area, maintaining aspect ratio
      float scaleX = areaWidth / intrinsicWidth;
      float scaleY = areaHeight / intrinsicHeight;
      float scale = std::min(scaleX, scaleY);
      return SkSize::Make(intrinsicWidth * scale, intrinsicHeight * scale);
    }
    else if (backgroundSize.isLength())
    {
      // Use specified length, maintaining aspect ratio
      float length = backgroundSize.getWidth();
      float scale = length / intrinsicWidth;
      return SkSize::Make(length, intrinsicHeight * scale);
    }
    else if (backgroundSize.isPercentage())
    {
      // Use percentage of positioning area, maintaining aspect ratio
      float percentage = backgroundSize.getWidth() / 100.0f;
      float width = areaWidth * percentage;
      float scale = width / intrinsicWidth;
      return SkSize::Make(width, intrinsicHeight * scale);
    }
    else if (backgroundSize.isTwoValues())
    {
      // Use specified width and height
      return SkSize::Make(backgroundSize.getWidth(), backgroundSize.getHeight());
    }

    // Fallback to auto
    return SkSize::Make(intrinsicWidth, intrinsicHeight);
  }

  // Helper function to calculate background position based on background-position property
  SkPoint calculateBackgroundPosition(const SkSize &imageSize,
                                      const SkRect &positioningArea,
                                      const ComputedStyle &style)
  {
    const auto &backgroundPosition = style.backgroundPosition();
    float areaWidth = positioningArea.width();
    float areaHeight = positioningArea.height();

    if (backgroundPosition.isCenter())
    {
      // Center both horizontally and vertically
      float x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f;
      float y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f;
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isLeft())
    {
      // Left edge horizontally, center vertically
      float x = positioningArea.fLeft;
      float y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f;
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isRight())
    {
      // Right edge horizontally, center vertically
      // Ensure precise alignment to the right edge
      float x = positioningArea.fRight - imageSize.width();
      float y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f;
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isTop())
    {
      // Center horizontally, top edge vertically
      float x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f;
      float y = positioningArea.fTop;
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isBottom())
    {
      // Center horizontally, bottom edge vertically
      // Ensure precise alignment to the bottom edge
      float x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f;
      float y = positioningArea.fBottom - imageSize.height();
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isLength())
    {
      // Use specified length for x, center vertically
      float x = positioningArea.fLeft + backgroundPosition.getX();
      float y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f;
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isPercentage())
    {
      // Use percentage for x, center vertically
      float percentage = backgroundPosition.getX() / 100.0f;
      float x = positioningArea.fLeft + (areaWidth - imageSize.width()) * percentage;
      float y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f;
      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isTwoValues())
    {
      // Use specified x and y values
      // Handle potential percentage values that may be parsed as lengths
      float xValue = backgroundPosition.getX();
      float yValue = backgroundPosition.getY();

      // Detect common percentage values that might be incorrectly parsed as lengths:
      // Focus on the most problematic cases: 0%, 50%, 100% and quarter values
      const float epsilon = 0.001f;
      auto isNearValue = [epsilon](float val, float target)
      {
        return std::abs(val - target) < epsilon;
      };

      // Check for the most common percentage values that cause alignment issues
      auto looksLikePercentage = [&isNearValue](float value)
      {
        return (value >= 0.0f && value <= 100.0f) &&
               (isNearValue(value, 0.0f) || isNearValue(value, 25.0f) ||
                isNearValue(value, 50.0f) || isNearValue(value, 75.0f) ||
                isNearValue(value, 100.0f));
      };

      bool xLooksLikePercentage = looksLikePercentage(xValue);
      bool yLooksLikePercentage = looksLikePercentage(yValue);

      float x, y;

      if (xLooksLikePercentage)
      {
        // Treat as percentage for horizontal positioning
        float xPercentage = xValue / 100.0f;
        x = positioningArea.fLeft + xPercentage * (areaWidth - imageSize.width());
      }
      else
      {
        // Treat as length value
        x = positioningArea.fLeft + xValue;
      }

      if (yLooksLikePercentage)
      {
        // Treat as percentage for vertical positioning
        float yPercentage = yValue / 100.0f;
        y = positioningArea.fTop + yPercentage * (areaHeight - imageSize.height());
      }
      else
      {
        // Treat as length value
        y = positioningArea.fTop + yValue;
      }

      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isThreeValues())
    {
      // Handle 3-value syntax: keyword offset keyword
      float x, y;

      auto hKeyword = backgroundPosition.getHorizontalKeyword();
      auto vKeyword = backgroundPosition.getVerticalKeyword();
      float hOffset = backgroundPosition.getHorizontalOffset();
      float vOffset = backgroundPosition.getVerticalOffset();

      using Keyword = client_cssom::values::generics::BackgroundPositionKeyword;

      // Calculate horizontal position
      if (hKeyword == Keyword::kLeftKeyword)
        x = positioningArea.fLeft + hOffset;
      else if (hKeyword == Keyword::kRightKeyword)
        x = positioningArea.fRight - imageSize.width() - hOffset;
      else if (hKeyword == Keyword::kCenterKeyword)
        x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f + hOffset;
      else
        x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f; // Default to center

      // Calculate vertical position
      if (vKeyword == Keyword::kTopKeyword)
        y = positioningArea.fTop + vOffset;
      else if (vKeyword == Keyword::kBottomKeyword)
        y = positioningArea.fBottom - imageSize.height() - vOffset;
      else if (vKeyword == Keyword::kCenterKeyword)
        y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f + vOffset;
      else
        y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f; // Default to center

      return SkPoint::Make(x, y);
    }
    else if (backgroundPosition.isFourValues())
    {
      // Handle 4-value syntax: keyword offset keyword offset
      float x, y;

      auto hKeyword = backgroundPosition.getHorizontalKeyword();
      auto vKeyword = backgroundPosition.getVerticalKeyword();
      float hOffset = backgroundPosition.getHorizontalOffset();
      float vOffset = backgroundPosition.getVerticalOffset();

      using Keyword = client_cssom::values::generics::BackgroundPositionKeyword;

      // Calculate horizontal position
      if (hKeyword == Keyword::kLeftKeyword)
        x = positioningArea.fLeft + hOffset;
      else if (hKeyword == Keyword::kRightKeyword)
        x = positioningArea.fRight - imageSize.width() - hOffset;
      else if (hKeyword == Keyword::kCenterKeyword)
        x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f + hOffset;
      else
        x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f; // Default to center

      // Calculate vertical position
      if (vKeyword == Keyword::kTopKeyword)
        y = positioningArea.fTop + vOffset;
      else if (vKeyword == Keyword::kBottomKeyword)
        y = positioningArea.fBottom - imageSize.height() - vOffset;
      else if (vKeyword == Keyword::kCenterKeyword)
        y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f + vOffset;
      else
        y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f; // Default to center

      return SkPoint::Make(x, y);
    }

    // Fallback to center
    float x = positioningArea.fLeft + (areaWidth - imageSize.width()) / 2.0f;
    float y = positioningArea.fTop + (areaHeight - imageSize.height()) / 2.0f;
    return SkPoint::Make(x, y);
  }

  // Helper function to draw background image with repeat pattern

  void InitSystem::onExecute()
  {
    auto webContentCtx = getResource<WebContentContext>();
    auto meshes = getResource<Meshes>();
    auto materials = getResource<Materials>();
    assert(webContentCtx != nullptr &&
           meshes != nullptr &&
           materials != nullptr);

    // Create mesh and material for web content rendering
    auto material = Material::Make<materials::WebContentInstancedMaterial>();
    auto mesh = MeshBuilder::CreateInstancedMesh<meshes::Plane>("HTMLClassicMeshes", math::Dir3::Forward());
    mesh->enableDepthOnlyPass();

    // Spawn the instanced mesh entity which will be used for rendering all web content elements.
    webContentCtx->instancedMeshEntity_ = spawn(hierarchy::Root(true),
                                                Mesh3d(meshes->add(mesh), false),
                                                MeshMaterial3d(materials->add(material)),
                                                Transform::FromXYZ(0.0f, 0.0f, 0.0f));
  }

  void RenderContentBaseSystem::onExecute()
  {
    auto selectDirtyContent = [](const WebContent &content) -> bool
    {
      // Only select content that has a canvas and is dirty
      return content.canvas() != nullptr && content.isContentDirty();
    };
    auto list = queryEntitiesWithComponent<WebContent>(selectDirtyContent);
    if (list.size() == 0)
      return;

    vector<future<void>> futures;
    futures.reserve(list.size());

    // Lambda to render a single WebContent entity
    auto renderContent = [this](ecs::EntityId entity, WebContent &content)
    {
      if (render(entity, content))
        content.setSurfaceDirty(true);
    };

    // Schedule rendering tasks for each dirty WebContent entity
    for (auto &item : list)
    {
      ecs::EntityId entity = item.first;
      WebContent &content = *item.second;
      futures.emplace_back(async(launch::async, [&renderContent, entity, &content]()
                                 { renderContent(entity, content); }));
    }

    // Wait for all rendering tasks to complete
    for (auto &future : futures)
      future.wait();
  }

  // Create a gradient shader based on the computed image and rounded rectangle.
  sk_sp<SkShader> createGradientShader(const computed::Image &, const SkRRect &);

  // Helper method to convert LengthPercentage to position (0.0 to 1.0)
  float lengthPercentageToPosition(const computed::LengthPercentage &lengthPercentage, float totalLength)
  {
    if (lengthPercentage.isPercentage())
    {
      return lengthPercentage.getPercentage().value();
    }
    else if (lengthPercentage.isLength())
    {
      // Convert length to position relative to total length
      float lengthPx = lengthPercentage.getLength().px();
      return totalLength > 0 ? std::clamp(lengthPx / totalLength, 0.0f, 1.0f) : 0.0f;
    }
    else
    {
      return 0.0f; // Fallback for calc or other types
    }
  }

  // Extract colors and positions from gradient items
  void extractGradientStops(const vector<computed::GradientItem> &items,
                            float totalLength,
                            vector<SkColor4f> &colors,
                            vector<SkScalar> &positions)
  {
    colors.clear();
    positions.clear();

    float lastHintPosition = 0.0f;
    for (const auto &item : items)
    {
      if (item.type == computed::GradientItem::kSimpleColorStop)
      {
        const auto &colorStop = get<typename computed::GradientItem::SimpleColorStop>(item.value);
        colors.push_back(SkColor4f::FromColor(colorStop.color.resolveToAbsoluteColor()));

        // For simple color stops, distribute positions evenly if not already set
        if (positions.empty())
          positions.push_back(0.0f);
        else if (positions.size() == colors.size() - 1)
          positions.push_back(1.0f);
        else
          positions.push_back((float)positions.size() / (colors.size() - 1));
      }
      else if (item.type == computed::GradientItem::kComplexColorStop)
      {
        const auto &colorStop = get<typename computed::GradientItem::ComplexColorStop>(item.value);
        colors.push_back(SkColor4f::FromColor(colorStop.color.resolveToAbsoluteColor()));

        // Convert length_percentage to position (0.0 to 1.0)
        float position = lengthPercentageToPosition(colorStop.length_percentage, totalLength);
        positions.push_back(position);
      }
      else if (item.type == computed::GradientItem::kInterpolationHint)
      {
        const auto &hint = get<typename computed::GradientItem::InterpolationHint>(item.value);

        // InterpolationHint affects the transition between the previous and next color stops
        // Store the hint position to potentially adjust gradient transitions
        lastHintPosition = lengthPercentageToPosition(hint.length_percentage, totalLength);

        // Note: Skia doesn't directly support interpolation hints, but we store the position
        // for potential future enhancement of gradient interpolation
      }
    }

    // Handle fallback cases
    if (colors.empty())
    {
      colors.push_back(SkColor4f::FromColor(SK_ColorTRANSPARENT));
      colors.push_back(SkColor4f::FromColor(SK_ColorTRANSPARENT));
      positions.push_back(0.0f);
      positions.push_back(1.0f);
    }
    else if (colors.size() == 1)
    {
      colors.push_back(colors[0]);
      positions.push_back(0.0f);
      positions.push_back(1.0f);
    }
    else if (positions.size() != colors.size())
    {
      positions.clear();
      for (size_t i = 0; i < colors.size(); ++i)
      {
        positions.push_back(colors.size() == 1 ? 0.0f : (float)i / (colors.size() - 1));
      }
    }
  }

  // Create a linear gradient shader
  sk_sp<SkShader> createLinearGradientShader(const computed::Gradient::LinearGradient *linearGradient,
                                             const SkRRect &originalRRect,
                                             SkTileMode tileMode)
  {
    // Calculate gradient direction points based on LineDirection
    SkPoint pts[2];
    const SkRect &rect = originalRRect.rect();

    switch (linearGradient->direction)
    {
    case generics::LineDirection::kToRight:
      pts[0] = SkPoint::Make(rect.fLeft, rect.centerY());
      pts[1] = SkPoint::Make(rect.fRight, rect.centerY());
      break;
    case generics::LineDirection::kToLeft:
      pts[0] = SkPoint::Make(rect.fRight, rect.centerY());
      pts[1] = SkPoint::Make(rect.fLeft, rect.centerY());
      break;
    case generics::LineDirection::kToBottom:
      pts[0] = SkPoint::Make(rect.centerX(), rect.fTop);
      pts[1] = SkPoint::Make(rect.centerX(), rect.fBottom);
      break;
    case generics::LineDirection::kToTop:
      pts[0] = SkPoint::Make(rect.centerX(), rect.fBottom);
      pts[1] = SkPoint::Make(rect.centerX(), rect.fTop);
      break;
    case generics::LineDirection::kToBottomRight:
      pts[0] = SkPoint::Make(rect.fLeft, rect.fTop);
      pts[1] = SkPoint::Make(rect.fRight, rect.fBottom);
      break;
    case generics::LineDirection::kToBottomLeft:
      pts[0] = SkPoint::Make(rect.fRight, rect.fTop);
      pts[1] = SkPoint::Make(rect.fLeft, rect.fBottom);
      break;
    case generics::LineDirection::kToTopRight:
      pts[0] = SkPoint::Make(rect.fLeft, rect.fBottom);
      pts[1] = SkPoint::Make(rect.fRight, rect.fTop);
      break;
    case generics::LineDirection::kToTopLeft:
      pts[0] = SkPoint::Make(rect.fRight, rect.fBottom);
      pts[1] = SkPoint::Make(rect.fLeft, rect.fTop);
      break;
    default:
      // Default to left-to-right
      pts[0] = SkPoint::Make(rect.fLeft, rect.centerY());
      pts[1] = SkPoint::Make(rect.fRight, rect.centerY());
      break;
    }

    // Calculate the total length for position conversion
    float totalLength = sqrt(pow(pts[1].fX - pts[0].fX, 2) + pow(pts[1].fY - pts[0].fY, 2));

    // Extract colors and positions from gradient items
    vector<SkColor4f> colors;
    vector<SkScalar> positions;
    extractGradientStops(linearGradient->items, totalLength, colors, positions);

    return SkGradientShader::MakeLinear(pts,
                                        colors.data(),
                                        SkColorSpace::MakeSRGB(),
                                        positions.data(),
                                        colors.size(),
                                        tileMode);
  }

  // Create a radial gradient shader
  sk_sp<SkShader> createRadialGradientShader(const computed::Gradient::RadialGradient *radialGradient,
                                             const SkRRect &originalRRect,
                                             SkTileMode tileMode)
  {
    const SkRect &rect = originalRRect.rect();
    SkPoint center = SkPoint::Make(rect.centerX(), rect.centerY());

    // Calculate radius based on size and shape
    SkScalar radius;
    switch (radialGradient->size)
    {
    case generics::RadialGradientSize::kClosestSide:
      radius = min(rect.width(), rect.height()) / 2.0f;
      break;
    case generics::RadialGradientSize::kFarthestSide:
      radius = max(rect.width(), rect.height()) / 2.0f;
      break;
    case generics::RadialGradientSize::kClosestCorner:
      radius = sqrt(pow(min(rect.width(), rect.height()) / 2.0f, 2) * 2);
      break;
    case generics::RadialGradientSize::kFarthestCorner:
    default:
      radius = sqrt(pow(rect.width() / 2.0f, 2) + pow(rect.height() / 2.0f, 2));
      break;
    }

    // Extract colors and positions from gradient items
    vector<SkColor4f> colors;
    vector<SkScalar> positions;
    extractGradientStops(radialGradient->items, radius, colors, positions);

    return SkGradientShader::MakeRadial(center,
                                        radius,
                                        colors.data(),
                                        SkColorSpace::MakeSRGB(),
                                        positions.data(),
                                        colors.size(),
                                        tileMode);
  }

  // Create a gradient shader from computed image value
  sk_sp<SkShader> createGradientShader(const computed::Image &image, const SkRRect &originalRRect)
  {
    if (!image.isGradient())
      return nullptr;

    SkTileMode tileMode = image.isGradientRepeating() ? SkTileMode::kRepeat : SkTileMode::kClamp;

    // Handle linear gradient
    const auto *linearGradient = image.getLinearGradient();
    if (linearGradient)
    {
      return createLinearGradientShader(linearGradient, originalRRect, tileMode);
    }

    // Handle radial gradient
    const auto *radialGradient = image.getRadialGradient();
    if (radialGradient)
    {
      return createRadialGradientShader(radialGradient, originalRRect, tileMode);
    }

    // TODO: Implement conic gradient support
    return nullptr;
  }

  // Compute the radius for a specific corner of the rounded rectangle.
  optional<SkVector> computeRoundedRectRadius(const SkRect &rect,
                                              const client_cssom::ComputedStyle &style,
                                              const BorderCorner &corner)
  {
    string name = client_cssom::values::generics::to_string(corner);
    if (!style.hasProperty(name))
      return nullopt;

    const auto &cornerRadius = style.borderRadius()[corner].lengthPercentage();
    if (cornerRadius.isPercentage())
    {
      const auto &percentage = cornerRadius.toPercentage();
      return SkVector({percentage->computeWithBase(rect.width()),
                       percentage->computeWithBase(rect.height())});
    }
    else if (cornerRadius.isLength())
    {
      auto computedPixels = cornerRadius.getLength().px();
      return SkVector({computedPixels, computedPixels});
    }
    else
    {
      return nullopt;
    }
  }

  bool shouldDrawRoundedRect(SkRRect &roundedRect, SkRect &rect, const client_cssom::ComputedStyle &style)
  {
    // Set the radius for all four corners.
    auto borderTopLeftRadius = computeRoundedRectRadius(rect, style, BorderCorner::kTopLeft);
    auto borderTopRightRadius = computeRoundedRectRadius(rect, style, BorderCorner::kTopRight);
    auto borderBottomRightRadius = computeRoundedRectRadius(rect, style, BorderCorner::kBottomRight);
    auto borderBottomLeftRadius = computeRoundedRectRadius(rect, style, BorderCorner::kBottomLeft);

    // Fast check for all zero radii.
    if (borderTopLeftRadius == nullopt &&
        borderTopRightRadius == nullopt &&
        borderBottomRightRadius == nullopt &&
        borderBottomLeftRadius == nullopt)
    {
      roundedRect.setRect(rect);
      return false;
    }

    static SkVector defaultRadius = {0.0f, 0.0f};
    SkVector radii[4] = {
      borderTopLeftRadius.value_or(defaultRadius),
      borderTopRightRadius.value_or(defaultRadius),
      borderBottomRightRadius.value_or(defaultRadius),
      borderBottomLeftRadius.value_or(defaultRadius)};
    roundedRect.setRectRadii(rect, radii);

    // Check if the radii are all zero.
    for (int i = 0; i < 4; i++)
    {
      if (radii[i].x() != 0.0f || radii[i].y() != 0.0f)
        return true;
    }

    // All corners have zero radius, so no need to draw rounded rectangle.
    return false;
  }

  void setBorderPaintEffect(SkPaint &paint,
                            client_cssom::values::computed::BorderSideStyle borderStyle,
                            float strokeWidth)
  {
    if (borderStyle.isNoneOrHidden())
    {
      paint.setStrokeWidth(0);
      paint.setPathEffect(nullptr);
      return;
    }

    paint.setStrokeWidth(strokeWidth);
    if (borderStyle.isDashed())
    {
      const SkScalar intervals[] = {10, 5};
      paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0));
    }
    else if (borderStyle.isDotted())
    {
      const SkScalar intervals[] = {2, 5};
      paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0));
    }
    else if (borderStyle.isSolid())
    {
      paint.setPathEffect(nullptr);
    }
  }

  // Should draw the border edge, and return the computed border width.
  inline bool shouldDrawBorderEdge(const client_cssom::ComputedStyle &style,
                                   const BorderEdge edge,
                                   float &computedBorderWidth)
  {
    const auto &edgeWidth = style.borderWidth()[edge];
    const auto &edgeStyle = style.borderStyle()[edge];

    // Fast check for border style and width.
    if (edgeStyle.isNoneOrHidden() || edgeWidth.isZero())
      return false;

    computedBorderWidth = edgeWidth.value;
    return computedBorderWidth > 0.0f;
  }

  bool drawBorders(SkCanvas *canvas,
                   SkRRect &roundedRect,
                   const client_layout::Fragment &fragment,
                   const client_cssom::ComputedStyle &style)
  {
    bool hasBorders = false;
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);

    float computedBorderWidth = 0.0f;
    if (shouldDrawBorderEdge(style, BorderEdge::kTop, computedBorderWidth))
    {
      const auto &edgeColor = style.borderColor()[BorderEdge::kTop];
      float halfBorderWidth = computedBorderWidth / 2.0f;

      paint.setColor(edgeColor.resolveToAbsoluteColor());
      setBorderPaintEffect(paint, style.borderStyle()[BorderEdge::kTop], halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      // Draw the left top corner
      path.arcTo(SkRect::MakeXYWH(rect.fLeft + halfBorderWidth,
                                  rect.fTop + halfBorderWidth,
                                  roundedRect.radii(SkRRect::kUpperLeft_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kUpperLeft_Corner).y() * 2),
                 180.0f,
                 90.0f,
                 false);
      path.lineTo(rect.fRight - roundedRect.radii(SkRRect::kUpperRight_Corner).x() - halfBorderWidth,
                  rect.fTop + halfBorderWidth);
      path.arcTo(SkRect::MakeXYWH(rect.fRight - roundedRect.radii(SkRRect::kUpperRight_Corner).x() * 2 - halfBorderWidth,
                                  rect.fTop + halfBorderWidth,
                                  roundedRect.radii(SkRRect::kUpperRight_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kUpperRight_Corner).y() * 2),
                 270.0f,
                 90.0f,
                 false);
      canvas->drawPath(path, paint);
      hasBorders = true;
    }
    if (shouldDrawBorderEdge(style, BorderEdge::kRight, computedBorderWidth))
    {
      const auto &edgeColor = style.borderColor()[BorderEdge::kRight];
      float halfBorderWidth = computedBorderWidth / 2.0f;

      paint.setColor(edgeColor.resolveToAbsoluteColor());
      setBorderPaintEffect(paint, style.borderStyle()[BorderEdge::kRight], halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      path.moveTo(rect.fRight - halfBorderWidth,
                  rect.fTop + roundedRect.radii(SkRRect::kUpperRight_Corner).y() + halfBorderWidth);
      path.lineTo(rect.fRight - halfBorderWidth,
                  rect.fBottom - roundedRect.radii(SkRRect::kLowerRight_Corner).y() - halfBorderWidth);
      canvas->drawPath(path, paint);
      hasBorders = true;
    }
    if (shouldDrawBorderEdge(style, BorderEdge::kBottom, computedBorderWidth))
    {
      const auto &edgeColor = style.borderColor()[BorderEdge::kBottom];
      float halfBorderWidth = computedBorderWidth / 2.0f;

      paint.setColor(edgeColor.resolveToAbsoluteColor());
      setBorderPaintEffect(paint, style.borderStyle()[BorderEdge::kBottom], halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      path.arcTo(SkRect::MakeXYWH(rect.fRight - roundedRect.radii(SkRRect::kLowerRight_Corner).x() * 2 - halfBorderWidth,
                                  rect.fBottom - roundedRect.radii(SkRRect::kLowerRight_Corner).y() * 2 - halfBorderWidth,
                                  roundedRect.radii(SkRRect::kLowerRight_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kLowerRight_Corner).y() * 2),
                 0.0f,
                 90.0f,
                 false);
      path.lineTo(rect.fLeft + roundedRect.radii(SkRRect::kLowerLeft_Corner).x() + halfBorderWidth,
                  rect.fBottom - halfBorderWidth);
      path.arcTo(SkRect::MakeXYWH(rect.fLeft + halfBorderWidth,
                                  rect.fBottom - roundedRect.radii(SkRRect::kLowerLeft_Corner).y() * 2 - halfBorderWidth,
                                  roundedRect.radii(SkRRect::kLowerLeft_Corner).x() * 2,
                                  roundedRect.radii(SkRRect::kLowerLeft_Corner).y() * 2),
                 90.0f,
                 90.0f,
                 false);
      canvas->drawPath(path, paint);
      hasBorders = true;
    }
    if (shouldDrawBorderEdge(style, BorderEdge::kLeft, computedBorderWidth))
    {
      const auto &edgeColor = style.borderColor()[BorderEdge::kLeft];
      float halfBorderWidth = computedBorderWidth / 2.0f;

      paint.setColor(edgeColor.resolveToAbsoluteColor());
      setBorderPaintEffect(paint, style.borderStyle()[BorderEdge::kLeft], halfBorderWidth * 2);

      SkPath path;
      const SkRect &rect = roundedRect.rect();
      path.moveTo(rect.fLeft + halfBorderWidth,
                  rect.fBottom - roundedRect.radii(SkRRect::kLowerLeft_Corner).y() - halfBorderWidth);
      path.lineTo(rect.fLeft + halfBorderWidth,
                  rect.fTop + roundedRect.radii(SkRRect::kUpperLeft_Corner).y() + halfBorderWidth);
      canvas->drawPath(path, paint);
      hasBorders = true;
    }
    return hasBorders;
  }

  bool RenderBackgroundSystem::render(ecs::EntityId entity, WebContent &content)
  {
    const ComputedStyle &style = content.style();
    const auto &fragment = content.fragment();
    if (!fragment.has_value()) // No layout, no rendering.
      return false;

    auto canvas = content.canvas();
    canvas->clear(SK_ColorTRANSPARENT);

    float top = 0.0f;
    float left = 0.0f;
    float width = fragment->contentWidth();
    float height = fragment->contentHeight();

    SkRect rect = SkRect::MakeXYWH(left, top, width, height);
    SkRRect &roundedRect = content.rounded_rect_;
    bool drawRoundedRect = shouldDrawRoundedRect(roundedRect, rect, style);
    if (drawRoundedRect)
    {
      // TODO(yorkie): support radius for xy
      content.setBorderRadius(roundedRect.radii(SkRRect::kUpperLeft_Corner).x(),
                              roundedRect.radii(SkRRect::kUpperRight_Corner).x(),
                              roundedRect.radii(SkRRect::kLowerRight_Corner).x(),
                              roundedRect.radii(SkRRect::kLowerLeft_Corner).x());
    }
    else
    {
      content.resetBorderRadius();
    }

    ClippingArea clipInfo;
    if (style.backgroundClip().isText())
    {
      // Get the text content from the children of this entity.
      string textContent;
      auto childrenComponent = getComponentChecked<hierarchy::Children>(entity);
      for (const auto &childEntity : childrenComponent.children())
      {
        auto textComponent = getComponent<Text2d>(childEntity);
        if (textComponent != nullptr)
          textContent += textComponent->content;
      }

      auto textPath = createTextPath(textContent, content);
      if (textPath.has_value())
        clipInfo = ClippingArea(textPath.value());
      else
        clipInfo = ClippingArea(SkRRect::MakeEmpty()); // No text path, use empty clipping area.
    }
    else if (!style.backgroundClip().isBorderBox())
    {
      SkRRect clippingArea = getBackgroundClippingArea(roundedRect, fragment.value(), style);
      clipInfo = ClippingArea(clippingArea);
    }

    if (style.hasBackgroundColor())
    {
      auto color = style.backgroundColor().resolveToAbsoluteColor();
      content.setBackgroundColor(SkColor4f::FromColor(color));
    }

    bool textureRequired = false;
    drawBackground(canvas,
                   roundedRect,
                   clipInfo,
                   fragment.value(),
                   style,
                   textureRequired);
    content.setTextureUsing(textureRequired);
    return true;
  }

  SkRRect RenderBackgroundSystem::getBackgroundClippingArea(const SkRRect &roundedRect,
                                                            const client_layout::Fragment &fragment,
                                                            const ComputedStyle &style)
  {
    const SkRect &borderBox = roundedRect.rect();
    SkRRect clippingArea = roundedRect; // Start with border-box

    if (style.backgroundClip().isPaddingBox())
    {
      // For padding-box, subtract border widths
      float borderTop = fragment.border().top();
      float borderRight = fragment.border().right();
      float borderBottom = fragment.border().bottom();
      float borderLeft = fragment.border().left();

      SkRect paddingRect = SkRect::MakeLTRB(
        borderBox.fLeft + borderLeft,
        borderBox.fTop + borderTop,
        borderBox.fRight - borderRight,
        borderBox.fBottom - borderBottom);

      // Adjust radii for the padding box
      SkVector radii[4];
      for (int i = 0; i < 4; i++)
      {
        SkVector originalRadius = roundedRect.radii(static_cast<SkRRect::Corner>(i));
        // Reduce radii by border width (but don't go below 0)
        float borderReduction = (i == 0 || i == 3) ? borderLeft : borderRight; // Simplified
        radii[i] = SkVector{std::max(0.0f, originalRadius.x() - borderReduction),
                            std::max(0.0f, originalRadius.y() - borderReduction)};
      }
      clippingArea.setRectRadii(paddingRect, radii);
    }
    else if (style.backgroundClip().isContentBox())
    {
      // For content-box, subtract border and padding widths
      float borderTop = fragment.border().top();
      float borderRight = fragment.border().right();
      float borderBottom = fragment.border().bottom();
      float borderLeft = fragment.border().left();

      float paddingTop = fragment.padding().top();
      float paddingRight = fragment.padding().right();
      float paddingBottom = fragment.padding().bottom();
      float paddingLeft = fragment.padding().left();

      SkRect contentRect = SkRect::MakeLTRB(
        borderBox.fLeft + borderLeft + paddingLeft,
        borderBox.fTop + borderTop + paddingTop,
        borderBox.fRight - borderRight - paddingRight,
        borderBox.fBottom - borderBottom - paddingBottom);

      // Adjust radii for the content box
      SkVector radii[4];
      for (int i = 0; i < 4; i++)
      {
        SkVector originalRadius = roundedRect.radii(static_cast<SkRRect::Corner>(i));
        // Reduce radii by border and padding width (but don't go below 0)
        float totalReduction = (i == 0 || i == 3) ? (borderLeft + paddingLeft) : (borderRight + paddingRight); // Simplified
        radii[i] = SkVector{std::max(0.0f, originalRadius.x() - totalReduction),
                            std::max(0.0f, originalRadius.y() - totalReduction)};
      }
      clippingArea.setRectRadii(contentRect, radii);
    }

    // For border-box (default) and text, return the original rounded rect
    return clippingArea;
  }

  optional<SkPath> RenderBackgroundSystem::createTextPath(const std::string &textContent,
                                                          const WebContent &content)
  {
    if (textContent.empty())
      return nullopt;

    // Try to get the font and text style for path creation
    const auto &fragment = content.fragment();
    if (!fragment.has_value())
      return nullopt;

    SkPath textPath;
    auto clientContext = TrClientContextPerProcess::Get();

    // Create paragraph to get text layout
    sk_sp<FontCollection> fontCollection = clientContext->getFontCacheManager();
    auto paragraphStyle = content.paragraphStyle();
    auto paragraphBuilder = ParagraphBuilder::make(paragraphStyle, fontCollection);
    paragraphBuilder->pushStyle(paragraphStyle.getTextStyle());
    paragraphBuilder->addText(textContent.c_str(), textContent.size());
    paragraphBuilder->pop();

    float layoutWidth = round(content.fragment()->contentWidth()) + 1.0f;
    auto paragraph = paragraphBuilder->Build();
    paragraph->layout(layoutWidth);

    // Calculate the text offset within the fragment
    // Text should be positioned in the content area (inside border and padding)
    const auto &borderBox = content.fragment()->border();
    const auto &paddingBox = content.fragment()->padding();
    float offsetX = borderBox.left() + paddingBox.left();
    float offsetY = borderBox.top() + paddingBox.top();

    // Apply vertical alignment
    const ComputedStyle &style = content.style();
    if (style.hasProperty("vertical-align"))
    {
      const auto &verticalAlign = style.verticalAlign();
      const float contentHeight = content.fragment()->contentHeight();
      const float textHeight = paragraph->getHeight();
      const float lineHeight = content.paragraphStyle().getHeight();

      switch (verticalAlign.tag())
      {
      case client_cssom::values::computed::VerticalAlign::Tag::kBaseline:
        // Baseline is the default - no adjustment needed
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kTop:
        // Align to top of content area
        offsetY = borderBox.top() + paddingBox.top();
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kMiddle:
        // Align to middle of content area
        offsetY = borderBox.top() + paddingBox.top() + (contentHeight - textHeight) / 2.0f;
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kBottom:
        // Align to bottom of content area
        offsetY = borderBox.top() + paddingBox.top() + contentHeight - textHeight;
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kSuper:
        // Move up by approximately 0.5em (half the font size)
        offsetY -= content.paragraphStyle().getTextStyle().getFontSize() * 0.5f;
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kSub:
        // Move down by approximately 0.25em (quarter the font size)
        offsetY += content.paragraphStyle().getTextStyle().getFontSize() * 0.25f;
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kTextTop:
        // Align with the top of the font (ascent)
        // This is approximated by moving up from baseline
        offsetY -= content.paragraphStyle().getTextStyle().getFontSize() * 0.8f;
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kTextBottom:
        // Align with the bottom of the font (descent)
        // This is approximated by moving down from baseline
        offsetY += content.paragraphStyle().getTextStyle().getFontSize() * 0.2f;
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kLength:
        // Apply custom length offset
        offsetY -= verticalAlign.value(); // Negative because positive values move up
        break;
      case client_cssom::values::computed::VerticalAlign::Tag::kPercentage:
        // Apply percentage offset relative to line height
        offsetY -= verticalAlign.getOffset(lineHeight);
        break;
      }
    }

    // Use the paragraph visitor to extract glyph paths with proper font handling
    // This approach correctly handles mixed CJK/English text by using the fonts
    // that the paragraph system has already resolved for each glyph
    auto addPath = [&textPath, offsetX, offsetY](int lineNumber, const Paragraph::VisitorInfo *info)
    {
      if (info == nullptr)
      {
        // End of line marker, nothing to do
        return;
      }

      // Extract glyph paths from the properly resolved fonts
      for (int i = 0; i < info->count; ++i)
      {
        SkPath glyphPath;
        if (info->font.getPath(info->glyphs[i], &glyphPath))
        {
          // Calculate the absolute position including the text offset within the fragment
          SkMatrix transform = SkMatrix::Translate(
            info->origin.x() + info->positions[i].x() + offsetX,
            info->origin.y() + info->positions[i].y() + offsetY);
          textPath.addPath(glyphPath, transform);
        }
      }
    };
    paragraph->visit(addPath);

    return textPath.isEmpty()
             ? nullopt
             : optional<SkPath>(textPath);
  }

  // Draw the background for a fragment, returning an optional SkPaint if a fill is drawn.
  optional<SkPaint> RenderBackgroundSystem::drawBackground(SkCanvas *canvas,
                                                           SkRRect &originalRRect,
                                                           ClippingArea &clipInfo,
                                                           const client_layout::Fragment &fragment,
                                                           const client_cssom::ComputedStyle &style,
                                                           bool &textureRequired)
  {
    optional<SkPaint> fillPaint = nullopt;

    // Mark the texture as not required by default.
    textureRequired = false;

    // If we have a clip path or rounded rect, we need to use texture.
    if (!clipInfo.isEmpty())
      textureRequired = true;

    SkRRect roundedRect;
    roundedRect.setRect(originalRRect.rect());

    if (style.hasBackgroundColor())
    {
      auto color = style.backgroundColor().resolveToAbsoluteColor();

      fillPaint = make_optional<SkPaint>();
      fillPaint->setColor(color);
      fillPaint->setAntiAlias(true);
      fillPaint->setStyle(SkPaint::kFill_Style);
      drawRRect(canvas,
                roundedRect,
                fillPaint.value(),
                clipInfo);
    }

    if (style.hasBackgroundImage())
    {
      // Init the fill paint if it hasn't been set yet.
      if (!fillPaint.has_value())
        fillPaint = make_optional<SkPaint>();

      // Reset the fill paint properties.
      fillPaint->setColor(SK_ColorBLACK);
      fillPaint->setAntiAlias(true);
      fillPaint->setStyle(SkPaint::kFill_Style);

      // Set the blend mode for the paint if the background blend mode is not normal.
      if (!style.backgroundBlendMode().isNormal())
        fillPaint->setBlendMode(style.backgroundBlendMode());

      const auto &image = style.backgroundImage();
      if (image.isUrl())
      {
        if (image.isUrlImageLoaded())
        {
          SkBitmap bitmap;
          // TODO(yorkie): support decoding this async?
          if (canvas::ImageCodec::Decode(image.getUrlImageData(),
                                         nullptr,
                                         bitmap,
                                         image.getUrl()))
          {
            canvas->save();
            {
              // Handle the clipping area if specified.
              if (clipInfo.isPath())
                canvas->clipPath(clipInfo.path(), true);
              else if (clipInfo.isRRect())
                canvas->clipRRect(clipInfo.roundedRect(), true);

              // Get the background positioning area based on background-origin
              SkRect positioningArea = getBackgroundPositioningArea(roundedRect, fragment, style);

              // Get the repeatable area based on background-clip (same as clipping area)
              SkRRect repeatableRRect = getBackgroundClippingArea(roundedRect, fragment, style);
              SkRect repeatableArea = repeatableRRect.rect();

              drawImage(canvas, bitmap.asImage(), positioningArea, repeatableArea, fillPaint.value(), style);
            }
            canvas->restore();
            textureRequired = true;
          }
        }
        else
        {
          // NOTE(yorkie): If the image is not loaded yet, just wait for the image to be loaded.
        }
      }
      else if (image.isGradient())
      {
        // Create gradient shader using the new helper method
        sk_sp<SkShader> shader = createGradientShader(image, originalRRect);

        // Apply the shader if successfully created
        if (shader)
        {
          fillPaint->setShader(shader);
          drawRRect(canvas,
                    roundedRect,
                    fillPaint.value(),
                    clipInfo);
          textureRequired = true;
        }
      }
    }
    return fillPaint;
  }

  void RenderBackgroundSystem::drawRRect(SkCanvas *canvas,
                                         const SkRRect &roundedRect,
                                         const SkPaint &paint,
                                         const ClippingArea &clipInfo)
  {
    if (clipInfo.isEmpty())
    {
      canvas->drawRRect(roundedRect, paint);
    }
    else
    {
      canvas->save();
      if (clipInfo.isPath())
        canvas->clipPath(clipInfo.path(), true);
      else if (clipInfo.isRRect())
        canvas->clipRRect(clipInfo.roundedRect(), true);
      canvas->drawRRect(roundedRect, paint);
      canvas->restore();
    }
  }

  void RenderBackgroundSystem::drawImage(SkCanvas *canvas,
                                         const sk_sp<SkImage> &image,
                                         const SkRect &positioningArea,
                                         const SkPaint &paint,
                                         const ComputedStyle &style)
  {
    // Use positioning area as repeatable area for backward compatibility
    drawImage(canvas, image, positioningArea, positioningArea, paint, style);
  }

  void RenderBackgroundSystem::drawImage(SkCanvas *canvas,
                                         const sk_sp<SkImage> &image,
                                         const SkRect &positioningArea,
                                         const SkRect &repeatableArea,
                                         const SkPaint &paint,
                                         const ComputedStyle &style)
  {
    if (!image)
      return;

    // Calculate the background size based on background-size property
    SkSize imageSize = calculateBackgroundSize(image, positioningArea, style);
    float imageWidth = imageSize.width();
    float imageHeight = imageSize.height();

    // Calculate the background position based on background-position property
    SkPoint imagePosition = calculateBackgroundPosition(imageSize, positioningArea, style);

    if (style.backgroundRepeat().isRepeat())
    {
      // Repeat both horizontally and vertically
      // Start from the calculated position and tile in both directions
      float startX = imagePosition.x();
      float startY = imagePosition.y();

      // Adjust start position to ensure full coverage of repeatable area
      while (startX > repeatableArea.fLeft)
        startX -= imageWidth;
      while (startY > repeatableArea.fTop)
        startY -= imageHeight;

      for (float y = startY; y < repeatableArea.fBottom; y += imageHeight)
      {
        for (float x = startX; x < repeatableArea.fRight; x += imageWidth)
        {
          SkRect destRect = SkRect::MakeXYWH(x, y, imageWidth, imageHeight);
          // Clip to repeatable area
          if (destRect.intersect(repeatableArea))
          {
            // Calculate source rect proportionally
            float srcLeft = (destRect.fLeft - x) / imageWidth * image->width();
            float srcTop = (destRect.fTop - y) / imageHeight * image->height();
            float srcRight = srcLeft + (destRect.width() / imageWidth * image->width());
            float srcBottom = srcTop + (destRect.height() / imageHeight * image->height());

            SkRect srcRect = SkRect::MakeLTRB(srcLeft, srcTop, srcRight, srcBottom);
            canvas->drawImageRect(image,
                                  srcRect,
                                  destRect,
                                  SkSamplingOptions(),
                                  &paint,
                                  SkCanvas::kStrict_SrcRectConstraint);
          }
        }
      }
    }
    else if (style.backgroundRepeat().isRepeatX())
    {
      // Repeat only horizontally
      float startX = imagePosition.x();
      float y = imagePosition.y();

      // Adjust start position to ensure full coverage of repeatable area
      while (startX > repeatableArea.fLeft)
        startX -= imageWidth;

      for (float x = startX; x < repeatableArea.fRight; x += imageWidth)
      {
        SkRect destRect = SkRect::MakeXYWH(x, y, imageWidth, imageHeight);
        if (destRect.intersect(repeatableArea))
        {
          float srcLeft = (destRect.fLeft - x) / imageWidth * image->width();
          float srcTop = (destRect.fTop - y) / imageHeight * image->height();
          float srcRight = srcLeft + (destRect.width() / imageWidth * image->width());
          float srcBottom = srcTop + (destRect.height() / imageHeight * image->height());

          SkRect srcRect = SkRect::MakeLTRB(srcLeft, srcTop, srcRight, srcBottom);
          canvas->drawImageRect(image,
                                srcRect,
                                destRect,
                                SkSamplingOptions(),
                                &paint,
                                SkCanvas::kStrict_SrcRectConstraint);
        }
      }
    }
    else if (style.backgroundRepeat().isRepeatY())
    {
      // Repeat only vertically
      float x = imagePosition.x();
      float startY = imagePosition.y();

      // Adjust start position to ensure full coverage of repeatable area
      while (startY > repeatableArea.fTop)
        startY -= imageHeight;

      for (float y = startY; y < repeatableArea.fBottom; y += imageHeight)
      {
        SkRect destRect = SkRect::MakeXYWH(x, y, imageWidth, imageHeight);
        if (destRect.intersect(repeatableArea))
        {
          float srcLeft = (destRect.fLeft - x) / imageWidth * image->width();
          float srcTop = (destRect.fTop - y) / imageHeight * image->height();
          float srcRight = srcLeft + (destRect.width() / imageWidth * image->width());
          float srcBottom = srcTop + (destRect.height() / imageHeight * image->height());

          SkRect srcRect = SkRect::MakeLTRB(srcLeft, srcTop, srcRight, srcBottom);
          canvas->drawImageRect(image,
                                srcRect,
                                destRect,
                                SkSamplingOptions(),
                                &paint,
                                SkCanvas::kStrict_SrcRectConstraint);
        }
      }
    }
    else if (style.backgroundRepeat().isNoRepeat())
    {
      // No repeat - draw once at the calculated position and size
      SkRect destRect = SkRect::MakeXYWH(imagePosition.x(), imagePosition.y(), imageWidth, imageHeight);
      if (destRect.intersect(repeatableArea))
      {
        // Calculate source rect proportionally
        float scaleX = imageWidth / image->width();
        float scaleY = imageHeight / image->height();
        float srcLeft = (destRect.fLeft - imagePosition.x()) / scaleX;
        float srcTop = (destRect.fTop - imagePosition.y()) / scaleY;
        float srcRight = srcLeft + destRect.width() / scaleX;
        float srcBottom = srcTop + destRect.height() / scaleY;

        SkRect srcRect = SkRect::MakeLTRB(srcLeft, srcTop, srcRight, srcBottom);
        canvas->drawImageRect(image,
                              srcRect,
                              destRect,
                              SkSamplingOptions(),
                              &paint,
                              SkCanvas::kStrict_SrcRectConstraint);
      }
    }
    else
    {
      // Default to no repeat for unsupported values (space, round)
      SkRect destRect = SkRect::MakeXYWH(imagePosition.x(), imagePosition.y(), imageWidth, imageHeight);
      if (destRect.intersect(repeatableArea))
      {
        canvas->drawImageRect(image, destRect, SkSamplingOptions(), &paint);
      }
    }
  }

  bool RenderImageSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto imageComponent = getComponent<Image2d>(entity);
    if (imageComponent == nullptr ||
        !imageComponent->hasImageData())
      return false;

    // Disable using texture if the image is not visible.
    if (!imageComponent->visible())
    {
      content.setTextureUsing(false);
      return true;
    }

    sk_sp<SkImage> skImage = imageComponent->image();
    if (skImage == nullptr)
    {
      // Disable using texture if the image is failed to load.
      content.setTextureUsing(false);
      return true;
    }

    SkCanvas *canvas = content.canvas();
    canvas->save();
    {
      SkRRect &roundedRect = content.rounded_rect_;
      canvas->clipRRect(roundedRect, true);

      SkPaint imagePaint;
      imagePaint.setAntiAlias(true);
      imagePaint.setStyle(SkPaint::kFill_Style);

      SkRect srcRect = SkRect::MakeWH(skImage->width(), skImage->height());
      SkRect dstRect = SkRect::MakeWH(content.logicalWidth(), content.logicalHeight());
      canvas->drawImageRect(skImage,
                            srcRect,
                            dstRect,
                            SkSamplingOptions(),
                            &imagePaint,
                            SkCanvas::kStrict_SrcRectConstraint);
    }
    canvas->restore();
    content.setTextureUsing(true);
    return true;
  }

  RenderTextSystem::RenderTextSystem()
      : RenderContentBaseSystem()
      , clientContext_(TrClientContextPerProcess::Get())
      , fontCollection_(clientContext_->getFontCacheManager())
      , paragraphBuilder_(nullptr)
      , sdfGenerator_(text::sdf::SDFParams(6, 0.25f))
  {
  }

  bool RenderTextSystem::render(ecs::EntityId entity, WebContent &content)
  {
    auto textComponent = getComponent<Text2d>(entity);
    if (textComponent == nullptr) [[unlikely]]
      return false;

    string &text = textComponent->content;
    SkCanvas *canvas = content.canvas();
    if (canvas == nullptr) [[unlikely]]
      return false;

    // 1. Render text normally using the original paragraph rendering
    {
      auto paragraphStyle = content.paragraphStyle();
      auto paragraphBuilder = ParagraphBuilder::make(paragraphStyle, fontCollection_);
      paragraphBuilder->pushStyle(paragraphStyle.getTextStyle());
      paragraphBuilder->addText(text.c_str(), text.size());
      paragraphBuilder->pop();

      auto layoutWidth = round(getLayoutWidthForText(content)) + 1.0f;
      auto paragraph = paragraphBuilder->Build();
      paragraph->layout(layoutWidth);
      paragraph->paint(canvas, 0.0f, 0.0f);
    }

    // 2. generate SDF texture from the painted canvas for anti-aliasing
    {
      auto usingSdf = generateSignedDistanceOn(canvas);
      content.setIsSDFTexture(usingSdf);
    }

    // 3. Mark the content as using texture
    content.setTextureUsing(true);
    return true;
  }

  float RenderTextSystem::getLayoutWidthForText(WebContent &content)
  {
    const auto &fragment = content.fragment();
    return fragment->contentWidth();
  }

  bool RenderTextSystem::generateSignedDistanceOn(SkCanvas *canvas)
  {
    // Get the surface from the canvas to extract pixel data
    auto surface = canvas->getSurface();
    if (!surface)
      return false;

    // Get writable pixel data
    SkPixmap pixmap;
    if (!surface->peekPixels(&pixmap))
      return false;

    unsigned char *pixels = (unsigned char *)pixmap.writable_addr();
    if (!pixels)
      return false;

    int width = pixmap.width();
    int height = pixmap.height();

    // Use `SDFGenerator` to generate from the pixel data to the alpha channel.
    bool success = sdfGenerator_.generateOnPixels(pixels, width, height);
    return success;
  }

  void UpdateTextureSystem::onExecute()
  {
    auto material3d = getInstancedMeshComponent<MeshMaterial3d>();
    assert(material3d != nullptr);

    auto webContentInstancedMaterial = material3d->material<materials::WebContentInstancedMaterial>();
    if (webContentInstancedMaterial == nullptr) [[unlikely]]
      return;

    auto selectContents = [](const WebContent &content) -> bool
    {
      return content.canvas() != nullptr && content.isSurfaceDirty();
    };
    auto list = queryEntitiesWithComponent<WebContent>(selectContents);
    if (list.size() > 0)
    {
      for (auto &item : list)
      {
        auto content = item.second;
        // Use the same texture update method for both text and image content
        auto status = webContentInstancedMaterial->updateTexture(*content);
        if (status != materials::WebContentInstancedMaterial::TextureUpdateStatus::kFailed)
        {
          content->setContentDirty(false);
          content->setSurfaceDirty(false);
        }
      }
    }
  }
}
