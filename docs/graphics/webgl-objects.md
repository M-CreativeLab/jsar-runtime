# WebGL Objects

JSAR Runtime provides a comprehensive implementation of WebGL objects, including programs, shaders, buffers, textures, and more. This document covers the various WebGL object types, their properties, methods, and usage patterns.

## Table of Contents

- [Overview](#overview)
- [WebGLProgram](#webglprogram)
- [WebGLShader](#webglshader)
- [WebGLBuffer](#webglbuffer)
- [WebGLTexture](#webgltexture)
- [WebGLFramebuffer](#webglframebuffer)
- [WebGLRenderbuffer](#webglrenderbuffer)
- [WebGLVertexArray](#webglvertexarray)
- [WebGLUniformLocation](#webgluniformlocation)
- [Object Scope Management](#object-scope-management)
- [Examples](#examples)

## Overview

WebGL objects are the building blocks of WebGL applications. They represent various resources used in the rendering pipeline, such as shaders, buffers, textures, and more. JSAR Runtime provides a complete implementation of these objects, following the WebGL specifications.

## WebGLProgram

The `WebGLProgram` class represents a WebGL program object, which is a combination of vertex and fragment shaders.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `linkStatus` | `bool` | Whether the program has been successfully linked |
| `deleteStatus` | `bool` | Whether the program has been marked for deletion |
| `activeAttributes` | `std::vector<WebGLActiveInfo>` | Information about active attributes |
| `activeUniforms` | `std::vector<WebGLActiveInfo>` | Information about active uniforms |

### Methods

```cpp
void setLinkStatus(bool status);
bool getLinkStatus() const;
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
void setActiveAttributes(const std::vector<WebGLActiveInfo> &attributes);
const std::vector<WebGLActiveInfo> &getActiveAttributes() const;
void setActiveUniforms(const std::vector<WebGLActiveInfo> &uniforms);
const std::vector<WebGLActiveInfo> &getActiveUniforms() const;
void setAttribLocation(const std::string &name, int location);
std::optional<int> getAttribLocation(const std::string &name) const;
void setUniformLocation(const std::string &name, WebGLUniformLocation location);
std::optional<WebGLUniformLocation> getUniformLocation(const std::string &name) const;
void printProgramInfo() const;
```

### Usage

Programs are created using the `createProgram` method of the WebGL context, and shaders are attached to them using the `attachShader` method. After attaching shaders, the program is linked using the `linkProgram` method.

```javascript
const program = gl.createProgram();
gl.attachShader(program, vertexShader);
gl.attachShader(program, fragmentShader);
gl.linkProgram(program);

if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
  console.error('Program linking failed:', gl.getProgramInfoLog(program));
}
```

## WebGLShader

The `WebGLShader` class represents a WebGL shader object, which contains the source code for either a vertex or fragment shader.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `type` | `int` | The type of the shader (vertex or fragment) |
| `source` | `std::string` | The source code of the shader |
| `compileStatus` | `bool` | Whether the shader has been successfully compiled |
| `deleteStatus` | `bool` | Whether the shader has been marked for deletion |

### Methods

```cpp
void setType(int type);
int getType() const;
void setSource(const std::string &source);
const std::string &getSource() const;
void setCompileStatus(bool status);
bool getCompileStatus() const;
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
```

### Usage

Shaders are created using the `createShader` method of the WebGL context, and their source code is set using the `shaderSource` method. After setting the source, the shader is compiled using the `compileShader` method.

```javascript
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, vertexShaderSource);
gl.compileShader(vertexShader);

if (!gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS)) {
  console.error('Shader compilation failed:', gl.getShaderInfoLog(vertexShader));
}
```

## WebGLBuffer

The `WebGLBuffer` class represents a WebGL buffer object, which is used to store vertex data or element indices.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `size` | `size_t` | The size of the buffer in bytes |
| `usage` | `WebGLBufferUsage` | The usage pattern of the buffer |
| `deleteStatus` | `bool` | Whether the buffer has been marked for deletion |

### Methods

```cpp
void setSize(size_t size);
size_t getSize() const;
void setUsage(WebGLBufferUsage usage);
WebGLBufferUsage getUsage() const;
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
```

### Usage

Buffers are created using the `createBuffer` method of the WebGL context, and they are bound to a target using the `bindBuffer` method. Data is uploaded to the buffer using the `bufferData` method.

```javascript
const positionBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
```

## WebGLTexture

The `WebGLTexture` class represents a WebGL texture object, which is used to store image data for rendering.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `width` | `size_t` | The width of the texture |
| `height` | `size_t` | The height of the texture |
| `format` | `WebGLTextureFormat` | The format of the texture |
| `type` | `WebGLPixelType` | The pixel type of the texture |
| `deleteStatus` | `bool` | Whether the texture has been marked for deletion |

### Methods

```cpp
void setWidth(size_t width);
size_t getWidth() const;
void setHeight(size_t height);
size_t getHeight() const;
void setFormat(WebGLTextureFormat format);
WebGLTextureFormat getFormat() const;
void setType(WebGLPixelType type);
WebGLPixelType getType() const;
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
```

### Usage

Textures are created using the `createTexture` method of the WebGL context, and they are bound to a target using the `bindTexture` method. Image data is uploaded to the texture using the `texImage2D` method.

```javascript
const texture = gl.createTexture();
gl.bindTexture(gl.TEXTURE_2D, texture);
gl.texImage2D(
  gl.TEXTURE_2D,
  0,                // level
  gl.RGBA,          // internalformat
  gl.RGBA,          // format
  gl.UNSIGNED_BYTE, // type
  image             // image data
);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
```

## WebGLFramebuffer

The `WebGLFramebuffer` class represents a WebGL framebuffer object, which is used for offscreen rendering.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `width` | `size_t` | The width of the framebuffer |
| `height` | `size_t` | The height of the framebuffer |
| `deleteStatus` | `bool` | Whether the framebuffer has been marked for deletion |
| `colorAttachments` | `std::map<WebGLFramebufferAttachment, std::shared_ptr<WebGLObject>>` | Color attachments |
| `depthAttachment` | `std::shared_ptr<WebGLObject>` | Depth attachment |
| `stencilAttachment` | `std::shared_ptr<WebGLObject>` | Stencil attachment |
| `depthStencilAttachment` | `std::shared_ptr<WebGLObject>` | Depth-stencil attachment |

### Methods

```cpp
void setWidth(size_t width);
size_t getWidth() const;
void setHeight(size_t height);
size_t getHeight() const;
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
void setColorAttachment(WebGLFramebufferAttachment attachment, std::shared_ptr<WebGLObject> object);
std::shared_ptr<WebGLObject> getColorAttachment(WebGLFramebufferAttachment attachment) const;
void setDepthAttachment(std::shared_ptr<WebGLObject> object);
std::shared_ptr<WebGLObject> getDepthAttachment() const;
void setStencilAttachment(std::shared_ptr<WebGLObject> object);
std::shared_ptr<WebGLObject> getStencilAttachment() const;
void setDepthStencilAttachment(std::shared_ptr<WebGLObject> object);
std::shared_ptr<WebGLObject> getDepthStencilAttachment() const;
```

### Usage

Framebuffers are created using the `createFramebuffer` method of the WebGL context, and they are bound to a target using the `bindFramebuffer` method. Textures or renderbuffers are attached to the framebuffer using the `framebufferTexture2D` or `framebufferRenderbuffer` methods.

```javascript
const framebuffer = gl.createFramebuffer();
gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

// Attach a texture to the framebuffer
const texture = gl.createTexture();
gl.bindTexture(gl.TEXTURE_2D, texture);
gl.texImage2D(
  gl.TEXTURE_2D,
  0,                // level
  gl.RGBA,          // internalformat
  width, height,    // dimensions
  0,                // border
  gl.RGBA,          // format
  gl.UNSIGNED_BYTE, // type
  null              // no data
);
gl.framebufferTexture2D(
  gl.FRAMEBUFFER,
  gl.COLOR_ATTACHMENT0,
  gl.TEXTURE_2D,
  texture,
  0                 // level
);
```

## WebGLRenderbuffer

The `WebGLRenderbuffer` class represents a WebGL renderbuffer object, which is used for offscreen rendering.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `width` | `size_t` | The width of the renderbuffer |
| `height` | `size_t` | The height of the renderbuffer |
| `internalFormat` | `int` | The internal format of the renderbuffer |
| `deleteStatus` | `bool` | Whether the renderbuffer has been marked for deletion |

### Methods

```cpp
void setWidth(size_t width);
size_t getWidth() const;
void setHeight(size_t height);
size_t getHeight() const;
void setInternalFormat(int format);
int getInternalFormat() const;
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
```

### Usage

Renderbuffers are created using the `createRenderbuffer` method of the WebGL context, and they are bound to a target using the `bindRenderbuffer` method. Storage is allocated for the renderbuffer using the `renderbufferStorage` method.

```javascript
const renderbuffer = gl.createRenderbuffer();
gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, width, height);
```

## WebGLVertexArray

The `WebGLVertexArray` class represents a WebGL vertex array object, which is used to store vertex attribute configurations.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `deleteStatus` | `bool` | Whether the vertex array has been marked for deletion |
| `elementArrayBuffer` | `std::shared_ptr<WebGLBuffer>` | The element array buffer bound to this vertex array |

### Methods

```cpp
void setDeleteStatus(bool status);
bool getDeleteStatus() const;
void setElementArrayBuffer(std::shared_ptr<WebGLBuffer> buffer);
std::shared_ptr<WebGLBuffer> getElementArrayBuffer() const;
```

### Usage

Vertex arrays are created using the `createVertexArray` method of the WebGL context, and they are bound using the `bindVertexArray` method. Vertex attribute configurations are stored in the vertex array.

```javascript
const vao = gl.createVertexArray();
gl.bindVertexArray(vao);

// Configure vertex attributes
gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
gl.vertexAttribPointer(
  positionAttributeLocation,
  3,        // size
  gl.FLOAT,  // type
  false,     // normalize
  0,         // stride
  0          // offset
);
gl.enableVertexAttribArray(positionAttributeLocation);
```

## WebGLUniformLocation

The `WebGLUniformLocation` class represents a location of a uniform variable in a WebGL program.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `program` | `std::shared_ptr<WebGLProgram>` | The program that contains the uniform |
| `name` | `std::string` | The name of the uniform |
| `location` | `int` | The location of the uniform |

### Methods

```cpp
void setProgram(std::shared_ptr<WebGLProgram> program);
std::shared_ptr<WebGLProgram> getProgram() const;
void setName(const std::string &name);
const std::string &getName() const;
void setLocation(int location);
int getLocation() const;
```

### Usage

Uniform locations are obtained using the `getUniformLocation` method of the WebGL context, and they are used to set uniform values.

```javascript
const modelViewMatrixLocation = gl.getUniformLocation(program, 'u_modelViewMatrix');
gl.uniformMatrix4fv(modelViewMatrixLocation, false, modelViewMatrix);
```

## Object Scope Management

JSAR Runtime provides several scope classes for managing WebGL objects, ensuring proper binding and unbinding of objects.

### WebGLObjectScope

The `WebGLObjectScope` class is a base class for all WebGL object scope classes, providing common functionality.

```cpp
template <typename ContextType, typename ObjectType>
  requires transmute::common::derived_from<ContextType, WebGLContext> &&
           transmute::common::derived_from<ObjectType, WebGLObject>
class WebGLObjectScope
{
public:
  WebGLObjectScope(std::shared_ptr<ContextType> glContext, std::shared_ptr<ObjectType> object);

protected:
  inline bool isObjectInContextChanged() const;
  inline void markObjectInContextChanged();

protected:
  std::shared_ptr<ContextType> glContext_;
  std::shared_ptr<ObjectType> glObject_;

private:
  bool objectInContextChanged_ = false;
};
```

### WebGLProgramScope

The `WebGLProgramScope` class manages the binding and unbinding of WebGL program objects.

```cpp
class WebGLProgramScope final : public WebGLObjectScope<WebGLContext, WebGLProgram>
{
public:
  WebGLProgramScope(std::shared_ptr<WebGLContext> glContext, std::shared_ptr<WebGLProgram> program);
  ~WebGLProgramScope();

public:
  inline std::shared_ptr<client_graphics::WebGLProgram> program() const;
};
```

### WebGLVertexArrayScope

The `WebGLVertexArrayScope` class manages the binding and unbinding of WebGL vertex array objects.

```cpp
class WebGLVertexArrayScope final : public WebGLObjectScope<WebGL2Context, WebGLVertexArray>
{
public:
  WebGLVertexArrayScope(std::shared_ptr<WebGL2Context> glContext, std::shared_ptr<WebGLVertexArray> glObject);
  ~WebGLVertexArrayScope();
};
```

### WebGLVertexBufferScope

The `WebGLVertexBufferScope` class manages the binding and unbinding of WebGL buffer objects.

```cpp
class WebGLVertexBufferScope final : public WebGLObjectScope<WebGL2Context, WebGLBuffer>
{
public:
  WebGLVertexBufferScope(std::shared_ptr<WebGL2Context> glContext,
                         std::shared_ptr<WebGLBuffer> glObject,
                         WebGLBufferBindingTarget bindingTarget);
  ~WebGLVertexBufferScope();

private:
  WebGLBufferBindingTarget bindingTarget_;
};
```

## Examples

### Creating and Using a Shader Program

```javascript
// Create shaders
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, `
  attribute vec4 a_position;
  uniform mat4 u_modelViewMatrix;
  uniform mat4 u_projectionMatrix;
  
  void main() {
    gl_Position = u_projectionMatrix * u_modelViewMatrix * a_position;
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

// Use program
gl.useProgram(program);
```

### Setting Up Buffers and Attributes

```javascript
// Create buffer
const positionBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
  -1.0, -1.0, 0.0,
   1.0, -1.0, 0.0,
   0.0,  1.0, 0.0
]), gl.STATIC_DRAW);

// Set up attribute
gl.enableVertexAttribArray(positionAttributeLocation);
gl.vertexAttribPointer(positionAttributeLocation, 3, gl.FLOAT, false, 0, 0);
```

### Creating and Using a Texture

```javascript
// Create texture
const texture = gl.createTexture();
gl.bindTexture(gl.TEXTURE_2D, texture);

// Set texture parameters
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

// Upload image data
gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

// Use texture
gl.activeTexture(gl.TEXTURE0);
gl.bindTexture(gl.TEXTURE_2D, texture);
gl.uniform1i(textureLocation, 0);
```
