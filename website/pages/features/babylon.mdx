# Babylon.js support

[Babylon.js][] is yet another popular 3D engine that can be used at JSAR.

In this chapter, you will learn how to use [Babylon.js][] in JSAR.

## Installation

As the first step, you need to install the library into your HTML:

```html title="index.html"
<html>
<head>
  <meta charset="utf-8">
  <title>My first Babylon.js app</title>
  <style>
    body { margin: 0; }
  </style>
</head>
<body>
  <script type="module" src="/main.js"></script>
</body>
</html>
```

And import the babylon in your script:

```js title="main.js"
import 'babylonjs';

console.info(BABYLON);  // namely window.BABYLON
```

To make the above code work, you need to use `importmap` to configure the Babylon.js library:

```html
<script type="importmap">
  {
    "imports": {
      "babylonjs": "http://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/babylon/umd/babylonjs/babylon.js"
    }
  }
</script>
```

## Create a `BABYLON.Engine`

Now that we've set up the basic project structure for Babylon.js, you need to create a `BABYLON.Engine` instance before scene constructing:

```js title="main.js"
let engine;
if (!navigator.gl) {
  const canvas = document.createElement('canvas');
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;
  document.body.appendChild(canvas);
  engine = new BABYLON.Engine(canvas, true, {
    xrCompatible: true
  });
} else {
  engine = new BABYLON.Engine(navigator.gl, true, {
    xrCompatible: true
  });
}
engine.disableUniformBuffers = true;
```

Firstly, we check if the `navigator.gl` is available, this is make our example work at classic browser. If `navigator.gl` doesn't exist, it means the browser is classic, and we need to create a canvas element to render the scene. Otherwise, we use the `navigator.gl` directly.

And `engine.disableUniformBuffers = true;` is necessary to make Babylon.js work with JSAR's asynchronous mixed-rendering.

## Create a scene

Next, you can start creating a scene with Babylon.js:

```js title="main.js"
function createScene(engine) {
  const scene = new BABYLON.Scene(engine);
  scene.useRightHandedSystem = false;
  scene.autoClear = false;
  scene.autoClearDepthAndStencil = false;
  scene.blockMaterialDirtyMechanism = true;
  scene.performancePriority = BABYLON.ScenePerformancePriority.Intermediate;

  scene.createDefaultSkybox();
  scene.createDefaultCamera(true);

  // Run the render loop
  engine.runRenderLoop(() => scene.render());
}
```

In the above code, we create a scene and set some properties and methods to initialize the scene:

- `useRightHandedSystem = false;` to use the left-handed system, to keep the same coordinate system as WebGL.
- `autoClear = false;` to disable the auto-clearing of the scene, because JSAR should not clear the framebuffer automatically by itself.
- `autoClearDepthAndStencil = false;` to disable the auto-clearing of the depth and stencil buffers.
- `blockMaterialDirtyMechanism = true;` to block the material dirty mechanism, to avoid the material recompilation.
- `performancePriority = BABYLON.ScenePerformancePriority.Intermediate;` to set the performance priority to intermediate, to keep the performance balance.
- `createDefaultSkybox();` to create a default skybox.
- `createDefaultCamera(true);` to create a default camera with the `true` parameter to make it active.

After the above setup, you can run the render loop to render the scene with the `engine.runRenderLoop()` method.

## Configuring for XR

JSAR is running for XR devices, so you also need to configure you scene for WebXR stereo rendering:

```js title="main.js"
async function createScene(engine) {
  // ...
  engine.runRenderLoop(() => scene.render());

  // Request an XR session
  const { baseExperience, renderTarget } = await scene.createDefaultXRExperienceAsync({
    disableDefaultUI: true,
    disablePointerSelection: true,
    disableNearInteraction: true,
    disableHandTracking: true,
    disableTeleportation: true,
    outputCanvasOptions: {
      renderingContext: navigator.gl,
    },
  });
  await baseExperience.enterXRAsync('immersive-ar', 'local', renderTarget, {
    optionalFeatures: [],
  });
  return scene;
}
```

We use the `createDefaultXRExperienceAsync()` method to create an XR experience helper for the scene, and then use the `enterXRAsync()` method to enter the XR session with the `immersive-ar` mode and `local` reference space.

The above used 2 methods are implemented by Babylon.js itself.

## Bootstrap your scene

Finally, you can bootstrap your scene by calling the `createScene()` method:

```js title="main.js"
createScene(engine).then((scene) => {
  scene.createDefaultLight();

  const mat = new BABYLON.StandardMaterial('mat', scene);
  mat.diffuseColor = new BABYLON.Color3(1, 1, 1);
  const box = BABYLON.MeshBuilder.CreateBox('box', { size: 0.5 }, scene);
  box.material = mat;
});
```

In the above code, we create a default light for the scene, and then create a standard material and a box mesh to show in the scene. You can also use other features of Babylon.js to create your own.

## Multiview rendering

Multiview rendering in [OpenGLES][] is a technique to render the stereo views of a XR scene in a single draw call, which is more efficient than rendering each view separately.

Even though multiview rendering has been supported by Babylon.js, but the multiview rendering in JSAR is a bit different, because the library or framework is not responsible for creating the stereo framebuffer and its compositing. Instead, the host created the stereo framebuffer and Babylon.js and your code just need to render virtual objects into the framebuffer only, and the host will composite the stereo views finally.

To make Babylon.js work, you must import the `babylonjs` library built by JSAR:

```html
<script type="importmap">
  {
    "imports": {
      "babylonjs": "http://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/babylon/umd/babylonjs/babylon.js"
    }
  }
</script>
```

## Using Babylon.js in XSML

XSML is built based on Babylon.js, so you are able to use Babylon.js in XSML directly:

```html title="index.xsml"
<xsml>
<head>
  <title>Using with Babylon.js</title>
  <style>
    space {
      rotation: 0 180 0;
    }
  </style>
</head>
<space>
  <script type="module">
    const scene: BABYLON.Scene = spatialDocument.scene;
    // ...
    // Write your Babylon.js code here
  </script>
</space>
</xsml>
```

As the above code shows, in XSML scripting, you can use the `spatialDocument.scene` to get a created Babylon.js scene , and then you can write your Babylon.js code directly in the script tag.

See [jsar-dom:fixtures/using-babylonjs](https://github.com/M-CreativeLab/jsar-dom/tree/main/fixtures/using-babylonjs) for more examples to use Babylon.js in XSML.

[Babylon.js]: https://www.babylonjs.com/
[OpenGLES]: https://www.khronos.org/opengles/
