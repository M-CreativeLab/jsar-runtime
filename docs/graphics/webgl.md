# WebGL Implementation

JSAR Runtime provides a comprehensive implementation of the WebGL API, supporting both WebGL 1.0 and WebGL 2.0 specifications. This document covers the core WebGL context implementation, its features, and usage patterns.

## Table of Contents

- [Overview](#overview)
- [WebGLContext](#webglcontext)
- [WebGL2Context](#webgl2context)
- [Context Attributes](#context-attributes)
- [WebGL State Management](#webgl-state-management)
- [Command Buffer System](#command-buffer-system)
- [XR Compatibility](#xr-compatibility)
- [Examples](#examples)

## Overview

The WebGL implementation in JSAR Runtime is designed to provide a complete and efficient rendering pipeline for 3D graphics. It supports all standard WebGL features and is optimized for spatial computing applications.

## WebGLContext

The `WebGLContext` class is the core of the WebGL implementation, providing methods for creating and managing WebGL resources and rendering operations.

### Key Features

- Complete implementation of WebGL 1.0 specification
- Efficient command buffer system for rendering
- Support for various rendering backends
- XR compatibility for immersive applications

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `id` | `uint8_t` | Unique identifier for the context |
| `contextAttributes` | `ContextAttributes` | Configuration options for the context |
| `maxCombinedTextureImageUnits` | `int` | Maximum number of texture image units that can be used |
| `maxCubeMapTextureSize` | `int` | Maximum cube map texture size |
| `maxFragmentUniformVectors` | `int` | Maximum number of uniform vectors in fragment shaders |
| `maxRenderbufferSize` | `int` | Maximum renderbuffer size |
| `maxTextureImageUnits` | `int` | Maximum number of texture image units |
| `maxTextureSize` | `int` | Maximum texture size |
| `maxVaryingVectors` | `int` | Maximum number of varying vectors |
| `maxVertexAttribs` | `int` | Maximum number of vertex attributes |
| `maxVertexTextureImageUnits` | `int` | Maximum number of vertex texture image units |
| `maxVertexUniformVectors` | `int` | Maximum number of uniform vectors in vertex shaders |
| `vendor` | `std::string` | Vendor string |
| `version` | `std::string` | Version string |
| `renderer` | `std::string` | Renderer string |

### Methods

#### Resource Creation

```cpp
std::shared_ptr<WebGLBuffer> createBuffer();
std::shared_ptr<WebGLFramebuffer> createFramebuffer();
std::shared_ptr<WebGLProgram> createProgram();
std::shared_ptr<WebGLRenderbuffer> createRenderbuffer();
std::shared_ptr<WebGLShader> createShader(int type);
std::shared_ptr<WebGLTexture> createTexture();
```

#### Resource Binding

```cpp
void bindBuffer(WebGLBufferBindingTarget target, std::shared_ptr<WebGLBuffer> buffer);
void bindFramebuffer(WebGLFramebufferBindingTarget target, std::shared_ptr<WebGLFramebuffer> framebuffer);
void bindRenderbuffer(WebGLRenderbufferBindingTarget target, std::shared_ptr<WebGLRenderbuffer> renderbuffer);
void bindTexture(WebGLTextureBindingTarget target, std::shared_ptr<WebGLTexture> texture);
```

#### Shader and Program Operations

```cpp
void compileShader(std::shared_ptr<WebGLShader> shader);
void shaderSource(std::shared_ptr<WebGLShader> shader, const std::string &source);
void linkProgram(std::shared_ptr<WebGLProgram> program);
void useProgram(std::shared_ptr<WebGLProgram> program);
void attachShader(std::shared_ptr<WebGLProgram> program, std::shared_ptr<WebGLShader> shader);
void detachShader(std::shared_ptr<WebGLProgram> program, std::shared_ptr<WebGLShader> shader);
```

#### Uniform and Attribute Operations

```cpp
std::optional<int> getAttribLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
std::optional<WebGLUniformLocation> getUniformLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
void uniform1f(WebGLUniformLocation location, float v0);
void uniform1fv(WebGLUniformLocation location, const std::vector<float> value);
// ... and many more uniform methods
```

#### Drawing Operations

```cpp
void drawArrays(WebGLDrawMode mode, int first, int count);
void drawElements(WebGLDrawMode mode, int count, int type, int offset);
```

#### State Management

```cpp
void enable(int cap);
void disable(int cap);
bool getParameter(WebGLBooleanParameterName pname);
float getParameter(WebGLFloatParameterName pname);
// ... and more parameter getters
```

## WebGL2Context

The `WebGL2Context` class extends `WebGLContext` to provide WebGL 2.0 functionality.

### Additional Features

- Complete implementation of WebGL 2.0 specification
- Support for 3D textures and instanced rendering
- Enhanced buffer and texture operations
- Transform feedback and uniform buffer objects

### Additional Properties

| Property | Type | Description |
|----------|------|-------------|
| `max3DTextureSize` | `int` | Maximum 3D texture size |
| `maxArrayTextureLayers` | `int` | Maximum array texture layers |
| `maxColorAttachments` | `int` | Maximum color attachments |
| `maxCombinedUniformBlocks` | `int` | Maximum combined uniform blocks |
| `maxDrawBuffers` | `int` | Maximum draw buffers |
| `maxElementsIndices` | `int` | Maximum elements indices |
| `maxElementsVertices` | `int` | Maximum elements vertices |
| `maxFragmentInputComponents` | `int` | Maximum fragment input components |
| `maxFragmentUniformBlocks` | `int` | Maximum fragment uniform blocks |
| `maxFragmentUniformComponents` | `int` | Maximum fragment uniform components |
| `maxProgramTexelOffset` | `int` | Maximum program texel offset |
| `maxSamples` | `int` | Maximum samples |

### Additional Methods

#### Vertex Array Objects

```cpp
std::shared_ptr<WebGLVertexArray> createVertexArray();
void bindVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray);
void deleteVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray);
bool isVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray);
```

#### 3D Texture Operations

```cpp
void texImage3D(WebGLTexture3DTarget target, int level, int internalformat, size_t width, size_t height, size_t depth, int border, WebGLTextureFormat format, WebGLPixelType type, unsigned char *pixels);
void texStorage3D(WebGLTexture3DTarget target, int levels, int internalformat, size_t width, size_t height, size_t depth);
void texSubImage3D(WebGLTexture3DTarget target, int level, int xoffset, int yoffset, int zoffset, size_t width, size_t height, size_t depth, WebGLTextureFormat format, WebGLPixelType type, unsigned char *pixels);
```

#### Instanced Rendering

```cpp
void drawArraysInstanced(WebGLDrawMode mode, int first, int count, int instanceCount);
void drawElementsInstanced(WebGLDrawMode mode, int count, int type, int offset, int instanceCount);
void vertexAttribDivisor(uint32_t index, uint32_t divisor);
```

## Context Attributes

The `ContextAttributes` class defines the configuration options for WebGL contexts.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `alpha` | `bool` | Whether the context has an alpha channel |
| `depth` | `bool` | Whether the context has a depth buffer |
| `stencil` | `bool` | Whether the context has a stencil buffer |
| `antialias` | `bool` | Whether the context has antialiasing |
| `premultipliedAlpha` | `bool` | Whether the context has premultiplied alpha |
| `preserveDrawingBuffer` | `bool` | Whether the drawing buffer is preserved |
| `powerPreference` | `std::string` | Power preference for the context |
| `failIfMajorPerformanceCaveat` | `bool` | Whether to fail if there's a major performance caveat |
| `desynchronized` | `bool` | Whether the context is desynchronized |
| `xrCompatible` | `bool` | Whether the context is XR compatible |

## WebGL State Management

The `WebGLState` class manages the state of the WebGL context, tracking bound objects and current settings.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `program` | `std::shared_ptr<WebGLProgram>` | Currently bound program |
| `arrayBuffer` | `std::shared_ptr<WebGLBuffer>` | Currently bound array buffer |
| `elementArrayBuffer` | `std::shared_ptr<WebGLBuffer>` | Currently bound element array buffer |
| `framebuffer` | `std::shared_ptr<WebGLFramebuffer>` | Currently bound framebuffer |
| `renderbuffer` | `std::shared_ptr<WebGLRenderbuffer>` | Currently bound renderbuffer |
| `texture2D` | `std::shared_ptr<WebGLTexture>` | Currently bound 2D texture |
| `textureCubeMap` | `std::shared_ptr<WebGLTexture>` | Currently bound cube map texture |

## Command Buffer System

JSAR Runtime uses a command buffer system for efficient rendering. Commands are batched and sent to the rendering backend, reducing overhead and improving performance.

### Command Buffer Flow

1. WebGL method is called
2. Command is added to the command buffer
3. Command buffer is sent to the rendering backend
4. Rendering backend executes the commands
5. Results are returned to the WebGL context

## XR Compatibility

WebGL contexts in JSAR Runtime can be made XR compatible, allowing them to be used with WebXR applications.

### Making a Context XR Compatible

```cpp
bool makeXRCompatible();
```

### Connecting to a WebXR Session

```cpp
void connectXRSession(std::shared_ptr<client_xr::XRSession> session);
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
```

### Creating and Using a Shader Program

```javascript
// Create shaders
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, vertexShaderSource);
gl.compileShader(vertexShader);

const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
gl.shaderSource(fragmentShader, fragmentShaderSource);
gl.compileShader(fragmentShader);

// Create program
const program = gl.createProgram();
gl.attachShader(program, vertexShader);
gl.attachShader(program, fragmentShader);
gl.linkProgram(program);

// Use program
gl.useProgram(program);
```

### XR-Compatible WebGL Context

```javascript
// Create an XR-compatible WebGL context
const gl = canvas.getContext('webgl', { xrCompatible: true });

// Set up WebXR session
const session = await navigator.xr.requestSession('immersive-vr', {
  requiredFeatures: ['local-floor'],
});

// Use the WebGL context with the XR session
const glLayer = new XRWebGLLayer(session, gl);
session.updateRenderState({ baseLayer: glLayer });
```
