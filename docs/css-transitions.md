# CSS Transitions Implementation

This document describes the CSS Transitions implementation in JSAR Runtime.

## Architecture Overview

CSS Transitions are implemented across four main modules:

### 1. CSSOM Module (`src/client/cssom/`)
- **Parsing**: Handles parsing of `transition-*` properties from CSS
- **Storage**: Stores computed transition properties in `ComputedStyle`
- **Properties**: Supports `transition-property`, `transition-duration`, `transition-delay`, `transition-timing-function`

**Key Files:**
- `values/specified/animation.hpp` - Transition property parsing
- `computed_style.hpp` - Storage of computed transition properties

### 2. Animation Module (`src/client/animation/`)
- **CSSTransition**: Core transition animation class
- **Timeline Integration**: Manages animation timing via `DocumentTimeline`
- **Value Interpolation**: Handles interpolation between start and end values
- **Animation Effects**: Computes timing, progress, and applies easing functions

**Key Files:**
- `css/css_transition.hpp/.cpp` - Main transition implementation
- `css/css_animations.hpp/.cpp` - Transition management within elements
- `animation_effect.hpp/.cpp` - Timing and progress computation
- `animation.cpp` - Base animation lifecycle and timeline integration

### 3. DOM Module (`src/client/dom/`)
- **Change Detection**: Compares old vs new computed styles
- **Transition Triggering**: Creates transitions when animatable properties change
- **Element Integration**: Associates transitions with their owning elements

**Key Files:**
- `element.cpp` - Style recalculation and transition detection (see `recalcStyleDirectly`)

### 4. Layout Module (`src/client/layout/`)
- **Style Application**: Applies interpolated values during layout computation
- **Integration**: Works with existing computed style system

## Transition Lifecycle

1. **CSS Parsing**: CSS `transition-*` properties are parsed and stored in `ComputedStyle`
2. **Change Detection**: When `Element::recalcStyleDirectly()` is called with new styles:
   - Compare old and new computed styles
   - Identify properties that have `transition-*` rules and changed values
3. **Transition Creation**: For each changed property:
   - Create `CSSTransition` instance with start/end values
   - Create `AnimationEffect` with timing parameters
   - Attach to `DocumentTimeline` for timing updates
4. **Animation Loop**: Each frame, the timeline:
   - Updates current time for all attached animations
   - Animations compute progress and interpolate values
   - Elements with active transitions are marked dirty for re-rendering
5. **Value Application**: During style computation:
   - `CSSTransition::updatePropertyToStyle()` applies interpolated values
   - Values override the base computed style for transitioning properties

## Value Interpolation

The system supports multiple value types:

### Numeric Values
```cpp
// Plain numbers: "0" → "100"
float result = start_num + (end_num - start_num) * progress;
```

### Values with Units
```cpp
// CSS units: "10px" → "50px", "0%" → "100%"
// Units must match between start and end values
parseValueWithUnit(value, number, unit);
```

### Colors
```cpp
// Hex colors: "#ff0000" → "#0000ff"
// Interpolates RGB channels independently
interpolateHexColor(start, end, progress);
```

### Discrete Values
```cpp
// Non-interpolable values use 50% threshold
return progress < 0.5f ? start : end;
```

## Extending the System

### Adding New Value Types
1. Add parsing logic to `CSSTransition::interpolateValue()`
2. Implement type-specific interpolation function
3. Add unit tests for the new value type

### Adding Timing Functions
1. Extend `AnimationEffect` to properly convert CSS timing functions
2. Update timing function evaluation in interpolation
3. Support cubic-bezier, steps, and other CSS timing functions

### Adding Transition Events
1. Implement event dispatching in `Animation::update()`
2. Add `transitionstart`, `transitionend`, `transitioncancel` events
3. Integrate with DOM event system

## Example Usage

```css
.element {
  width: 100px;
  background-color: #ff0000;
  transition: width 1s ease-in-out, background-color 0.5s linear;
}

.element:hover {
  width: 200px;
  background-color: #0000ff;
}
```

When the element is hovered:
1. System detects `width` changed from `100px` to `200px`
2. System detects `background-color` changed from `#ff0000` to `#0000ff`  
3. Creates two `CSSTransition` instances with respective timing
4. Animates both properties simultaneously with different durations and easing

## Performance Considerations

- Transitions only created when values actually change
- Zero-duration transitions are filtered out
- Finished transitions are automatically cleaned up
- Element marking ensures minimal re-computation overhead
- Value interpolation uses efficient numeric operations where possible

## Future Enhancements

- Transform interpolation (matrix decomposition)
- Complex timing functions (cubic-bezier, steps)
- Transition events (transitionstart, transitionend)
- Performance optimizations for large numbers of transitions
- Support for more CSS value types (gradients, shadows, etc.)