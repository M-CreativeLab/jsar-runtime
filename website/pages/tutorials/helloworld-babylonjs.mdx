# Hello world with Babylon.js

This tutorial will guide you through creating a simple 3D scene using Babylon.js in JSAR.

## Step 1: Create a new HTML file

Create a new HTML file and add the following code:

```html
<html>
<head>
  <meta charset="utf-8">
  <title>My first Babylon.js app</title>
</head>
<body>
  <script type="module" src="/main.js"></script>
</body>
</html>
```

## Step 2: Specifiy the Babylon.js `importmap`

Specify the Babylon.js `importmap` in the HTML file:

```html
<html>
<head>
  <meta charset="utf-8">
  <title>My first Babylon.js app</title>
  <script type="importmap">
    {
      "imports": {
        "babylonjs": "http://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/babylon/umd/babylonjs/babylon.js",
        "babylonjs-gltf-loader": "http://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/babylon/umd/babylonjs-loaders/babylon.glTFFileLoader.js"
      }
    }
  </script>
</head>
<body>
  <!-- ... -->
</body>
</html>
```

Note that, to use Babylon.js in JSAR's multiview stereo rendering, you will need to use the Babylon.js version provided by JSAR as shown above.

## Step 3: Create a Babylon.js scene

Create a new file named `main.js` and add the following code:

```js
import 'babylonjs';
import 'babylonjs-gltf-loader';

async function createScene(engine) {
  const scene = new BABYLON.Scene(engine);
  scene.autoClear = false;
  scene.autoClearDepthAndStencil = false;
  scene.blockMaterialDirtyMechanism = true;
  scene.performancePriority = BABYLON.ScenePerformancePriority.Intermediate;

  scene.createDefaultSkybox();
  scene.createDefaultCamera(true);

  // Run the render loop
  engine.runRenderLoop(() => scene.render());
  return scene;
}

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

createScene(engine).then(scene => {
  scene.createDefaultLight();

  // Your scene initialization code here
});
```

Note that, in JSAR, the `canvas` element is not available in the `WebGLRenderer` constructor. Instead, you can use the `navigator.gl` object to get the WebGL context.

## Step 4: Load a 3D model

Next, use `BABYLON.SceneLoader` to load a 3D model into the scene:

```js
createScene(engine).then(scene => {
  scene.createDefaultLight();

  // Append glTF model to scene.
  BABYLON.SceneLoader.Append("https://ar.rokidcdn.com/web-assets/pages/models/", "floating_fox.glb", scene, () => {
    // Loaded
    console.info('Model loaded', scene.meshes);
  });
});
```

## Step 5: Fit the size

After you have initialized the scene, it's better to fit the scene size to the proper size, like this:

```js
function fitTo(root, targetSize = 1) {
  const boundingVectors = root.getHierarchyBoundingVectors(true);
  const totalSize = boundingVectors.max.subtract(boundingVectors.min);
  const scalingFactor = Math.min(targetSize / totalSize.x, targetSize / totalSize.y, targetSize / totalSize.z);
  root.scaling.multiplyInPlace(new BABYLON.Vector3(scalingFactor, scalingFactor, scalingFactor));
}
```

Then, call the `fitTo` function after you have added the cube to the scene:

```js
fitTo(scene.rootNodes[0], 0.3);
```

The above code will fit the scene to the size of 0.3 meters in the host world space.

## Step 6: Create XR content

Unlike Three.js, you don't need to use `navigator.xr` directly to request an XR session. Instead, you should use the related Babylon.js APIs to create XR content. Here is an example:

```js
async function createScene(engine) {
  const scene = new BABYLON.Scene(engine);
  // ...

  // Create XR experience and enter XR
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

## Step 7: Run the app

Now you can run the app in the JSAR environment, follow the instructions in the [Quick Start / Run](/quick-start/run) guide to run the HTML in your device.

## Summary

The following is the complete code for the HTML and JavaScript files

```js title="index.html"
<html>
<head>
  <meta charset="utf-8">
  <title>My first Babylon.js app</title>
  <script type="importmap">
    {
      "imports": {
        "babylonjs": "http://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/babylon/umd/babylonjs/babylon.js",
        "babylonjs-gltf-loader": "http://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/babylon/umd/babylonjs-loaders/babylon.glTFFileLoader.js"
      }
    }
  </script>
</head>
<body>
  <script type="module" src="/main.js"></script>
</body>
</html>
```

```js title="main.js"
import 'babylonjs';
import 'babylonjs-gltf-loader';

async function createScene(engine) {
  const scene = new BABYLON.Scene(engine);
  scene.autoClear = false;
  scene.autoClearDepthAndStencil = false;
  scene.blockMaterialDirtyMechanism = true;
  scene.performancePriority = BABYLON.ScenePerformancePriority.Intermediate;

  scene.createDefaultSkybox();
  scene.createDefaultCamera(true);

  // Run the render loop
  engine.runRenderLoop(() => scene.render());

  // Create XR experience and enter XR
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

function fitTo(root, targetSize = 1) {
  const boundingVectors = root.getHierarchyBoundingVectors(true);
  const totalSize = boundingVectors.max.subtract(boundingVectors.min);
  const scalingFactor = Math.min(targetSize / totalSize.x, targetSize / totalSize.y, targetSize / totalSize.z);
  root.scaling.multiplyInPlace(new BABYLON.Vector3(scalingFactor, scalingFactor, scalingFactor));
}

createScene(engine).then(scene => {
  scene.createDefaultLight();

  // Append glTF model to scene.
  BABYLON.SceneLoader.Append("https://ar.rokidcdn.com/web-assets/pages/models/", "floating_fox.glb", scene, () => {
    // Loaded
    console.info('Model loaded', scene.meshes);
    fitTo(scene.rootNodes[0], 0.3);
  });
}).catch(err => {
  console.error(err);
});
```
