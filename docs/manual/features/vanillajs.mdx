# Vanilla.js support

[Vanilla.js][vanillajs] is not any a framework or library, but a term used to describe the use of plain JavaScript without any additional libraries or frameworks. It is the opposite of using a library like jQuery, which provides a lot of utility functions and methods to make working with JavaScript easier.

> For more details about Vanilla.js, you can visit the official website at [vanilla-js.com][vanillajs].

In this article, we use the term Vanilla.js to refer to the use of plain JavaScript/TypeScript without the popular 3D libraries like [Three.js][threejs] or [Babylon.js][babylonjs] that JSAR supports.

In this article, we will show you:

- How to create space(3D) applications using Vanilla.js, namely with the APIs of WebXR and WebGL.
- How to support your custom WebGL-based 3D library or framework in JSAR.

> WebGPU will be available later.

## Preparing your HTML

To create your space applications, you need to prepare the HTML firstly:

```html title="index-vanillajs.html"
<html>
<head>
  <title>Vanilla.js Space Application</title>
  <script type="module">
    console.info(navigator.xr);
    console.info(navigator.gl);

    navigator.xr.requestSession('immersive-ar').then((session) => {
      const baseLayer = new XRWebGLLayer(session, navigator.gl);
      session.updateRenderState({ baseLayer });

      // Start rendering
    });
  </script>
</head>
<body>
</body>
</html>
```

In the above code, `navigator.xr` is the API for WebXR, and `navigator.gl` is introduced by JSAR to provide the WebGL context for stereo rendering.

Note that unlike the classic Web browser, that requires the `navigator.xr.requestSession` to be called when the user clicks on a so-called "VR/AR" button, that is not necessary in JSAR because the user is already in an XR environment.

The `navigator.gl` is a `WebGLRenderingContext` or `WebGL2RenderingContext` object, which is created by JSAR and for stereo rendering in WebXR, thus it's xr-compatible by default.

And the above code requested a `immersive-ar` session, and created a `XRWebGLLayer` object with the `navigator.gl` context, and set it as the base layer of the session, these lines of code are necessary to tell the JSAR to render the `navigator.gl` context to the session.

## Setup your renderer

Before start rendering your scene, you need use the `navigator.gl` to create buffers, shaders, and other resources for rendering.

```js title="create-vertex-buffer.js"
const buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
  -1, -1, 0,
  1, -1, 0,
  -1, 1, 0,
  1, 1, 0,
]), gl.STATIC_DRAW);
```

```js title="create-shaders.js"
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, `
  attribute vec3 position;
  void main() {
    gl_Position = vec4(position, 1.0);
  }
`);
gl.compileShader(vertexShader);

const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
gl.shaderSource(fragmentShader, `
  void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
`);
gl.compileShader(fragmentShader);
```

```js title="create-program.js"
const program = gl.createProgram();
gl.attachShader(program, vertexShader);
gl.attachShader(program, fragmentShader);
gl.linkProgram(program);
```

The above code snippets are just samples to show the use of the rendering context object, developers are free to use any WebGL APIs to create their own rendering pipeline.

## Request a reference space

To render your scene in the host scene, you need to request a reference space to tell where the origin of your scene is.

```typescript title="request-reference-space.ts"
let rootSpace: XRReferenceSpace;
session.requestReferenceSpace('local').then((referenceSpace) => {
  // Store the reference space for later use
  rootSpace = referenceSpace;
});
```

> Currently JSAR only supports the `local` reference space type, other types will be supported later.

## Request animation frames

After setting up the renderer, you can start rendering your scene by requesting the animation frame.

```typescript title="render-loop.ts"
function render(time, frame: XRFrame) {
  session.requestAnimationFrame(render);

  if (!rootSpace) {
    // Skip if the root reference space is not ready
    return;
  }
  const viewerPose = frame.getViewerPose(rootSpace);
  for (const view of viewerPose.views) {
    console.info(view.projectionMatrix);
    console.info(view.transform);
  }
  // This depends on which stero rendering mode is used: multiview or not?
}

session.requestAnimationFrame(render);
```

The above shows how to request animation frames in WebXR, and how to get the viewer pose and views from the frame object, for more usages about the `XRFrame` and other objects, you can refer to to the [WebXR basis reference](./references/webxr).

## Stereo rendering

To render your scene to the XR device, you need to achieve stereo rendering by your own. In JSAR, you need implement the single-pass stereo rendering and the single-pass multiview rendering, this is because the stereo rendering mode is not decided by the applications in JSAR, but by the host, if the host uses the single-pass multiview rendering, you need to render your scene to the multiview framebuffer, otherwise, you need to render your scene to the left and right eye framebuffers.

### Checking the stereo rendering mode

To check the stereo rendering mode in runtime, you can use the following code:

```typescript title="check-stereo-rendering-mode.ts"
if (baseLayer.multiviewRequired) {
  // Use the single-pass multiview rendering
} else {
  // Use the single-pass stereo rendering
}
```

The `XRWebGLLayer` object has a new property `multiviewRequired` in JSAR, which indicates whether the single-pass multiview rendering is required or not.

> At YodaOS-Master and _Play WebXR_, the single-pass multiview rendering is used only because it reduces CPU usage of applications, we recommend you to implement the single-pass multiview rendering firstly if you only want to support one at the beginning.

### Single-pass stereo rendering

To achieve the single-pass stereo rendering, you need to render your objects to the left and right eye framebuffers only, and no need to create the final framebuffer to display.

```typescript title="single-pass-stereo-rendering.ts"
for (const view of viewerPose.views) {
  // Set the projection matrix and view matrix
  gl.bindFramebuffer(gl.FRAMEBUFFER, baseLayer.framebuffer);
  gl.useProgram(program);

  // Set the matrices
  gl.uniformMatrix4fv(gl.getUniformLocation(program, 'projectionMatrix'), false, view.projectionMatrix);
  gl.uniformMatrix4fv(gl.getUniformLocation(program, 'viewMatrix'), false, view.transform.inverse.matrix);

  // Draw the objects
  gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
  gl.enableVertexAttribArray(0);
  gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 0, 0);
  gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}
```

In each view, set the projection matrix and view matrix, and draw the scene to the framebuffer of the `XRWebGLLayer` object.

### Single-pass multiview rendering

To achieve the single-pass multiview rendering, you need to render your objects to the framebuffer.

```typescript title="single-pass-multiview-rendering.ts"
gl.bindFramebuffer(gl.FRAMEBUFFER, baseLayer.framebuffer);

for (const view of viewerPose.views) {
  // Set the matrices
  const postfix = view.eye === 'left' ? 'L' : 'R';
  gl.uniformMatrix4fv(gl.getUniformLocation(program, `projectionMatrix${postfix}`), false, view.projectionMatrix);
  gl.uniformMatrix4fv(gl.getUniformLocation(program, `viewMatrix${postfix}`), false, view.transform.inverse.matrix);
}

// Draw the objects
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.enableVertexAttribArray(0);
gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 0, 0);
gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
```

As you can see, the single-pass multiview rendering is similar to the single-pass stereo rendering, but you need to set the matrices for each eye view, and draw the objects only once.

To get the multiview rendering working, you need to modify the shaders to support the multiview rendering, for example, you can use the following code to get the eye index:

```glsl title="multiview-shader.vert"
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;

uniform mat4 projectionMatrixL;
uniform mat4 projectionMatrixR;
uniform mat4 viewMatrixL;
uniform mat4 viewMatrixR;

void main() {
  gl_Position = (gl_ViewID_OVR == 0 ? projectionMatrixL : projectionMatrixR) * (gl_ViewID_OVR == 0 ? viewMatrixL : viewMatrixR) * vec4(position, 1.0);
}
```

```glsl title="multiview-shader.frag"
#extension GL_OVR_multiview2 : enable

void main() {
  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
```

There are some modifications in the above shaders to enable the multiview rendering:

- Use the `GL_OVR_multiview2` extension to enable the multiview rendering.
- Use the `layout(num_views = 2) in;` to specify the number of views, it means the input views are two.
- Use the `gl_ViewID_OVR` to get the current view index to select the corresponding matrices.

### Compositing the final framebuffer

At classic Web browsers, you need to create the final framebuffer to display the stereo rendering result, but in JSAR, this step is achieved by the host, so applications don't need to do this compositing step.

Because of this, especially for the single-pass multiview rendering, you won't need to use `OCCLUS_multiview` to support the MSAA in multiview rendering, because the framebuffer to use is absolute managed by the host, and the applications just need to render objects to the framebuffer.

## Limitations

When you have finished the above works, you can see your objects in the host scene, but when you move your head (viewer), you will find a rendering delay, this is because JSAR uses an asynchronous rendering method to avoid the host blocking, and the objects from applications are rendered delayed always.

To resolve this issue, you need make some changes to your shader uniforms:

- Use the following names for the projection matrix or matrices:
  - `projectionMatrix`
  - `projectionMatrices`
- Use the following names for the view matrix or matrices:
  - `viewMatrix`
  - `viewMatrices`
- Not use the `modelViewMatrix`, `modelViewProjectionMatrix`, etc.
- Not use the uniform blocks for the view and projection matrices.

With the above changes, JSAR renderer will use the present time to get the correct matrices for them, and the delay will be erased.

We recommend developers not using the matrices like `modelViewMatrix`, `modelViewProjectionMatrix`, etc., even though they are supported conditionally by JSAR.

The reason is that JSAR will replace the view and projection matrices in the present frame to avoid the rendering delay, if a computed matrix is used in the shaders, it's impossible to replace the computed matrix by the view and projection matrices, so the delay will be still there.

If you still want to use the `modelViewMatrix`, JSAR has a GLSL preprocessor to replace the `modelViewMatrix` to the `viewMatrix * modelMatrix` in the shaders, thus a `modelMatrix` uniform is need to be declared and set in the applications. And Three.js support is an example of this.

Then, don't use the uniform blocks for the view and projection matrices, because the uniform blocks is lack of the data structure at renderer, thus to replace the corresponding matrices in the present frame is impossible, and the delay will be still there.

[threejs]: https://threejs.org/
[babylonjs]: https://www.babylonjs.com/
[vanillajs]: https://vanilla-js.com/
