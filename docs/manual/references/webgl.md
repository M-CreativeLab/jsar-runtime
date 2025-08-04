# WebGL and extensions

JSAR implements the [WebGL][] and [WebGL 2.0][WebGL2] standards, and also provides some WebGL extensions to enhance the graphics rendering capabilities. Here you can find the references about the WebGL and WebGL extensions that JSAR supports.

[WebGL]: https://registry.khronos.org/webgl/specs/latest/1.0/
[WebGL2]: https://registry.khronos.org/webgl/specs/latest/2.0/

## Convention

In this document, we will use the following convention to describe implementation status:

| Code | Description |
| --- | --- |
| *NI* | <b>N</b>ot <b>I</b>mplemented |
| *PI* | <b>P</b>artially <b>I</b>mplemented |
| *YI* | <b>Y</b>et to be <b>I</b>mplemented |

## WebGL APIs

In this section, we will provide the implementation status of the WebGL APIs in JSAR.

### Class `WebGLRenderingContext`

The `WebGLRenderingContext` interface provides the WebGL rendering context for the drawing surface. It is part of the WebGL API and is used to draw graphics on the scene.

#### Constants

All the constants defined in the [WebGLRenderingContext][] interface are supported in JSAR, see the MDN's [WebGL constants][] page for more details.

#### The WebGL context

The following properties and methods provide general information and functionality to deal with the WebGL context:

__[`WebGLRenderingContext.canvas`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/canvas)__

> A read-only back-reference to the `HTMLCanvasElement`. Might be `null` if it is not associated with a `<canvas>` element.

__[`WebGLRenderingContext.drawingBufferWidth`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/drawingBufferWidth)__

> The read-only width of the current drawing buffer. Should match the width of the canvas element associated with this context.

__[`WebGLRenderingContext.drawingBufferHeight`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/drawingBufferHeight)__

> The read-only height of the current drawing buffer. Should match the height of the canvas element associated with this context.

__[`WebGLRenderingContext.getContextAttributes()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getContextAttributes)__

> Returns a `WebGLContextAttributes` object containing the actual context configuration.

| Property | Description |
| --- | --- |
| `alpha` | A Boolean that indicates whether the canvas contains an alpha buffer. |
| `depth` | A Boolean that indicates whether the drawing buffer has a depth buffer of at least 16 bits. |
| `stencil` | A Boolean that indicates whether the drawing buffer has a stencil buffer of at least 8 bits. |
| `antialias` | A Boolean that indicates whether the drawing buffer is antialiased. |
| `failIfMajorPerformanceCaveat` | A Boolean that indicates whether a context will be created if the system performance is low. |
| `premultipliedAlpha` | A Boolean that indicates whether the page compositor will assume the drawing buffer contains colors with premultiplied alpha. |
| `preserveDrawingBuffer` | A Boolean that indicates whether the drawing buffer is preserved after rendering. |
| `powerPreference` | A string that indicates the preferred GPU to use. |
| `xrCompatible` | A Boolean that indicates whether the context is compatible with the WebXR Device API. |

__[`WebGLRenderingContext.isContextLost()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isContextLost)__ (*NI*)

> Returns a Boolean indicating whether the WebGL context has been lost.

__[`WebGLRenderingContext.makeXRCompatible()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/makeXRCompatible)__

> Ensures the context is compatible with the user's XR hardware.

At JSAR, you don't need to call this method on the `navigator.gl` object, because this context is created for XR by default.

#### Viewing and clipping

__[`WebGLRenderingContext.scissor()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/scissor)__

> Defines the scissor box.

__[`WebGLRenderingContext.viewport()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/viewport)__

> Sets the viewport.

#### State information

__[`WebGLRenderingContext.activeTexture()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/activeTexture)__

> Selects the active texture unit.

__[`WebGLRenderingContext.blendColor()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/blendColor)__

> Sets the blend color.

__[`WebGLRenderingContext.blendEquation()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/blendEquation)__

> Sets both the RGB blend equation and alpha blend equation to a single equation.

__[`WebGLRenderingContext.blendEquationSeparate()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/blendEquationSeparate)__

> Sets the RGB blend equation and alpha blend equation separately.

__[`WebGLRenderingContext.blendFunc()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/blendFunc)__

> Sets the source and destination blending factors.

__[`WebGLRenderingContext.blendFuncSeparate()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/blendFuncSeparate)__

> Sets the source and destination blending factors separately for RGB and alpha.

__[`WebGLRenderingContext.clearColor()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/clearColor)__

> Sets the color to clear the color buffer to.

Note that because the JSAR is a mixed-rendering architecture, to avoid the applications clearing the color buffer, the `clearColor` won't have any effect.

__[`WebGLRenderingContext.clearDepth()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/clearDepth)__

> Sets the value to clear the depth buffer to.

See the note in `clearColor`, the `clearDepth` won't have any effect, either.

__[`WebGLRenderingContext.clearStencil()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/clearStencil)__

> Sets the value to clear the stencil buffer to.

See the note in `clearColor`, the `clearStencil` won't have any effect, either.

__[`WebGLRenderingContext.colorMask()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/colorMask)__

> Sets which color components to write to the color buffer.

__[`WebGLRenderingContext.cullFace()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/cullFace)__

> Specifies whether or not front- and/or back-facing polygons can be culled.

__[`WebGLRenderingContext.depthFunc()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/depthFunc)__

> Specifies the function used to compare the depth value of a fragment with the depth buffer.

__[`WebGLRenderingContext.depthMask()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/depthMask)__

> Sets whether writing into the depth buffer is enabled or disabled.

__[`WebGLRenderingContext.depthRange()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/depthRange)__

> Specifies a linear mapping of the depth values in the depth buffer.

__[`WebGLRenderingContext.disable()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/disable)__

> Disables a GL capability.

__[`WebGLRenderingContext.enable()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/enable)__

> Enables a GL capability.

__[`WebGLRenderingContext.frontFace()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/frontFace)__

> Specifies whether front- or back-facing polygons can be culled.

__[`WebGLRenderingContext.getParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getParameter)__

> Returns the value of a parameter for a given target.

__[`WebGLRenderingContext.getError()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getError)__

> Returns the error value.

__[`WebGLRenderingContext.hint()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/hint)__

> Specifies implementation-specific hints.

__[`WebGLRenderingContext.isEnabled()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isEnabled)__ (*NI*)

> Returns whether a GL capability is enabled or not.

__[`WebGLRenderingContext.lineWidth()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/lineWidth)__

> Sets the width of rasterized lines.

__[`WebGLRenderingContext.pixelStorei()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/pixelStorei)__

> Sets the pixel storage modes.

__[`WebGLRenderingContext.polygonOffset()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/polygonOffset)__

> Sets the scale and units used to calculate depth values.

__[`WebGLRenderingContext.sampleCoverage()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/sampleCoverage)__ (*NI*)

> Specifies multisample coverage parameters.

__[`WebGLRenderingContext.stencilFunc()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/stencilFunc)__

> Sets the front and back function and reference value for stencil testing.

__[`WebGLRenderingContext.stencilFuncSeparate()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/stencilFuncSeparate)__

> Sets the front and back function and reference value for stencil testing.

__[`WebGLRenderingContext.stencilMask()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/stencilMask)__

> Controls the writing of individual bits in the stencil planes.

__[`WebGLRenderingContext.stencilMaskSeparate()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/stencilMaskSeparate)__

> Controls the writing of individual bits in the stencil planes.

__[`WebGLRenderingContext.stencilOp()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/stencilOp)__

> Sets the front and back-facing stencil test actions.

__[`WebGLRenderingContext.stencilOpSeparate()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/stencilOpSeparate)__

> Sets the front and back-facing stencil test actions.

#### Buffers

__[`WebGLRenderingContext.bindBuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bindBuffer)__

> Binds a buffer to a target.

__[`WebGLRenderingContext.bufferData()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bufferData)__

> Creates and initializes a buffer object's data store.

__[`WebGLRenderingContext.bufferSubData()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bufferSubData)__

> Updates a subset of a buffer object's data store.

__[`WebGLRenderingContext.createBuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/createBuffer)__

> Creates a buffer object.

__[`WebGLRenderingContext.deleteBuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/deleteBuffer)__

> Deletes buffer objects.

__[`WebGLRenderingContext.isBuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isBuffer)__ (*NI*)

> Returns whether a given object is a buffer.

#### Framebuffers

__[`WebGLRenderingContext.bindFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bindFramebuffer)__

> Binds a `WebGLFramebuffer` object to a given target.

__[`WebGLRenderingContext.checkFramebufferStatus()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/checkFramebufferStatus)__

> Checks the completeness status of the `WebGLFramebuffer` object.

__[`WebGLRenderingContext.createFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/createFramebuffer)__

> Creates a `WebGLFramebuffer` object.

__[`WebGLRenderingContext.deleteFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/deleteFramebuffer)__

> Deletes a `WebGLFramebuffer` object.

__[`WebGLRenderingContext.framebufferRenderbuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/framebufferRenderbuffer)__

> Attaches a renderbuffer object to a framebuffer object.

__[`WebGLRenderingContext.framebufferTexture2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/framebufferTexture2D)__

> Attaches a texture object to a framebuffer object.

__[`WebGLRenderingContext.getFramebufferAttachmentParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getFramebufferAttachmentParameter)__ (*NI*)

> Returns information about attachments to a framebuffer object.

__[`WebGLRenderingContext.isFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isFramebuffer)__ (*NI*)

> Returns whether a given object is a framebuffer.

__[`WebGLRenderingContext.readPixels()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/readPixels)__ (*NI*)

> Reads a block of pixels from the framebuffer.

#### Renderbuffers

__[`WebGLRenderingContext.bindRenderbuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bindRenderbuffer)__

> Binds a `WebGLRenderbuffer` object to a target.

__[`WebGLRenderingContext.createRenderbuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/createRenderbuffer)__

> Creates a `WebGLRenderbuffer` object.

__[`WebGLRenderingContext.deleteRenderbuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/deleteRenderbuffer)__

> Deletes a `WebGLRenderbuffer` object.

__[`WebGLRenderingContext.getRenderbufferParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getRenderbufferParameter)__ (*NI*)

> Returns information about a renderbuffer object.

__[`WebGLRenderingContext.isRenderbuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isRenderbuffer)__ (*NI*)

> Returns whether a given object is a renderbuffer.

__[`WebGLRenderingContext.renderbufferStorage()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/renderbufferStorage)__

> Defines storage and format of a renderbuffer object's image.

#### Textures

__[`WebGLRenderingContext.bindTexture()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bindTexture)__

> Binds a texture to a target.

__[`WebGLRenderingContext.compressedTexImage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/compressedTexImage2D)__

> Specifies a two-dimensional texture image in a compressed format.

__[`WebGLRenderingContext.compressedTexSubImage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/compressedTexSubImage2D)__

> Specifies a sub-rectangle of a two-dimensional texture image in a compressed format.

__[`WebGLRenderingContext.copyTexImage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/copyTexImage2D)__

> Copies pixels from the current framebuffer to a texture image.

__[`WebGLRenderingContext.copyTexSubImage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/copyTexSubImage2D)__

> Copies pixels from the current framebuffer to a sub-rectangle of a texture image.

__[`WebGLRenderingContext.createTexture()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/createTexture)__

> Creates a texture object.

__[`WebGLRenderingContext.deleteTexture()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/deleteTexture)__

> Deletes a texture object.

__[`WebGLRenderingContext.generateMipmap()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/generateMipmap)__

> Generates a set of mipmaps for a texture object.

__[`WebGLRenderingContext.getTexParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getTexParameter)__ (*NI*)

> Returns the value of a texture parameter.

__[`WebGLRenderingContext.isTexture()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isTexture)__ (*NI*)

> Returns whether a given object is a texture.

__[`WebGLRenderingContext.texImage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/texImage2D)__

> Specifies a two-dimensional texture image.

__[`WebGLRenderingContext.texSubImage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/texSubImage2D)__

> Specifies a sub-rectangle of a two-dimensional texture image.

__[`WebGLRenderingContext.texParameterf()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/texParameterf)__ (*NI*)

> Sets a float texture parameter.

__[`WebGLRenderingContext.texParameteri()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/texParameteri)__

> Sets an integer texture parameter.

#### Programs and shaders

__[`WebGLRenderingContext.attachShader()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/attachShader)__

> Attaches a shader to a program.

__[`WebGLRenderingContext.bindAttribLocation()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bindAttribLocation)__

> Binds a generic vertex attribute index to a named attribute variable.

__[`WebGLRenderingContext.compileShader()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/compileShader)__

> Compiles a `WebGLShader`.

__[`WebGLRenderingContext.createProgram()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/createProgram)__

> Creates a `WebGLProgram`.

__[`WebGLRenderingContext.createShader()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/createShader)__

> Creates a `WebGLShader`.

__[`WebGLRenderingContext.deleteProgram()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/deleteProgram)__

> Deletes a `WebGLProgram`.

__[`WebGLRenderingContext.deleteShader()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/deleteShader)__

> Deletes a `WebGLShader`.

__[`WebGLRenderingContext.detachShader()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/detachShader)__

> Detaches a shader from a program.

__[`WebGLRenderingContext.getAttachedShaders()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getAttachedShaders)__

> Returns an array of the `WebGLShader` objects attached to a `WebGLProgram`.

__[`WebGLRenderingContext.getProgramParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getProgramParameter)__

> Returns information about a `WebGLProgram`.

__[`WebGLRenderingContext.getProgramInfoLog()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getProgramInfoLog)__

> Returns information about the last link operation.

__[`WebGLRenderingContext.getShaderParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getShaderParameter)__

> Returns information about a `WebGLShader`.

__[`WebGLRenderingContext.getShaderPrecisionFormat()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getShaderPrecisionFormat)__

> Returns a `WebGLShaderPrecisionFormat` object describing the precision and range of supported floating-point formats.

__[`WebGLRenderingContext.getShaderInfoLog()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getShaderInfoLog)__

> Returns information about the last compile operation.

__[`WebGLRenderingContext.getShaderSource()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getShaderSource)__

> Returns the source code of a `WebGLShader`.

__[`WebGLRenderingContext.isProgram()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isProgram)__ (*NI*)

> Returns whether a given object is a program.

__[`WebGLRenderingContext.isShader()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/isShader)__ (*NI*)

> Returns whether a given object is a shader.

__[`WebGLRenderingContext.linkProgram()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/linkProgram)__

> Links a `WebGLProgram`.

__[`WebGLRenderingContext.shaderSource()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/shaderSource)__

> Sets the source code in a `WebGLShader`.

__[`WebGLRenderingContext.useProgram()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/useProgram)__

> Installs a program object as part of the current rendering state.

__[`WebGLRenderingContext.validateProgram()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/validateProgram)__ (*NI*)

> Validates a `WebGLProgram`.

#### Uniforms and attributes

__[`WebGLRenderingContext.disableVertexAttribArray()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/disableVertexAttribArray)__

> Disables a generic vertex attribute array.

__[`WebGLRenderingContext.enableVertexAttribArray()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/enableVertexAttribArray)__

> Enables a generic vertex attribute array.

__[`WebGLRenderingContext.getActiveAttrib()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getActiveAttrib)__

> Returns information about an active attribute variable.

__[`WebGLRenderingContext.getActiveUniform()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getActiveUniform)__

> Returns information about an active uniform variable.

__[`WebGLRenderingContext.getAttribLocation()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getAttribLocation)__

> Returns the location of an attribute variable.

__[`WebGLRenderingContext.getUniform()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getUniform)__ (*NI*)

> Returns the value of a uniform variable.

__[`WebGLRenderingContext.getUniformLocation()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getUniformLocation)__

> Returns the location of a uniform variable.

__[`WebGLRenderingContext.getVertexAttrib()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getVertexAttrib)__ (*NI*)

> Returns the value of a vertex attribute.

__[`WebGLRenderingContext.getVertexAttribOffset()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getVertexAttribOffset)__ (*NI*)

> Returns the byte offset of a vertex attribute.

__[`WebGLRenderingContext.uniform[1234][fi][v]()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/uniform)__

> Specifies a value for a uniform variable.

__[`WebGLRenderingContext.uniformMatrix[234]fv()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/uniformMatrix)__

> Specifies a matrix value for a uniform variable.

__[`WebGLRenderingContext.vertexAttrib[1234]f[v]()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/vertexAttrib)__ (*NI*)

> Specifies the value of a generic vertex attribute.

__[`WebGLRenderingContext.vertexAttribPointer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/vertexAttribPointer)__

> Specifies the location and data format of a vertex attribute array.

#### Drawing buffers

__[`WebGLRenderingContext.clear()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/clear)__

> Clears specified buffers to preset values.

Because JSAR is a mixed-rendering architecture, to avoid the applications clearing the color buffer, the `clear` won't have any effect.

__[`WebGLRenderingContext.drawArrays()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/drawArrays)__

> Renders primitives from array data.

__[`WebGLRenderingContext.drawElements()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/drawElements)__

> Renders primitives from array data.

__[`WebGLRenderingContext.finish()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/finish)__ (*NI*)

> Blocks execution until all previously called commands are finished.

__[`WebGLRenderingContext.flush()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/flush)__ (*NI*)

> Empties different buffer commands, causing all commands to be executed as quickly as possible.

#### Color spaces

__[`WebGLRenderingContext.drawingBufferColorSpace`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/drawingBufferColorSpace)__ (*NI*)

> Specifies the color space of the WebGL drawing buffer.

__[`WebGLRenderingContext.unpackColorSpace`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/unpackColorSpace)__ (*NI*)

> Specifies the color space to convert to when importing textures.

#### Working with extensions

__[`WebGLRenderingContext.getSupportedExtensions()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getSupportedExtensions)__

> Returns an Array of strings containing all the supported WebGL extensions.

__[`WebGLRenderingContext.getExtension()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getExtension)__

> Returns an extension object.

### Class `WebGL2RenderingContext`

The [`WebGL2RenderingContext`][WebGL2RenderingContext] interface provides the WebGL 2.0 rendering context for the drawing surface. It is part of the WebGL 2.0 API and is used to draw graphics on the scene.

#### Constants

See the MDN's [WebGL constants][] page.

#### State information

__[`WebGL2RenderingContext.getIndexedParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getIndexedParameter)__ (*NI*)

> Returns the value of a parameter for a specific target and index.

#### Buffers

__[`WebGL2RenderingContext.bufferData()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bufferData)__ (*PI*)

> Initializes and creates the buffer object's data store.

__[`WebGL2RenderingContext.bufferSubData()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bufferSubData)__ (*PI*)

> Updates a subset of a buffer object's data store.

__[`WebGL2RenderingContext.copyBufferSubData()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/copyBufferSubData)__ (*NI*)

> Copies a block of data from one buffer to another.

__[`WebGL2RenderingContext.getBufferSubData()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getBufferSubData)__ (*NI*)

> Returns a subset of a buffer object's data store.

#### Framebuffers

__[`WebGL2RenderingContext.blitFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/blitFramebuffer)__

> Transfers a block of pixels from the read framebuffer to the draw framebuffer.

__[`WebGL2RenderingContext.framebufferTextureLayer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/framebufferTextureLayer)__ (*NI*)

> Attaches a single layer of a texture to a framebuffer object.

__[`WebGL2RenderingContext.invalidateFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/invalidateFramebuffer)__ (*NI*)

> Discards the contents of a framebuffer.

__[`WebGL2RenderingContext.invalidateSubFramebuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/invalidateSubFramebuffer)__ (*NI*)

> Discards the contents of a region within a framebuffer.

__[`WebGL2RenderingContext.readBuffer()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/readBuffer)__

> Specifies the color buffer from which pixels are read.

#### Renderbuffers

__[`WebGL2RenderingContext.getInternalformatParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getInternalformatParameter)__ (*NI*)

> Returns information about the implementation's support for specific internal formats.

__[`WebGL2RenderingContext.renderbufferStorageMultisample()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/renderbufferStorageMultisample)__

> Defines storage and format of a renderbuffer object's image.

#### Textures

__[`WebGL2RenderingContext.texStorage2D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/texStorage2D)__

> Specifies the storage requirements for a two-dimensional texture.

__[`WebGL2RenderingContext.texStorage3D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/texStorage3D)__

> Specifies the storage requirements for a three-dimensional texture.

__[`WebGL2RenderingContext.texImage3D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/texImage3D)__

> Specifies a three-dimensional texture image.

__[`WebGL2RenderingContext.texSubImage3D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/texSubImage3D)__

> Specifies a sub-rectangle of a three-dimensional texture image.

__[`WebGL2RenderingContext.copyTexSubImage3D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/copyTexSubImage3D)__ (*NI*)

> Copies pixels from the current framebuffer to a sub-rectangle of a three-dimensional texture image.

__[`WebGL2RenderingContext.compressedTexImage3D()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/compressedTexImage3D)__ (*NI*)

> Specifies a three-dimensional texture image in a compressed format.

__[`WebGL2RenderingContext.compressedTexSubImage3D()`](https://developer.mozilla.org/en-US/docs/WebGL2RenderingContext/compressedTexSubImage3D)__ (*NI*)

> Specifies a sub-rectangle of a three-dimensional texture image in a compressed format.

#### Programs and shaders

__[`WebGL2RenderingContext.getFragDataLocation()`](https://developer.mozilla.org/en-US/docs/WebGL2RenderingContext/getFragDataLocation)__ (*NI*)

> Returns the location of a fragment output variable.

#### Uniforms and attributes

__[`WebGL2RenderingContext.uniform[1234][uif][v]()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/uniform)__ (*PI*)

> Specifies a value for a uniform variable.

__[`WebGL2RenderingContext.uniformMatrix[234]x[234]fv()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/uniformMatrix)__ (*PI*)

> Specifies a matrix value for a uniform variable.

__[`WebGL2RenderingContext.vertexAttribI4[u]i[v]()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/vertexAttribI)__

#### Color spaces

__[`WebGL2RenderingContext.drawingBufferColorSpace`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/drawingBufferColorSpace)__ (*NI*)

> Specifies the color space of the WebGL drawing buffer.

__[`WebGL2RenderingContext.unpackColorSpace`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/unpackColorSpace)__ (*NI*)

> Specifies the color space to convert to when importing textures.

#### Drawing buffers

__[`WebGL2RenderingContext.vertexAttribDivisor()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/vertexAttribDivisor)__

> Modifies the rate at which generic vertex attributes advance during instanced rendering.

__[`WebGL2RenderingContext.drawArraysInstanced()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/drawArraysInstanced)__

> Renders primitives from array data. In addition, it can execute multiple instances of the range of elements.

__[`WebGL2RenderingContext.drawElementsInstanced()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/drawElementsInstanced)__

> Renders primitives from array data. In addition, it can execute multiple instances of a set of elements.

__[`WebGL2RenderingContext.drawRangeElements()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/drawRangeElements)__

> Renders primitives from array data.

__[`WebGL2RenderingContext.drawBuffers()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/drawBuffers)__

> Specifies a list of color buffers to be drawn into.

__[`WebGL2RenderingContext.clearBuffer[fiuv]()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/clearBuffer)__ (*NI*)

> Clears buffers from the currently bound framebuffer.

#### Query objects

__[`WebGL2RenderingContext.createQuery()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/createQuery)__ (*NI*)

> Creates a new `WebGLQuery` object.

__[`WebGL2RenderingContext.deleteQuery()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/deleteQuery)__ (*NI*)

> Deletes a given `WebGLQuery` object.

__[`WebGL2RenderingContext.isQuery()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/isQuery)__ (*NI*)

> Returns `true` if a given object is a valid `WebGLQuery` object.

__[`WebGL2RenderingContext.beginQuery()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/beginQuery)__ (*NI*)

> Begins an asynchronous query.

__[`WebGL2RenderingContext.endQuery()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/endQuery)__ (*NI*)

> Marks the end of an asynchronous query.

__[`WebGL2RenderingContext.getQuery()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getQuery)__ (*NI*)

> Returns the query result.

__[`WebGL2RenderingContext.getQueryParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getQueryParameter)__ (*NI*)

> Returns information about a query object.

#### Sampler objects

__[`WebGL2RenderingContext.createSampler()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/createSampler)__ (*NI*)

> Creates a new `WebGLSampler` object.

__[`WebGL2RenderingContext.deleteSampler()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/deleteSampler)__ (*NI*)

> Deletes a given `WebGLSampler` object.

__[`WebGL2RenderingContext.bindSampler()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bindSampler)__ (*NI*)

> Binds a `WebGLSampler` object to a texture unit.

__[`WebGL2RenderingContext.isSampler()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/isSampler)__ (*NI*)

> Returns `true` if a given object is a valid `WebGLSampler` object.

__[`WebGL2RenderingContext.samplerParameter[if]()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/samplerParameter)__ (*NI*)

> Sets a parameter for a `WebGLSampler` object.

__[`WebGL2RenderingContext.getSamplerParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getSamplerParameter)__ (*NI*)

> Returns the value of a parameter for a `WebGLSampler` object.

#### Sync objects

__[`WebGL2RenderingContext.fenceSync()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/fenceSync)__ (*NI*)

> Creates a new `WebGLSync` object.

__[`WebGL2RenderingContext.isSync()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/isSync)__ (*NI*)

> Returns `true` if a given object is a valid `WebGLSync` object.

__[`WebGL2RenderingContext.deleteSync()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/deleteSync)__ (*NI*)

> Deletes a given `WebGLSync` object.

__[`WebGL2RenderingContext.clientWaitSync()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/clientWaitSync)__ (*NI*)

> Blocks execution until the specified sync object is signaled.

__[`WebGL2RenderingContext.waitSync()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/waitSync)__ (*NI*)

> Blocks execution until the specified sync object is signaled.

__[`WebGL2RenderingContext.getSyncParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getSyncParameter)__ (*NI*)

> Returns information about a sync object.

#### Transform feedback

__[`WebGL2RenderingContext.createTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/createTransformFeedback)__ (*NI*)

> Creates a new `WebGLTransformFeedback` object.

__[`WebGL2RenderingContext.deleteTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/deleteTransformFeedback)__ (*NI*)

> Deletes a given `WebGLTransformFeedback` object.

__[`WebGL2RenderingContext.isTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/isTransformFeedback)__ (*NI*)

> Returns `true` if a given object is a valid `WebGLTransformFeedback` object.

__[`WebGL2RenderingContext.bindTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bindTransformFeedback)__ (*NI*)

> Binds a `WebGLTransformFeedback` object.

__[`WebGL2RenderingContext.beginTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/beginTransformFeedback)__ (*NI*)

> Starts a transform feedback operation.

__[`WebGL2RenderingContext.endTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/endTransformFeedback)__ (*NI*)

> Ends a transform feedback operation.

__[`WebGL2RenderingContext.transformFeedbackVaryings()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/transformFeedbackVaryings)__ (*NI*)

> Specifies output variables for the transform feedback process.

__[`WebGL2RenderingContext.getTransformFeedbackVarying()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getTransformFeedbackVarying)__ (*NI*)

> Returns information about an output variable.

__[`WebGL2RenderingContext.pauseTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/pauseTransformFeedback)__ (*NI*)

> Pauses a transform feedback operation.

__[`WebGL2RenderingContext.resumeTransformFeedback()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/resumeTransformFeedback)__ (*NI*)

> Resumes a transform feedback operation.

#### Uniform buffer objects

__[`WebGL2RenderingContext.bindBufferBase()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bindBufferBase)__

> Binds a given `WebGLBuffer` to a given binding point (`target`) at a given index.

__[`WebGL2RenderingContext.bindBufferRange()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bindBufferRange)__

> Binds a range of a given `WebGLBuffer` to a given binding point (`target`) at a given index.

__[`WebGL2RenderingContext.getUniformIndices()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getUniformIndices)__ (*NI*)

> Retrieves the indices of a number of uniforms within a `WebGLProgram`.

__[`WebGL2RenderingContext.getActiveUniforms()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getActiveUniforms)__ (*NI*)

> Retrieves the values of a number of uniforms within a `WebGLProgram`.

__[`WebGL2RenderingContext.getUniformBlockIndex()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getUniformBlockIndex)__

> Retrieves the index of a uniform block within a `WebGLProgram`.

__[`WebGL2RenderingContext.getActiveUniformBlockParameter()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getActiveUniformBlockParameter)__ (*NI*)

> Retrieves information about a uniform block within a `WebGLProgram`.

__[`WebGL2RenderingContext.getActiveUniformBlockName()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/getActiveUniformBlockName)__ (*NI*)

> Retrieves the name of a uniform block within a `WebGLProgram`.

__[`WebGL2RenderingContext.uniformBlockBinding()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/uniformBlockBinding)__

> Assigns a specific uniform block to a uniform block binding point.

#### Vertex array objects

__[`WebGL2RenderingContext.createVertexArray()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/createVertexArray)__

> Creates a new `WebGLVertexArrayObject`.

__[`WebGL2RenderingContext.deleteVertexArray()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/deleteVertexArray)__

> Deletes a given `WebGLVertexArrayObject`.

__[`WebGL2RenderingContext.isVertexArray()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/isVertexArray)__

> Returns `true` if a given object is a valid `WebGLVertexArrayObject`.

__[`WebGL2RenderingContext.bindVertexArray()`](https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/bindVertexArray)__

> Binds a given `WebGLVertexArrayObject`.

## Supported WebGL extensions

In this section, we list the supported WebGL extensions in the JSAR.

### EXT_color_buffer_float

> See: https://developer.mozilla.org/en-US/docs/Web/API/EXT_color_buffer_float.

### EXT_texture_filter_anisotropic

> See: https://developer.mozilla.org/en-US/docs/Web/API/EXT_texture_filter_anisotropic.

This extension added new constants:

```js
const texture = gl.createTexture();
gl.bindTexture(gl.TEXTURE_2D, texture);

const ext = gl.getExtension('EXT_texture_filter_anisotropic');
if (ext) {
  const max = gl.getParameter(ext.MAX_TEXTURE_MAX_ANISOTROPY_EXT);
  gl.texParameterf(gl.TEXTURE_2D, ext.TEXTURE_MAX_ANISOTROPY_EXT, max);
}
```

### OES_element_index_uint

> See: https://developer.mozilla.org/en-US/docs/Web/API/OES_element_index_uint.

### OES_standard_derivatives

> See: https://developer.mozilla.org/en-US/docs/Web/API/OES_standard_derivatives.

### OES_texture_float_linear

> See: https://developer.mozilla.org/en-US/docs/Web/API/OES_texture_float_linear.

### OVR_multiview2

> See: https://developer.mozilla.org/en-US/docs/Web/API/OVR_multiview2.

This extension is used to render to multiple views in a single draw call.

There are new constants added.

- `FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR`: Number of views of the framebuffer object attachment.
- `FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR`: Base view index of the framebuffer object attachment.
- `MAX_VIEWS_OVR`: The maximum number of views. Most VR headsets have two views, but there are prototypes of headset with ultra-wide FOV using 4 views which is currently the maximum number of views supported by multiview.
- `FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR`: If baseViewIndex is not the same for all framebuffer attachment points where the value of `FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE` is not `NONE`, the framebuffer is considered incomplete. Calling `checkFramebufferStatus` for a framebuffer in this state returns `FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR`.

This extension also exposes a new method `framebufferTextureMultiviewOVR` that is used to attach a texture array to the given framebuffer object. However in JSAR, 3D libraries doesn't need to use this function to create a multiview framebuffer. Instead, the application will use the framebuffer that created by the host, and the library will render to the views of the framebuffer.

To make the existing libraries like Three.js or Babylon.js which multiview rendering are still using `framebufferTextureMultiviewOVR()`, JSAR implemented this function but nothing will happen when it is called.

### OCULUS_multiview

This extension is introduced by [Meta Horizon][] at its blog: [Multiview WebGL Rendering](https://developers.meta.com/horizon/documentation/web/web-multiview/).

This extension inherits from `OVR_multiview2` and add a new function `framebufferTextureMultisampleMultiviewOVR` to address the multisampled antialiasing issue in multiview rendering.

Because of JSAR's mixed-rendering architecture, the multiview framebuffer is managed by the host application, so the library doesn't need to use the function `framebufferTextureMultisampleMultiviewOVR` to enable multisampled antialiasing in multiview rendering.

To 3D libraries in JSAR, using `OCULUS_multiview` is the same as using `OVR_multiview2`, and the library only needs to make draw calls on the multiview framebuffer.

[WebGLRenderingContext]: https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext
[WebGL2RenderingContext]: https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext
[WebGL constants]: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Constants
[Meta Horizon]: https://developers.meta.com/horizon
