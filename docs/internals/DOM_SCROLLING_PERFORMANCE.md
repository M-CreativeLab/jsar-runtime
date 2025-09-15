# DOM Scrolling Performance Optimizations

This document describes the scroll performance optimizations implemented in the JSAR runtime client code.

## Overview

The scrolling performance has been optimized through several targeted improvements to address performance bottlenecks in scroll-heavy applications.

## 1. Scroll Event Throttling

**Location**: `src/client/dom/element.cpp`, `src/client/dom/element.hpp`, `src/client/dom/document.cpp`, `src/client/dom/document.hpp`

**Problem**: Scroll events were dispatched on every scroll operation without any rate limiting, causing performance issues with rapid scrolling.

**Solution**: Added 16ms throttling (~60fps) to scroll event dispatching for both Element and HTMLDocument classes.

```cpp
// Added to Element and HTMLDocument classes
static constexpr std::chrono::milliseconds scroll_throttle_duration_{16};
std::chrono::steady_clock::time_point last_scroll_event_time_;

bool shouldThrottleScrollEvent() const;
```

**Implementation Details**:
- **Element::simulateScrollWithOffset()**: Already had throttling implemented
- **HTMLDocument::simulateScrollWithOffset()**: Throttling newly added for consistency and performance

**Impact**: Reduces event frequency from unlimited to maximum 60fps, significantly reducing CPU overhead for both element-level and document-level scrolling.

## 2. Optimized Scroll Bounds Checking

**Location**: `src/client/scroll/scrollable_area.cpp`

**Problem**: Inefficient bounds checking with complex conditional logic.

**Solution**: Replaced with `std::clamp()` for cleaner, more efficient bounds validation.

```cpp
// Before
if (xOffset > 0 && scroll_origin_.x + xOffset <= overflow_rect_->x)
    scroll_offset_.x = xOffset;

// After  
new_offset.x = std::clamp(offset.x, 0.0f, max_scroll_x);
```

**Impact**: More efficient bounds checking and cleaner code.

## 3. Zero-Offset Early Exits

**Location**: `src/client/scroll/scrollable_area.hpp`, `src/client/layout/layout_object.cpp`

**Problem**: Expensive operations were performed even when scroll offset was zero.

**Solution**: Added early exit checks for zero offsets.

```cpp
// Skip expensive operations for zero offsets
if (offset.x == 0.0f && offset.y == 0.0f && offset.z == 0.0f)
    return;
```

**Impact**: Prevents unnecessary calculations when no scrolling is needed.

## 4. Viewport Culling Infrastructure

**Location**: `src/client/scroll/scrollable_area.hpp`, `src/client/layout/layout_object.cpp`

**Problem**: No mechanism to skip processing elements outside the visible viewport.

**Solution**: Added viewport intersection checking.

```cpp
bool isFragmentInViewport(const client_layout::Fragment &fragment) const;
```

**Impact**: Foundation for skipping expensive operations on off-screen elements.

## Performance Metrics

- **Event Rate**: Reduced from unlimited to 60fps max
- **CPU Usage**: Significant reduction in layout calculations
- **Memory**: Reduced event object allocation
- **Code maintainability**: Cleaner, more efficient scroll handling logic

## Usage

### Basic Scrolling (with throttling)
```cpp
element->scrollTo({left: 100, top: 50});
element->scrollBy({left: 10, top: -20});
```

## Configuration

### Throttling Rate
```cpp
// In element.hpp - adjust for different frame rates
static constexpr std::chrono::milliseconds scroll_throttle_duration_{16}; // 60fps
```

## Future Enhancements

1. **Actual Rendering Skip**: Use viewport culling to skip rendering off-screen elements
2. **Performance Monitoring**: Add metrics to track scroll performance
3. **Touch Optimization**: Optimize for touch/gesture input on mobile devices
4. **Advanced Throttling**: Adaptive throttling based on scroll velocity

## Testing

All optimizations have been thoroughly tested with:
- Unit tests for throttling logic
- Boundary tests for scroll bounds
- Viewport intersection tests for culling

## Backward Compatibility

All changes maintain full backward compatibility with existing APIs. No breaking changes were introduced.