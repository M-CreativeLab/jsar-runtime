# XR Touchpad Input System

This document describes the touchpad input handling system in JSAR Runtime, which processes 2D touchpad input from XR devices and synthesizes DOM events.

## Overview

The XR touchpad input system provides:
- **WebXR Gamepad API compliance**: Touchpad state exposed via `XRInputSource.gamepad`
- **Scroll mode with kinetic inertia**: Default mode emits WheelEvent for DOM scrolling
- **Performance optimizations**: 16ms throttling, early zero-offset exit, value clamping
- **Future extensibility**: Placeholders for cursor mode, double-tap, long-press

## Architecture

### Input Pipeline

```
Unity C# Touchpad Input
    ↓
Unity C++ Plugin API
    ↓
Shared Memory (TrXRInputSource)
    ↓
C++ Client (XRInputSource + Gamepad)
    ↓
TouchpadInputProcessor
    ↓
V8 Bindings (WheelEvent synthesis)
    ↓
DOM Event Dispatch
```

### Key Components

1. **TrXRInputSource** (`src/common/xr/input_sources.hpp`)
   - Shared memory structure containing touchpad state
   - Fields: `axes[]`, `buttons[]`, `touchpadFingerCount`

2. **Gamepad** (`src/client/xr/webxr_gamepad.hpp`)
   - WebXR Gamepad Module implementation
   - Exposes axes and buttons to JavaScript

3. **TouchpadInputProcessor** (`src/client/xr/touchpad_input_processor.hpp`)
   - Processes touchpad input each frame
   - Implements scroll mode with inertia
   - Synthesizes WheelEvent

4. **WheelEvent** (`src/client/dom/events/wheel_event.hpp`)
   - DOM WheelEvent implementation
   - V8 bindings for JavaScript access

## WebXR Gamepad Mapping

For a touchpad input source:

- **`axes[0]`**: Touchpad X position (normalized -1 to 1)
- **`axes[1]`**: Touchpad Y position (normalized -1 to 1)
- **`buttons[0].touched`**: Finger contact with touchpad
- **`buttons[0].pressed`**: Physical click (same as touched, no pressure)
- **`buttons[0].value`**: 0.0 or 1.0 (no pressure sensing)

### Example JavaScript

```javascript
session.requestAnimationFrame((time, frame) => {
  for (const inputSource of frame.session.inputSources) {
    const gamepad = inputSource.gamepad;
    if (gamepad) {
      console.log('Touchpad X:', gamepad.axes[0]);
      console.log('Touchpad Y:', gamepad.axes[1]);
      console.log('Touched:', gamepad.buttons[0].touched);
    }
  }
});
```

## Touchpad Modes

### Scroll Mode (Default)

When the user slides their finger on the touchpad:
- Delta calculation: `current_position - previous_position`
- Scaling: `delta * scrollScale` (default: 40px/unit)
- Throttling: Max 60fps (16ms intervals)
- Inertia: Exponential decay when finger lifts (τ=300ms)

#### Inertia Model

Kinetic scrolling uses exponential decay:

```
velocity(t) = velocity₀ × e^(-t/τ)
```

Where:
- `τ` (tau) = decay time constant = 300ms (default)
- velocity₀ = velocity at finger lift
- t = time since finger lift

This creates a natural deceleration effect similar to iOS/Android scrolling.

### Cursor Mode (Future)

Reserved for future implementation. Will control a 2D cursor for pointer interactions.

**Feature flag**: `TouchpadConfig::cursorModeEnabled` (default: false)

## Configuration

### TouchpadConfig

```cpp
struct TouchpadConfig {
  // Input mode
  TouchpadMode mode = TouchpadMode::Scroll;
  
  // Scroll settings
  bool inertiaEnabled = true;
  float decayConstantMs = 300.0f;  // τ in milliseconds
  float scrollScale = 40.0f;       // px per unit
  float deadzone = 0.01f;          // Axis deadzone
  
  // Event throttling
  std::chrono::milliseconds scrollThrottleDuration{16}; // 60fps
  
  // Future features (disabled by default)
  bool doubleTapEnabled = false;
  bool longPressEnabled = false;
  bool cursorModeEnabled = false;
};
```

### Updating Configuration

```cpp
auto inputSource = /* get XRInputSource */;
auto processor = inputSource->touchpadProcessor();
if (processor) {
  TouchpadConfig config = processor->config();
  config.scrollScale = 60.0f;  // Increase scroll sensitivity
  config.decayConstantMs = 200.0f;  // Faster decay
  processor->setConfig(config);
}
```

## Performance Optimizations

Following `docs/internals/DOM_SCROLLING_PERFORMANCE.md` specification:

1. **16ms Throttling**: Events limited to ~60fps maximum
2. **Zero-offset early exit**: Skip processing when delta is zero
3. **Value clamping**: Deltas clamped to [-1000, 1000] range
4. **Deadzone application**: Ignore small axis values (< 0.01)

## Future Work

### Pending Implementation

- **DOM Event Dispatch Integration**: Currently WheelEvent synthesis is a placeholder. Needs:
  - Hit-testing system integration to find target element
  - DOM event dispatch system connection
  - XRSession baseLayer targeting

- **Cursor Mode**: Enable 2D pointer control via touchpad
- **Double-tap Detection**: Quick gesture recognition
- **Long-press Detection**: Hold gesture recognition
- **Multi-finger Gestures**: Mode switching via finger count

### Reserved Feature Flags

These features are designed but disabled by default:

```cpp
// In TouchpadConfig
bool doubleTapEnabled = false;     // Reserved
bool longPressEnabled = false;     // Reserved  
bool cursorModeEnabled = false;    // Reserved
```

## Unity Integration

### Unity C# Side

```csharp
// Example: Setting touchpad state
public void UpdateTouchpad(Vector2 position, bool touching, bool pressed) {
  inputSource.axes[0] = position.x;
  inputSource.axes[1] = position.y;
  inputSource.axesCount = 2;
  
  inputSource.buttons[0].touched = touching;
  inputSource.buttons[0].pressed = pressed;
  inputSource.buttons[0].value = pressed ? 1.0f : 0.0f;
  inputSource.buttonsCount = 1;
  
  inputSource.touchpadFingerCount = touching ? 1 : 0;
}
```

### Unity C++ Plugin

The plugin writes to shared memory (`TrXRInputSource`) which is read by the client each frame.

## Testing

### Manual Testing

Test pages are provided in `fixtures/xr-touchpad/`:

1. **`scroll-vertical.html`**: Vertical scrolling test
2. **`scroll-horizontal.html`**: Horizontal scrolling test
3. **`gamepad-readout.html`**: Real-time gamepad state display

### Example Test Page

```html
<!DOCTYPE html>
<html>
<body style="height: 3000px; padding: 20px;">
  <h1>Touchpad Vertical Scroll Test</h1>
  <div id="scroll-info"></div>
  
  <script>
    let scrollCount = 0;
    window.addEventListener('wheel', (e) => {
      scrollCount++;
      document.getElementById('scroll-info').textContent = 
        `Scrolled ${scrollCount} times. Last: ${e.deltaY.toFixed(2)}px`;
    });
    
    // In XR session
    session.requestAnimationFrame((time, frame) => {
      for (const inputSource of frame.session.inputSources) {
        const gamepad = inputSource.gamepad;
        if (gamepad) {
          console.log('Axes:', gamepad.axes);
          console.log('Buttons:', gamepad.buttons.map(b => ({
            touched: b.touched,
            pressed: b.pressed,
            value: b.value
          })));
        }
      }
    });
  </script>
</body>
</html>
```

## References

- [WebXR Gamepads Module Level 1](https://www.w3.org/TR/webxr-gamepads-module-1/)
- [WheelEvent Specification](https://w3c.github.io/uievents/#interface-wheelevent)
- [Gamepad API](https://w3c.github.io/gamepad/)
- [DOM Scrolling Performance](./DOM_SCROLLING_PERFORMANCE.md)
- [Chromium XR Gamepad Implementation](https://source.chromium.org/chromium/chromium/src/+/main:device/vr/public/cpp/xr_gamepad_data.h)

## See Also

- `docs/manual/basic-concepts/input-sources.md`: Input sources overview
- `docs/manual/references/webxr-inputsources.md`: WebXR input sources reference
- `src/common/xr/input_sources.hpp`: Shared memory structures
- `src/client/xr/touchpad_input_processor.hpp`: Input processor API
