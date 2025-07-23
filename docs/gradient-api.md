# Gradient Data Extraction API

This document describes the API for extracting gradient data from computed image values in JSAR Runtime.

## Overview

The `client_cssom::values::computed::Image` class has been extended with methods to extract gradient information for rendering purposes. This enables proper CSS gradient background rendering in the web content renderer.

## API Methods

### Gradient Detection

```cpp
// Check if the image is a gradient
bool isGradient() const;

// Check if the gradient is repeating (only valid if isGradient() returns true)
bool isGradientRepeating() const;
```

### Gradient Data Extraction

```cpp
// Get the base gradient data (returns nullptr if not a gradient)
const Gradient* getGradient() const;

// Get linear gradient data (returns nullptr if not a linear gradient)
const typename Gradient::LinearGradient* getLinearGradient() const;

// Get radial gradient data (returns nullptr if not a radial gradient)
const typename Gradient::RadialGradient* getRadialGradient() const;
```

## Gradient Structure

### Linear Gradient
```cpp
class LinearGradient {
public:
    LineDirection direction;  // Direction of the gradient
    std::vector<GenericGradientItem<Color, LengthPercentage>> items;  // Color stops
};
```

**Line Directions:**
- `kToLeft` - Gradient flows from right to left
- `kToRight` - Gradient flows from left to right  
- `kToTop` - Gradient flows from bottom to top
- `kToBottom` - Gradient flows from top to bottom
- `kToTopLeft` - Gradient flows diagonally to top-left
- `kToTopRight` - Gradient flows diagonally to top-right
- `kToBottomLeft` - Gradient flows diagonally to bottom-left
- `kToBottomRight` - Gradient flows diagonally to bottom-right

### Radial Gradient
```cpp
class RadialGradient {
public:
    enum Shape { kCircle, kEllipse };
    enum Size { kClosestSide, kClosestCorner, kFarthestSide, kFarthestCorner };
    
    Shape shape;
    Size size;
    std::vector<GenericGradientItem<Color, LengthPercentage>> items;  // Color stops
};
```

### Gradient Items (Color Stops)
```cpp
enum ItemType {
    kSimpleColorStop,     // Color without position
    kComplexColorStop,    // Color with position
    kInterpolationHint    // Position hint for color interpolation
};
```

## Usage Examples

### Basic Gradient Detection
```cpp
#include <client/cssom/values/computed/image.hpp>

void processBackgroundImage(const client_cssom::values::computed::Image& image) {
    if (image.isGradient()) {
        const auto* gradient = image.getGradient();
        if (gradient->repeating) {
            // Handle repeating gradient
        }
    }
}
```

### Linear Gradient Processing
```cpp
void renderLinearGradient(const client_cssom::values::computed::Image& image) {
    const auto* linearGradient = image.getLinearGradient();
    if (linearGradient) {
        // Process direction
        auto direction = linearGradient->direction;
        
        // Process color stops
        for (const auto& item : linearGradient->items) {
            if (item.type == GenericGradientItem::kSimpleColorStop) {
                const auto& colorStop = std::get<SimpleColorStop>(item.value);
                SkColor color = colorStop.color.resolveToAbsoluteColor();
                // Use color...
            }
            else if (item.type == GenericGradientItem::kComplexColorStop) {
                const auto& colorStop = std::get<ComplexColorStop>(item.value);
                SkColor color = colorStop.color.resolveToAbsoluteColor();
                // Use color and position (colorStop.length_percentage)...
            }
        }
    }
}
```

### Radial Gradient Processing
```cpp
void renderRadialGradient(const client_cssom::values::computed::Image& image) {
    const auto* radialGradient = image.getRadialGradient();
    if (radialGradient) {
        // Process shape and size
        bool isCircle = (radialGradient->shape == RadialGradient::kCircle);
        auto sizeType = radialGradient->size;
        
        // Process color stops (same as linear gradient)
        for (const auto& item : radialGradient->items) {
            // ... same processing as linear gradient
        }
    }
}
```

### Complete Example in Web Content Renderer
```cpp
void drawGradientBackground(SkCanvas* canvas, const SkRect& rect, 
                           const client_cssom::values::computed::Image& image) {
    if (!image.isGradient()) return;
    
    sk_sp<SkShader> shader = nullptr;
    SkTileMode tileMode = image.isGradientRepeating() ? SkTileMode::kRepeat : SkTileMode::kClamp;
    
    // Handle linear gradient
    const auto* linearGradient = image.getLinearGradient();
    if (linearGradient) {
        // Calculate points based on direction
        SkPoint pts[2];
        switch (linearGradient->direction) {
            case LineDirection::kToRight:
                pts[0] = SkPoint::Make(rect.fLeft, rect.centerY());
                pts[1] = SkPoint::Make(rect.fRight, rect.centerY());
                break;
            // ... handle other directions
        }
        
        // Extract colors and positions
        std::vector<SkColor4f> colors;
        std::vector<SkScalar> positions;
        
        for (const auto& item : linearGradient->items) {
            if (item.type == GenericGradientItem::kSimpleColorStop) {
                const auto& colorStop = std::get<SimpleColorStop>(item.value);
                colors.push_back(SkColor4f::FromColor(colorStop.color.resolveToAbsoluteColor()));
                // Calculate position...
            }
        }
        
        shader = SkGradientShader::MakeLinear(pts, colors.data(), 
                                            SkColorSpace::MakeSRGB(),
                                            positions.data(), colors.size(), 
                                            tileMode);
    }
    
    // Handle radial gradient
    const auto* radialGradient = image.getRadialGradient();
    if (radialGradient) {
        SkPoint center = SkPoint::Make(rect.centerX(), rect.centerY());
        SkScalar radius = /* calculate based on size and shape */;
        
        // Extract colors (same as linear)...
        
        shader = SkGradientShader::MakeRadial(center, radius, colors.data(),
                                            SkColorSpace::MakeSRGB(),
                                            positions.data(), colors.size(),
                                            tileMode);
    }
    
    // Apply shader to paint and draw
    if (shader) {
        SkPaint paint;
        paint.setShader(shader);
        canvas->drawRect(rect, paint);
    }
}
```

## Notes

1. **Color Resolution**: Use `color.resolveToAbsoluteColor()` to convert computed colors to Skia colors.

2. **Position Handling**: Complex color stops include position information in `length_percentage`. Currently, the web renderer uses simple position distribution, but this can be enhanced to properly handle percentage and length values.

3. **Interpolation Hints**: The API supports interpolation hints (`kInterpolationHint` items), but the current renderer implementation focuses on color stops. Interpolation hints can be used to control how colors blend between stops.

4. **Error Handling**: All getter methods return `nullptr` when the requested gradient type is not available. Always check for null pointers before using the returned data.

5. **Thread Safety**: The gradient data is immutable once created, making it safe to access from multiple threads.

## Benefits

- **Standards Compliance**: Proper CSS gradient rendering according to web standards
- **Visual Fidelity**: High-quality gradient rendering using Skia's gradient shaders
- **Performance**: Direct access to parsed gradient data without re-parsing
- **Extensibility**: API supports future gradient types (conic gradients, etc.)