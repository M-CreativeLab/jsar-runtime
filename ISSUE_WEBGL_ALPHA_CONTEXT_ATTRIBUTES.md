# WebGL Context Alpha Attribute Not Properly Passed Through Canvas.getContext()

## Issue Description

When calling `canvas.getContext('webgl2', { alpha: false })` in JSAR Runtime, the `alpha: false` attribute is not properly passed through the entire call chain, resulting in WebGL contexts always being created with `alpha: true` (default value). This causes rendering inconsistencies compared to standard browsers.

## Root Cause Analysis

### Problem Location
The issue exists in the Canvas-to-WebGL context creation pipeline:

1. **JavaScript Binding Layer**: `HTMLCanvasElement` inherits `CanvasWrap::GetContext` method
2. **Canvas Implementation Layer**: `CanvasBase::getContext` only supports 2D contexts, ignoring WebGL context types
3. **WebGL Context Creation**: `TrClientContextPerProcess::createHostWebGLContext()` uses hardcoded `ContextAttributes`

### Specific Issues

1. **Canvas Layer Gap**: In `src/client/canvas/canvas-inl.hpp`, `CanvasBase::getContext` only handles `RenderingContext2D`, completely skipping WebGL context processing.

2. **Context Attributes Loss**: Even though `src/bindings/canvas/canvas-inl.hpp` contains code to parse `contextAttributes` (including `alpha` property), it's never reached due to the Canvas layer limitation.

3. **Hardcoded Context Creation**: In `src/client/per_process.cpp`, `createHostWebGLContext()` creates contexts with hardcoded attributes:
   ```cpp
   client_graphics::ContextAttributes contextAttrs;
   contextAttrs.xrCompatible = true;  // Only this is set, alpha defaults to true
   ```

## Impact

- **Test Failures**: `clearbufferfv-with-alpha-false.html` test fails because it expects opaque framebuffer but gets transparent one
- **Rendering Inconsistency**: WebGL applications expecting `alpha: false` behavior will render differently than in standard browsers
- **API Compliance**: JSAR Runtime doesn't fully comply with WebGL specification for context creation

## Affected Files

- `src/client/canvas/canvas-inl.hpp` - Canvas implementation layer
- `src/bindings/canvas/canvas-inl.hpp` - Canvas binding layer  
- `src/client/per_process.cpp` - WebGL context creation
- `src/bindings/webgl/rendering_context-inl.hpp` - WebGL context constructor

## Expected Behavior

When `canvas.getContext('webgl2', { alpha: false })` is called:
1. The `alpha: false` attribute should be parsed and preserved
2. The WebGL context should be created with `alpha: false`
3. `getContextAttributes()` should return `{ alpha: false }`
4. Rendering behavior should match standard browsers

## Current Behavior

1. `alpha: false` attribute is ignored
2. WebGL context is created with default `alpha: true`
3. `getContextAttributes()` returns `{ alpha: true }`
4. Rendering differs from standard browsers

## Proposed Solution

Two potential approaches:

### Option 1: Extend Canvas Layer WebGL Support
- Modify `CanvasBase::getContext` to handle WebGL context types
- Pass `contextAttributes` through the Canvas layer
- Update WebGL context creation to accept custom attributes

### Option 2: Direct WebGL Context Creation Path
- Create a separate path for WebGL context creation that bypasses Canvas layer
- Ensure `contextAttributes` are properly passed to `createHostWebGLContext`
- Maintain backward compatibility

## Test Case

```javascript
const canvas = document.createElement('canvas');
const gl = canvas.getContext('webgl2', { alpha: false });
const attrs = gl.getContextAttributes();
console.log(attrs.alpha); // Should be false, currently true
```

## Priority

**High** - This affects WebGL API compliance and causes test failures in the WebGL conformance suite.

## Labels

- `bug`
- `webgl`
- `canvas`
- `api-compliance`
- `rendering`