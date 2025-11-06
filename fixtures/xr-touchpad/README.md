# XR Touchpad Test Fixtures

Test pages for validating XR touchpad input functionality in JSAR Runtime.

## Test Pages

### 1. `scroll-vertical.html` - Vertical Scrolling Test

Tests touchpad scroll mode with vertical content.

**Features:**
- Long scrollable page (3000px)
- Real-time scroll statistics
- Event rate monitoring
- Visual scroll position indicator

**What to test:**
- Slide finger vertically on touchpad → page should scroll
- Lift finger → kinetic inertia should continue scrolling with decay
- Events should be throttled to ~60fps (16ms intervals)
- Zero movement should not trigger events

### 2. `gamepad-readout.html` - Live Gamepad State Display

Real-time visualization of gamepad state from XR input sources.

**Features:**
- Live axes values with visual bars
- Button state (touched/pressed/value)
- Timestamps and metadata
- Support for multiple input sources

**What to test:**
- Touchpad position → axes values should update (-1 to 1 range)
- Finger touch → button[0].touched should be true
- Physical click → button[0].pressed should be true
- Deadzone → very small movements should be filtered

## Usage

### In JSAR Runtime

1. Build and run JSAR Runtime with XR support
2. Load one of the test pages
3. Start an XR session
4. Use your XR controller's touchpad to interact

### Manual Testing Checklist

#### Scroll Mode
- [ ] Vertical scroll works smoothly
- [ ] Scroll speed feels natural (adjust `scrollScale` if needed)
- [ ] Kinetic inertia continues after finger lift
- [ ] Inertia decay feels natural (~300ms)
- [ ] Events are throttled to ~60fps
- [ ] No jitter or stuttering
- [ ] Scroll direction matches touchpad direction
- [ ] Zero movement doesn't trigger events

#### Gamepad API
- [ ] `inputSource.gamepad` is not null
- [ ] `axes[0]` updates with horizontal movement
- [ ] `axes[1]` updates with vertical movement
- [ ] Axes are in range [-1, 1]
- [ ] `buttons[0].touched` reflects finger contact
- [ ] `buttons[0].pressed` reflects click
- [ ] `buttons[0].value` is 0.0 or 1.0
- [ ] Deadzone filters small movements
- [ ] `gamepad.mapping` is "xr-standard"
- [ ] `gamepad.timestamp` updates each frame

#### Performance
- [ ] No frame drops during scrolling
- [ ] Event rate stays at ~60fps
- [ ] CPU usage is reasonable
- [ ] Memory usage is stable
- [ ] No lag or input delay

## Configuration Testing

To test different configurations, modify `TouchpadConfig` in C++:

```cpp
// Example: Test faster scroll
config.scrollScale = 60.0f;  // Increase from default 40

// Example: Test faster decay
config.decayConstantMs = 200.0f;  // Decrease from default 300

// Example: Disable inertia
config.inertiaEnabled = false;

// Example: Increase deadzone
config.deadzone = 0.05f;  // Increase from default 0.01
```

## Debugging

### Console Logging

Both test pages log useful debug information:

```javascript
// In browser console
console.log('WheelEvent:', event.deltaY);
console.log('Gamepad axes:', gamepad.axes);
console.log('Gamepad buttons:', gamepad.buttons);
```

### Performance Monitoring

Use Chrome DevTools Performance panel to:
- Monitor frame rate
- Check for event throttling
- Measure input lag
- Profile CPU usage

## Known Limitations

1. **DOM Event Synthesis**: Currently a placeholder (TODO in `touchpad_input_processor.cpp`)
   - WheelEvent is created but not yet dispatched to DOM
   - Requires hit-testing and event dispatch integration

2. **Cursor Mode**: Not yet implemented (feature flag disabled)

3. **Double-tap/Long-press**: Reserved for future implementation

## See Also

- [XR Touchpad Input Documentation](../../docs/internals/XR_TOUCHPAD_INPUT.md)
- [DOM Scrolling Performance](../../docs/internals/DOM_SCROLLING_PERFORMANCE.md)
- [Input Sources](../../docs/manual/basic-concepts/input-sources.md)
