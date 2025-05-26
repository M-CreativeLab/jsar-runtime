# Graphics Integration and Bindings

JSAR Runtime provides a comprehensive integration layer for its graphics components, allowing JavaScript code to interact with the underlying C++ implementations. This document covers the JavaScript bindings for WebGL and Canvas, as well as integration with other components.

## Table of Contents

- [Overview](#overview)
- [WebGL Bindings](#webgl-bindings)
- [Canvas Bindings](#canvas-bindings)
- [Integration with Node.js](#integration-with-nodejs)
- [Integration with XR](#integration-with-xr)
- [Examples](#examples)

## Overview

JSAR Runtime uses the Node.js N-API to expose its graphics components to JavaScript. This allows JavaScript code to interact with the underlying C++ implementations in a seamless manner. The bindings are designed to be compatible with standard web APIs, making it easy to port existing web applications to JSAR Runtime.

## WebGL Bindings

The WebGL bindings in JSAR Runtime provide JavaScript access to the WebGL implementation. They follow the WebGL specifications and are designed to be compatible with standard WebGL code.

### WebGL Context Binding

The `WebGLRenderingContext` and `WebGL2RenderingContext` classes provide JavaScript bindings for the WebGL contexts. They expose all the standard WebGL methods and properties, allowing JavaScript code to interact with the WebGL implementation.

```cpp
class WebGLRenderingContextWrap : public Napi::ObjectWrap<WebGLRenderingContextWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, std::shared_ptr<client_graphics::WebGLContext> context);
  WebGLRenderingContextWrap(const Napi::CallbackInfo &info);
  ~WebGLRenderingContextWrap();

  // WebGL methods
  Napi::Value CreateBuffer(const Napi::CallbackInfo &info);
  Napi::Value CreateFramebuffer(const Napi::CallbackInfo &info);
  Napi::Value CreateProgram(const Napi::CallbackInfo &info);
  Napi::Value CreateRenderbuffer(const Napi::CallbackInfo &info);
  Napi::Value CreateShader(const Napi::CallbackInfo &info);
  Napi::Value CreateTexture(const Napi::CallbackInfo &info);
  // ... and many more methods

private:
  std::shared_ptr<client_graphics::WebGLContext> context_;
};
```

### WebGL Object Bindings

The WebGL object bindings provide JavaScript access to WebGL objects such as buffers, textures, programs, and shaders. They wrap the C++ implementations and expose them to JavaScript.

```cpp
class WebGLBufferWrap : public Napi::ObjectWrap<WebGLBufferWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, std::shared_ptr<client_graphics::WebGLBuffer> buffer);
  WebGLBufferWrap(const Napi::CallbackInfo &info);
  ~WebGLBufferWrap();

  std::shared_ptr<client_graphics::WebGLBuffer> GetBuffer() const;

private:
  std::shared_ptr<client_graphics::WebGLBuffer> buffer_;
};
```

### WebGL Constants

The WebGL constants are exposed to JavaScript as properties of the WebGL context objects. They follow the standard WebGL constants and are used for various WebGL operations.

```javascript
// WebGL constants
const gl = canvas.getContext('webgl');
gl.ARRAY_BUFFER;          // 0x8892
gl.ELEMENT_ARRAY_BUFFER;  // 0x8893
gl.STATIC_DRAW;           // 0x88E4
gl.DYNAMIC_DRAW;          // 0x88E8
// ... and many more constants
```

## Canvas Bindings

The Canvas bindings in JSAR Runtime provide JavaScript access to the Canvas implementation. They follow the Canvas API specifications and are designed to be compatible with standard Canvas code.

### Canvas Element Binding

The `HTMLCanvasElement` class provides JavaScript bindings for the Canvas element. It exposes methods for getting the rendering context and converting the canvas to a data URL.

```cpp
class CanvasWrap : public Napi::ObjectWrap<CanvasWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, std::shared_ptr<client_canvas::Canvas> canvas);
  CanvasWrap(const Napi::CallbackInfo &info);
  ~CanvasWrap();

  Napi::Value GetContext(const Napi::CallbackInfo &info);
  Napi::Value ToDataURL(const Napi::CallbackInfo &info);
  Napi::Value GetWidth(const Napi::CallbackInfo &info);
  Napi::Value SetWidth(const Napi::CallbackInfo &info);
  Napi::Value GetHeight(const Napi::CallbackInfo &info);
  Napi::Value SetHeight(const Napi::CallbackInfo &info);

  std::shared_ptr<client_canvas::Canvas> GetCanvas() const;

private:
  std::shared_ptr<client_canvas::Canvas> canvas_;
};
```

### OffscreenCanvas Binding

The `OffscreenCanvas` class provides JavaScript bindings for the OffscreenCanvas implementation. It exposes methods for getting the rendering context and committing the canvas content to the main thread.

```cpp
class OffscreenCanvasWrap : public Napi::ObjectWrap<OffscreenCanvasWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, std::shared_ptr<client_canvas::OffscreenCanvas> canvas);
  OffscreenCanvasWrap(const Napi::CallbackInfo &info);
  ~OffscreenCanvasWrap();

  Napi::Value GetContext(const Napi::CallbackInfo &info);
  Napi::Value Commit(const Napi::CallbackInfo &info);
  Napi::Value GetWidth(const Napi::CallbackInfo &info);
  Napi::Value SetWidth(const Napi::CallbackInfo &info);
  Napi::Value GetHeight(const Napi::CallbackInfo &info);
  Napi::Value SetHeight(const Napi::CallbackInfo &info);

  std::shared_ptr<client_canvas::OffscreenCanvas> GetCanvas() const;

private:
  std::shared_ptr<client_canvas::OffscreenCanvas> canvas_;
};
```

### Canvas Rendering Context 2D Binding

The `CanvasRenderingContext2D` class provides JavaScript bindings for the 2D rendering context. It exposes all the standard Canvas 2D methods and properties, allowing JavaScript code to interact with the Canvas implementation.

```cpp
class CanvasRenderingContext2DWrap : public Napi::ObjectWrap<CanvasRenderingContext2DWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env env, std::shared_ptr<client_canvas::CanvasRenderingContext2D> context);
  CanvasRenderingContext2DWrap(const Napi::CallbackInfo &info);
  ~CanvasRenderingContext2DWrap();

  // Canvas 2D methods
  Napi::Value Save(const Napi::CallbackInfo &info);
  Napi::Value Restore(const Napi::CallbackInfo &info);
  Napi::Value Scale(const Napi::CallbackInfo &info);
  Napi::Value Rotate(const Napi::CallbackInfo &info);
  Napi::Value Translate(const Napi::CallbackInfo &info);
  Napi::Value Transform(const Napi::CallbackInfo &info);
  Napi::Value SetTransform(const Napi::CallbackInfo &info);
  Napi::Value ResetTransform(const Napi::CallbackInfo &info);
  Napi::Value BeginPath(const Napi::CallbackInfo &info);
  Napi::Value ClosePath(const Napi::CallbackInfo &info);
  Napi::Value MoveTo(const Napi::CallbackInfo &info);
  Napi::Value LineTo(const Napi::CallbackInfo &info);
  Napi::Value BezierCurveTo(const Napi::CallbackInfo &info);
  Napi::Value QuadraticCurveTo(const Napi::CallbackInfo &info);
  Napi::Value Arc(const Napi::CallbackInfo &info);
  Napi::Value ArcTo(const Napi::CallbackInfo &info);
  Napi::Value Rect(const Napi::CallbackInfo &info);
  Napi::Value Fill(const Napi::CallbackInfo &info);
  Napi::Value Stroke(const Napi::CallbackInfo &info);
  Napi::Value Clip(const Napi::CallbackInfo &info);
  Napi::Value FillRect(const Napi::CallbackInfo &info);
  Napi::Value StrokeRect(const Napi::CallbackInfo &info);
  Napi::Value ClearRect(const Napi::CallbackInfo &info);
  Napi::Value FillText(const Napi::CallbackInfo &info);
  Napi::Value StrokeText(const Napi::CallbackInfo &info);
  Napi::Value MeasureText(const Napi::CallbackInfo &info);
  Napi::Value DrawImage(const Napi::CallbackInfo &info);
  Napi::Value CreateImageData(const Napi::CallbackInfo &info);
  Napi::Value GetImageData(const Napi::CallbackInfo &info);
  Napi::Value PutImageData(const Napi::CallbackInfo &info);
  // ... and many more methods

  // Canvas 2D properties
  Napi::Value GetFillStyle(const Napi::CallbackInfo &info);
  Napi::Value SetFillStyle(const Napi::CallbackInfo &info);
  Napi::Value GetStrokeStyle(const Napi::CallbackInfo &info);
  Napi::Value SetStrokeStyle(const Napi::CallbackInfo &info);
  Napi::Value GetLineWidth(const Napi::CallbackInfo &info);
  Napi::Value SetLineWidth(const Napi::CallbackInfo &info);
  Napi::Value GetLineCap(const Napi::CallbackInfo &info);
  Napi::Value SetLineCap(const Napi::CallbackInfo &info);
  Napi::Value GetLineJoin(const Napi::CallbackInfo &info);
  Napi::Value SetLineJoin(const Napi::CallbackInfo &info);
  Napi::Value GetMiterLimit(const Napi::CallbackInfo &info);
  Napi::Value SetMiterLimit(const Napi::CallbackInfo &info);
  Napi::Value GetGlobalAlpha(const Napi::CallbackInfo &info);
  Napi::Value SetGlobalAlpha(const Napi::CallbackInfo &info);
  Napi::Value GetGlobalCompositeOperation(const Napi::CallbackInfo &info);
  Napi::Value SetGlobalCompositeOperation(const Napi::CallbackInfo &info);
  Napi::Value GetFont(const Napi::CallbackInfo &info);
  Napi::Value SetFont(const Napi::CallbackInfo &info);
  Napi::Value GetTextAlign(const Napi::CallbackInfo &info);
  Napi::Value SetTextAlign(const Napi::CallbackInfo &info);
  Napi::Value GetTextBaseline(const Napi::CallbackInfo &info);
  Napi::Value SetTextBaseline(const Napi::CallbackInfo &info);
  Napi::Value GetDirection(const Napi::CallbackInfo &info);
  Napi::Value SetDirection(const Napi::CallbackInfo &info);
  Napi::Value GetImageSmoothingEnabled(const Napi::CallbackInfo &info);
  Napi::Value SetImageSmoothingEnabled(const Napi::CallbackInfo &info);
  Napi::Value GetImageSmoothingQuality(const Napi::CallbackInfo &info);
  Napi::Value SetImageSmoothingQuality(const Napi::CallbackInfo &info);
  // ... and many more properties

private:
  std::shared_ptr<client_canvas::CanvasRenderingContext2D> context_;
};
```

## Integration with Node.js

JSAR Runtime integrates with Node.js to provide a JavaScript runtime environment for web applications. The graphics components are exposed to JavaScript through the Node.js N-API, allowing JavaScript code to interact with the underlying C++ implementations.

### Module Initialization

The graphics components are initialized as Node.js modules, which are loaded when the JavaScript runtime starts. The modules expose the WebGL and Canvas APIs to JavaScript code.

```cpp
Napi::Object InitWebGL(Napi::Env env, Napi::Object exports)
{
  WebGLRenderingContextWrap::Init(env, exports);
  WebGL2RenderingContextWrap::Init(env, exports);
  WebGLBufferWrap::Init(env, exports);
  WebGLFramebufferWrap::Init(env, exports);
  WebGLProgramWrap::Init(env, exports);
  WebGLRenderbufferWrap::Init(env, exports);
  WebGLShaderWrap::Init(env, exports);
  WebGLTextureWrap::Init(env, exports);
  WebGLUniformLocationWrap::Init(env, exports);
  WebGLVertexArrayWrap::Init(env, exports);
  return exports;
}

Napi::Object InitCanvas(Napi::Env env, Napi::Object exports)
{
  CanvasWrap::Init(env, exports);
  OffscreenCanvasWrap::Init(env, exports);
  CanvasRenderingContext2DWrap::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, [](Napi::Env env, Napi::Object exports) {
  InitWebGL(env, exports);
  InitCanvas(env, exports);
  return exports;
})
```

### JavaScript API

The JavaScript API for the graphics components follows the standard web APIs, making it easy to port existing web applications to JSAR Runtime.

```javascript
// Get a WebGL context
const canvas = document.createElement('canvas');
const gl = canvas.getContext('webgl');

// Use WebGL
gl.clearColor(0.0, 0.0, 0.0, 1.0);
gl.clear(gl.COLOR_BUFFER_BIT);

// Get a Canvas 2D context
const ctx = canvas.getContext('2d');

// Use Canvas 2D
ctx.fillStyle = 'red';
ctx.fillRect(10, 10, 100, 100);
```

## Integration with XR

JSAR Runtime integrates with XR to provide immersive experiences. The graphics components are designed to work with XR, allowing WebGL contexts to be used with WebXR sessions.

### XR-Compatible WebGL Contexts

WebGL contexts in JSAR Runtime can be made XR-compatible, allowing them to be used with WebXR sessions. This is done by setting the `xrCompatible` attribute when creating the context.

```javascript
// Create an XR-compatible WebGL context
const gl = canvas.getContext('webgl', { xrCompatible: true });
```

### WebXR Integration

The WebXR API in JSAR Runtime integrates with the graphics components to provide immersive experiences. WebGL contexts can be used with WebXR sessions to render 3D content in VR or AR.

```javascript
// Set up WebXR session
const session = await navigator.xr.requestSession('immersive-vr', {
  requiredFeatures: ['local-floor'],
});

// Use the WebGL context with the XR session
const glLayer = new XRWebGLLayer(session, gl);
session.updateRenderState({ baseLayer: glLayer });

// Set up XR frame loop
session.requestAnimationFrame((time, frame) => {
  // Get viewer pose
  const pose = frame.getViewerPose(referenceSpace);
  if (pose) {
    // Render each view
    for (const view of pose.views) {
      const viewport = glLayer.getViewport(view);
      gl.viewport(viewport.x, viewport.y, viewport.width, viewport.height);
      
      // Render scene
      // ...
    }
  }
});
```

## Examples

### Basic WebGL Setup

```javascript
// Get a WebGL context
const canvas = document.createElement('canvas');
const gl = canvas.getContext('webgl');

// Set up viewport
gl.viewport(0, 0, canvas.width, canvas.height);

// Clear the canvas
gl.clearColor(0.0, 0.0, 0.0, 1.0);
gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

// Create shaders
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, `
  attribute vec4 a_position;
  
  void main() {
    gl_Position = a_position;
  }
`);
gl.compileShader(vertexShader);

const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
gl.shaderSource(fragmentShader, `
  precision mediump float;
  
  void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
`);
gl.compileShader(fragmentShader);

// Create program
const program = gl.createProgram();
gl.attachShader(program, vertexShader);
gl.attachShader(program, fragmentShader);
gl.linkProgram(program);
gl.useProgram(program);

// Create buffer
const positionBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
  -1.0, -1.0,
   1.0, -1.0,
   0.0,  1.0
]), gl.STATIC_DRAW);

// Set up attribute
const positionAttributeLocation = gl.getAttribLocation(program, 'a_position');
gl.enableVertexAttribArray(positionAttributeLocation);
gl.vertexAttribPointer(positionAttributeLocation, 2, gl.FLOAT, false, 0, 0);

// Draw
gl.drawArrays(gl.TRIANGLES, 0, 3);
```

### Basic Canvas 2D Setup

```javascript
// Get a Canvas 2D context
const canvas = document.createElement('canvas');
const ctx = canvas.getContext('2d');

// Draw a rectangle
ctx.fillStyle = 'red';
ctx.fillRect(10, 10, 100, 100);

// Draw a circle
ctx.beginPath();
ctx.arc(200, 60, 50, 0, Math.PI * 2);
ctx.fillStyle = 'blue';
ctx.fill();

// Draw text
ctx.font = '24px Arial';
ctx.fillStyle = 'black';
ctx.fillText('Hello, world!', 10, 150);
```

### WebXR Setup

```javascript
// Create an XR-compatible WebGL context
const canvas = document.createElement('canvas');
const gl = canvas.getContext('webgl', { xrCompatible: true });

// Set up WebXR session
const session = await navigator.xr.requestSession('immersive-vr', {
  requiredFeatures: ['local-floor'],
});

// Use the WebGL context with the XR session
const glLayer = new XRWebGLLayer(session, gl);
session.updateRenderState({ baseLayer: glLayer });

// Create reference space
const referenceSpace = await session.requestReferenceSpace('local-floor');

// Set up XR frame loop
session.requestAnimationFrame(onXRFrame);

function onXRFrame(time, frame) {
  // Queue the next frame
  session.requestAnimationFrame(onXRFrame);

  // Get viewer pose
  const pose = frame.getViewerPose(referenceSpace);
  if (pose) {
    // Render each view
    for (const view of pose.views) {
      const viewport = glLayer.getViewport(view);
      gl.viewport(viewport.x, viewport.y, viewport.width, viewport.height);
      
      // Render scene
      // ...
    }
  }
}
```

### OffscreenCanvas Usage

```javascript
// Create an offscreen canvas
const offscreenCanvas = new OffscreenCanvas(300, 150);
const ctx = offscreenCanvas.getContext('2d');

// Draw on the offscreen canvas
ctx.fillStyle = 'green';
ctx.fillRect(10, 10, 100, 100);

// Commit the offscreen canvas to the main thread
offscreenCanvas.commit();
```
