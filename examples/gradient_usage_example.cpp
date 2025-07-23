/**
 * Example usage of the gradient data extraction API
 * 
 * This example demonstrates how to use the new gradient extraction methods
 * to render CSS gradients in a web content renderer or similar graphics context.
 */

#include <client/cssom/values/computed/image.hpp>
#include <client/cssom/values/generics/image.hpp>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/effects/SkGradientShader.h>

using namespace client_cssom::values::computed;
using namespace client_cssom::values::generics;

/**
 * Example function showing how to detect and process gradient images
 */
void processBackgroundImage(const Image& image) {
    if (image.isNone()) {
        // Handle no background
        return;
    }
    
    if (image.isUrl()) {
        // Handle URL-based images
        std::string url = image.getUrl();
        // Load and render image from URL...
        return;
    }
    
    if (image.isGradient()) {
        // Handle gradient backgrounds
        bool isRepeating = image.isGradientRepeating();
        
        const auto* linearGradient = image.getLinearGradient();
        if (linearGradient) {
            renderLinearGradient(*linearGradient, isRepeating);
            return;
        }
        
        const auto* radialGradient = image.getRadialGradient();
        if (radialGradient) {
            renderRadialGradient(*radialGradient, isRepeating);
            return;
        }
    }
}

/**
 * Example function to render a linear gradient
 */
void renderLinearGradient(const Gradient::LinearGradient& gradient, bool repeating) {
    // Analyze gradient direction
    switch (gradient.direction) {
        case LineDirection::kToRight:
            // Configure left-to-right gradient
            break;
        case LineDirection::kToBottom:
            // Configure top-to-bottom gradient
            break;
        case LineDirection::kToBottomRight:
            // Configure diagonal gradient
            break;
        // ... handle other directions
    }
    
    // Process color stops
    for (const auto& item : gradient.items) {
        switch (item.type) {
            case GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop: {
                const auto& colorStop = std::get<
                    typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop
                >(item.value);
                
                SkColor color = colorStop.color.resolveToAbsoluteColor();
                // Use color at automatically distributed position
                break;
            }
            
            case GenericGradientItem<Color, LengthPercentage>::kComplexColorStop: {
                const auto& colorStop = std::get<
                    typename GenericGradientItem<Color, LengthPercentage>::ComplexColorStop
                >(item.value);
                
                SkColor color = colorStop.color.resolveToAbsoluteColor();
                // Use color at specified position (colorStop.length_percentage)
                // Position conversion would need to be implemented based on rect size
                break;
            }
            
            case GenericGradientItem<Color, LengthPercentage>::kInterpolationHint: {
                const auto& hint = std::get<
                    typename GenericGradientItem<Color, LengthPercentage>::InterpolationHint
                >(item.value);
                
                // Use hint.length_percentage to adjust color interpolation
                break;
            }
        }
    }
}

/**
 * Example function to render a radial gradient
 */
void renderRadialGradient(const Gradient::RadialGradient& gradient, bool repeating) {
    // Analyze gradient shape and size
    bool isCircle = (gradient.shape == Gradient::RadialGradient::kCircle);
    
    switch (gradient.size) {
        case Gradient::RadialGradient::kClosestSide:
            // Use distance to closest side as radius
            break;
        case Gradient::RadialGradient::kFarthestCorner:
            // Use distance to farthest corner as radius
            break;
        // ... handle other size types
    }
    
    // Process color stops (same logic as linear gradient)
    for (const auto& item : gradient.items) {
        // ... same processing as in renderLinearGradient
    }
}

/**
 * Complete example showing integration with Skia rendering
 */
void drawGradientToCanvas(SkCanvas* canvas, const SkRect& rect, const Image& image) {
    if (!image.isGradient()) return;
    
    sk_sp<SkShader> shader = nullptr;
    SkTileMode tileMode = image.isGradientRepeating() ? SkTileMode::kRepeat : SkTileMode::kClamp;
    
    // Handle linear gradient
    const auto* linearGradient = image.getLinearGradient();
    if (linearGradient) {
        // Calculate gradient points based on direction
        SkPoint pts[2];
        switch (linearGradient->direction) {
            case LineDirection::kToRight:
                pts[0] = SkPoint::Make(rect.fLeft, rect.centerY());
                pts[1] = SkPoint::Make(rect.fRight, rect.centerY());
                break;
            case LineDirection::kToBottom:
                pts[0] = SkPoint::Make(rect.centerX(), rect.fTop);
                pts[1] = SkPoint::Make(rect.centerX(), rect.fBottom);
                break;
            // ... handle other directions
            default:
                pts[0] = SkPoint::Make(rect.fLeft, rect.centerY());
                pts[1] = SkPoint::Make(rect.fRight, rect.centerY());
                break;
        }
        
        // Extract colors and positions
        std::vector<SkColor4f> colors;
        std::vector<SkScalar> positions;
        
        for (const auto& item : linearGradient->items) {
            if (item.type == GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop) {
                const auto& colorStop = std::get<
                    typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop
                >(item.value);
                colors.push_back(SkColor4f::FromColor(colorStop.color.resolveToAbsoluteColor()));
            }
            // ... handle other item types
        }
        
        // Generate positions if not explicitly provided
        if (positions.size() != colors.size()) {
            positions.clear();
            for (size_t i = 0; i < colors.size(); ++i) {
                positions.push_back(colors.size() > 1 ? (float)i / (colors.size() - 1) : 0.0f);
            }
        }
        
        shader = SkGradientShader::MakeLinear(pts, colors.data(), 
                                            SkColorSpace::MakeSRGB(),
                                            positions.data(), colors.size(), 
                                            tileMode);
    }
    
    // Handle radial gradient
    const auto* radialGradient = image.getRadialGradient();
    if (radialGradient && !shader) {
        SkPoint center = SkPoint::Make(rect.centerX(), rect.centerY());
        
        // Calculate radius based on size type
        SkScalar radius;
        switch (radialGradient->size) {
            case Gradient::RadialGradient::kClosestSide:
                radius = std::min(rect.width(), rect.height()) / 2.0f;
                break;
            case Gradient::RadialGradient::kFarthestCorner:
            default:
                radius = std::sqrt(std::pow(rect.width() / 2.0f, 2) + 
                                 std::pow(rect.height() / 2.0f, 2));
                break;
        }
        
        // Extract colors (same logic as linear gradient)
        std::vector<SkColor4f> colors;
        std::vector<SkScalar> positions;
        // ... extract colors and positions from radialGradient->items
        
        shader = SkGradientShader::MakeRadial(center, radius, colors.data(),
                                            SkColorSpace::MakeSRGB(),
                                            positions.data(), colors.size(),
                                            tileMode);
    }
    
    // Apply shader and draw
    if (shader) {
        SkPaint paint;
        paint.setShader(shader);
        paint.setAntiAlias(true);
        canvas->drawRect(rect, paint);
    }
}

/**
 * Example showing how to create gradients programmatically for testing
 */
Image createTestLinearGradient() {
    // Create linear gradient structure
    typename Gradient::LinearGradient linearGrad;
    linearGrad.direction = LineDirection::kToRight;
    
    // Add color stops
    GenericGradientItem<Color, LengthPercentage> redStop;
    redStop.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    redStop.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{
        Color::Black()
    };
    linearGrad.items.push_back(redStop);
    
    GenericGradientItem<Color, LengthPercentage> blueStop;
    blueStop.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    blueStop.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{
        Color::White()
    };
    linearGrad.items.push_back(blueStop);
    
    // Create gradient and image
    Gradient gradient(linearGrad);
    return Image(gradient);
}

/**
 * Example showing how to inspect gradient properties
 */
void inspectGradient(const Image& image) {
    if (!image.isGradient()) {
        printf("Not a gradient image\n");
        return;
    }
    
    printf("Gradient detected:\n");
    printf("  Repeating: %s\n", image.isGradientRepeating() ? "yes" : "no");
    
    const auto* linearGradient = image.getLinearGradient();
    if (linearGradient) {
        printf("  Type: Linear\n");
        printf("  Direction: %d\n", static_cast<int>(linearGradient->direction));
        printf("  Color stops: %zu\n", linearGradient->items.size());
        return;
    }
    
    const auto* radialGradient = image.getRadialGradient();
    if (radialGradient) {
        printf("  Type: Radial\n");
        printf("  Shape: %s\n", radialGradient->shape == Gradient::RadialGradient::kCircle ? "circle" : "ellipse");
        printf("  Size: %d\n", static_cast<int>(radialGradient->size));
        printf("  Color stops: %zu\n", radialGradient->items.size());
        return;
    }
    
    printf("  Type: Unknown\n");
}