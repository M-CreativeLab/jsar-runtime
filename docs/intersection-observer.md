# IntersectionObserver Implementation

This document describes the implementation of the IntersectionObserver API for JSAR Runtime.

## Overview

The IntersectionObserver API allows you to observe changes in the intersection of a target element with an ancestor element or with a top-level document's viewport. This implementation follows the [W3C Intersection Observer API specification](https://www.w3.org/TR/intersection-observer/) and the [MDN documentation](https://developer.mozilla.org/en-US/docs/Web/API/Intersection_Observer_API).

## Files Added

### C++ Implementation
- `src/client/dom/intersection_observer.hpp` - Header file defining the core classes
- `src/client/dom/intersection_observer.cpp` - Implementation of the core IntersectionObserver logic

### JavaScript Bindings
- `src/bindings/dom/intersection_observer.hpp` - N-API binding header
- `src/bindings/dom/intersection_observer.cpp` - N-API binding implementation

### Integration
- `src/bindings/dom/binding.cpp` - Updated to register IntersectionObserver

### Test Files
- `fixtures/html/intersection-observer-test.html` - HTML test page demonstrating the API

## API Implementation

### IntersectionObserver Constructor

```javascript
const observer = new IntersectionObserver(callback, options);
```

**Parameters:**
- `callback` (function): Called when intersection changes occur
- `options` (object, optional): Configuration options

**Options:**
- `root` (Element, optional): Root element for intersection calculations (default: viewport)
- `rootMargin` (string, optional): Margin around the root (default: "0px")
- `threshold` (number|Array, optional): Intersection ratio thresholds (default: [0])

### Methods

#### observe(target)
Start observing a target element for intersection changes.

```javascript
observer.observe(element);
```

#### unobserve(target)
Stop observing a specific target element.

```javascript
observer.unobserve(element);
```

#### disconnect()
Stop observing all target elements.

```javascript
observer.disconnect();
```

#### takeRecords()
Return and clear all pending intersection records.

```javascript
const records = observer.takeRecords();
```

### Properties

#### root (readonly)
The root element used for intersection calculations.

#### rootMargin (readonly)
The margin around the root element.

#### thresholds (readonly)
Array of intersection ratio thresholds.

### IntersectionObserverEntry

Each callback receives an array of `IntersectionObserverEntry` objects with these properties:

- `target` (Element): The observed element
- `boundingClientRect` (DOMRect): Bounding rectangle of the target
- `intersectionRect` (DOMRect): Rectangle representing the intersection
- `intersectionRatio` (number): Ratio of intersection area to target area (0.0 to 1.0)
- `isIntersecting` (boolean): True if target intersects with root
- `rootBounds` (DOMRect|null): Bounding rectangle of the root
- `time` (number): Timestamp when intersection was recorded

## Implementation Details

### Core Algorithm

1. **Bounding Rectangle Calculation**: Uses `Element::getBoundingClientRect()` to get element positions
2. **Viewport Detection**: Automatically detects viewport size from browser window when no root is specified
3. **Intersection Calculation**: Computes rectangle intersection using geometric algorithms
4. **Threshold Checking**: Compares intersection ratio against configured thresholds
5. **Callback Scheduling**: Uses V8 microtask queue for asynchronous callback execution

### Viewport Size Detection

When no custom root is specified, the implementation:
1. Gets the target element's owner document
2. Casts to HTMLDocument to access defaultView()
3. Uses window.innerWidth() and window.innerHeight() for viewport dimensions
4. Falls back to 1920x1080 if window is not available

### Root Margin Parsing

Supports CSS-style margin values:
- Single value: `"10px"` (applied to all sides)
- Four values: `"10px 20px 30px 40px"` (top, right, bottom, left)
- Currently supports pixel values only

### Memory Management

- Uses `std::shared_ptr` for safe memory management
- Follows existing JSAR Runtime patterns from MutationObserver
- Thread-safe with `std::shared_mutex` for concurrent access

## Example Usage

```javascript
// Create observer with multiple thresholds
const observer = new IntersectionObserver((entries, observer) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      console.log(`Element is ${Math.round(entry.intersectionRatio * 100)}% visible`);
      entry.target.classList.add('visible');
    } else {
      entry.target.classList.remove('visible');
    }
  });
}, {
  root: document.querySelector('.container'),
  rootMargin: '10px',
  threshold: [0, 0.25, 0.5, 0.75, 1.0]
});

// Observe elements
document.querySelectorAll('.target').forEach(el => {
  observer.observe(el);
});
```

## Testing

The implementation includes:
1. **Core Logic Tests**: C++ unit tests for intersection calculation algorithms
2. **HTML Demo**: Interactive test page showing real-world usage
3. **Integration Tests**: Verification of JavaScript binding functionality

Run the HTML test file in JSAR Runtime to see the implementation in action:
```
fixtures/html/intersection-observer-test.html
```

## Limitations and Future Improvements

### Current Limitations
1. **No Automatic Triggering**: Intersection checks only occur when `observe()` is called
2. **Static Calculations**: No integration with scroll/resize/animation events
3. **Limited Root Margin**: Only supports pixel values, not percentages
4. **No Implicit Root**: Custom root element extraction from JS objects needs refinement

### Planned Improvements
1. **Event Integration**: Hook into scroll, resize, and layout change events
2. **Performance Optimization**: Batch intersection calculations and use RAF timing
3. **Enhanced Root Margin**: Support for percentage and mixed unit values
4. **Better Error Handling**: More robust error cases and edge conditions
5. **Animation Frame Integration**: Align with browser rendering pipeline

## Architecture Notes

The implementation follows JSAR Runtime's established patterns:
- **Separation of Concerns**: Core logic in `src/client/dom/`, JS bindings in `src/bindings/dom/`
- **Memory Safety**: RAII and smart pointers throughout
- **Error Handling**: Graceful degradation with fallbacks
- **Thread Safety**: Proper synchronization for multi-threaded access
- **API Compatibility**: Matches W3C specification and MDN documentation

This implementation provides a solid foundation for intersection-based features like lazy loading, infinite scrolling, and view tracking in JSAR Runtime applications.