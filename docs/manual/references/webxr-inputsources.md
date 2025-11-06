# WebXR input sources

This document describes the WebXR input sources implementation in JSAR Runtime.

## Supported Input Sources

JSAR implements the following WebXR input sources for XR devices:

- **`main-controller`**: The device's main handheld controller with ray-based pointing
- **`hand-left`**: Left hand tracking input source
- **`hand-right`**: Right hand tracking input source
- **`eye`**: Gaze-based input using eye tracking
- **`screen`**: Touch-based input on screen devices

## Input Source Features

### Gamepad Support

Input sources that have axes or buttons expose a `gamepad` property following the [WebXR Gamepads Module](https://www.w3.org/TR/webxr-gamepads-module-1/) specification.

```javascript
session.requestAnimationFrame((time, frame) => {
  for (const inputSource of frame.session.inputSources) {
    const gamepad = inputSource.gamepad;
    if (gamepad) {
      console.log('Axes:', gamepad.axes);
      console.log('Buttons:', gamepad.buttons);
      console.log('Mapping:', gamepad.mapping); // "xr-standard"
    }
  }
});
```

### Touchpad Input

Controllers with touchpads expose the touchpad state via the Gamepad API:

- **`gamepad.axes[0]`**: Horizontal position (-1 to 1)
- **`gamepad.axes[1]`**: Vertical position (-1 to 1)
- **`gamepad.buttons[0].touched`**: Finger contact with touchpad
- **`gamepad.buttons[0].pressed`**: Physical click or touch
- **`gamepad.buttons[0].value`**: 0.0 or 1.0 (no pressure sensing)

#### Touchpad Scroll Mode

By default, touchpad input is configured in **scroll mode**, which synthesizes `WheelEvent` for DOM scrolling:

- Slide finger on touchpad → WheelEvent with deltaX/deltaY
- Kinetic inertia enabled by default (exponential decay, τ=300ms)
- Event throttling at 16ms intervals (~60fps)
- Automatic deadzone filtering (0.01 threshold)

See [XR Touchpad Input Documentation](../../internals/XR_TOUCHPAD_INPUT.md) for details.

### Hand Tracking

Input sources with `targetRayMode: "tracked-pointer"` and hand tracking support provide the `hand` property:

```javascript
const inputSource = session.inputSources[0];
if (inputSource.hand) {
  for (const [jointName, jointSpace] of inputSource.hand) {
    const pose = frame.getJointPose(jointSpace, referenceSpace);
    if (pose) {
      console.log(`${jointName}:`, pose.transform);
    }
  }
}
```

## Target Ray Modes

- **`gaze`**: Input directed by user's gaze (eye tracking)
- **`tracked-pointer`**: Physical controller or hand with 6DOF tracking
- **`screen`**: Screen-based touch input
- **`transient-pointer`**: Temporary pointer (e.g., hand pinch gesture)

## Spaces

Each input source provides spatial reference frames:

- **`targetRaySpace`**: Origin and direction of the input ray
- **`gripSpace`**: Position for rendering a virtual object in the user's hand (tracked-pointer only)

## Events

### Select Events

Triggered by primary action (e.g., trigger button, touchpad click):

```javascript
inputSource.addEventListener('select', (event) => {
  console.log('Select action performed');
});

inputSource.addEventListener('selectstart', (event) => {
  console.log('Select action started');
});

inputSource.addEventListener('selectend', (event) => {
  console.log('Select action ended');
});
```

### Squeeze Events

Triggered by squeeze action (e.g., grip button):

```javascript
inputSource.addEventListener('squeeze', (event) => {
  console.log('Squeeze action performed');
});
```

### Input Sources Change

Notified when input sources are added or removed:

```javascript
session.addEventListener('inputsourceschange', (event) => {
  console.log('Added:', event.added);
  console.log('Removed:', event.removed);
});
```

## Examples

### Basic Input Handling

```javascript
navigator.xr.requestSession('immersive-vr').then(session => {
  session.addEventListener('inputsourceschange', (event) => {
    console.log('Input sources changed');
  });
  
  function onXRFrame(time, frame) {
    session.requestAnimationFrame(onXRFrame);
    
    for (const inputSource of session.inputSources) {
      // Get input source spaces
      const gripSpace = inputSource.gripSpace;
      const targetRaySpace = inputSource.targetRaySpace;
      
      // Get poses
      if (gripSpace) {
        const gripPose = frame.getPose(gripSpace, referenceSpace);
        // Render controller model at gripPose
      }
      
      const rayPose = frame.getPose(targetRaySpace, referenceSpace);
      // Render ray at rayPose
    }
  }
  
  session.requestAnimationFrame(onXRFrame);
});
```

### Touchpad Scrolling

```javascript
// Touchpad automatically emits WheelEvent for scrolling
document.addEventListener('wheel', (event) => {
  console.log('Scroll delta:', event.deltaY);
  // DOM scrolling handled automatically
});
```

### Gamepad State Reading

```javascript
function onXRFrame(time, frame) {
  for (const inputSource of session.inputSources) {
    const gamepad = inputSource.gamepad;
    if (gamepad) {
      // Check touchpad position
      const x = gamepad.axes[0];
      const y = gamepad.axes[1];
      
      // Check if touchpad is touched
      if (gamepad.buttons[0].touched) {
        console.log(`Touchpad touched at (${x}, ${y})`);
      }
      
      // Check if touchpad is clicked
      if (gamepad.buttons[0].pressed) {
        console.log('Touchpad clicked');
      }
    }
  }
  session.requestAnimationFrame(onXRFrame);
}
```

## See Also

- [Input Sources Concept](../basic-concepts/input-sources.md)
- [XR Touchpad Input System](../../internals/XR_TOUCHPAD_INPUT.md)
- [WebXR Device API Specification](https://www.w3.org/TR/webxr/)
- [WebXR Gamepads Module](https://www.w3.org/TR/webxr-gamepads-module-1/)
- [Test Fixtures](../../../fixtures/xr-touchpad/README.md)
