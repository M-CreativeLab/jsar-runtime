# Hello world with Three.js

This tutorial will guide you through creating a simple 3D scene using Three.js in JSAR.

## Step 1: Create a new HTML file

Create a new HTML file and add the following code:

```html
<html>
<head>
  <meta charset="utf-8">
  <title>My first three.js app</title>
</head>
<body>
  <script type="module" src="/main.js"></script>
</body>
</html>
```

## Step 2: Specifiy the Three.js `importmap`

Specify the Three.js `importmap` in the HTML file:

```html
<html>
<head>
  <meta charset="utf-8">
  <title>My first three.js app</title>
  <script type="importmap">
    {
      "imports": {
        "three": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/build/three.module.js",
        "three/addons/": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/examples/jsm/"
      }
    }
  </script>
</head>
<body>
  <!-- ... -->
</body>
</html>
```

Note that, to use Three.js in JSAR's multiview stereo rendering, you will need to use the Three.js version provided by JSAR as shown above.

## Step 3: Create a Three.js scene

Create a new file named `main.js` and add the following code:

```js
import * as THREE from 'three';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 );

const renderer = new THREE.WebGLRenderer({
  canvas: { // Mock canvas element
    addEventListener: () => {},
  },
  context: navigator.gl,
});
```

Note that, in JSAR, the `canvas` element is not available in the `WebGLRenderer` constructor. Instead, you can use the `navigator.gl` object to get the WebGL context.

## Step 4: Add a cube to the scene

Next, add a cube to the scene:

```js
const geometry = new THREE.BoxGeometry();
const material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } );
const cube = new THREE.Mesh( geometry, material );
scene.add(cube);
```

## Step 5: Fit the size

After you have initialized the scene, it's better to fit the scene size to the proper size, like this:

```js
function fitTo(scene, targetSize = 1) {
  const box = new THREE.Box3();
  scene.traverse(object => {
    if (object instanceof THREE.Mesh || object instanceof THREE.Group) {
      box.expandByObject(object);
    }
  });
  const size = box.getSize(new THREE.Vector3());
  const scale = targetSize / Math.max(size.x, size.y, size.z);
  scene.scale.set(scale, scale, scale);
}
```

Then, call the `fitTo` function after you have added the cube to the scene:

```js
fitTo(scene, 0.3);
```

The above code will fit the scene to the size of 0.3 meters in the host world space.

## Step 6: Create XR content

Next, add the following code to request an XR session for your scene:

```js
// 1. Request an XR session
navigator.xr.requestSession('immersive-ar', {}).then((session) => {

  // 2. Create a XRWebGLLayer and set it to the session
  const baseLayer = new XRWebGLLayer(session, navigator.gl);
  session.updateRenderState({ baseLayer });

  // 3. Configure the renderer for the XR session
  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  // 4. Write the animation callback
  renderer.setAnimationLoop(() => {
    renderer.render(scene, camera);
  });
});
```

## Step 7: Run the app

Now you can run the app in the JSAR environment, follow the instructions in the [Quick Start / Run](/quick-start/run) guide to run the HTML in your device.

## Summary

The following is the complete code for the HTML and JavaScript files

```js title="index.html"
<html>
<head>
  <meta charset="utf-8">
  <title>My first three.js app</title>
  <script type="importmap">
    {
      "imports": {
        "three": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/build/three.module.js",
        "three/addons/": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/examples/jsm/"
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
import * as THREE from 'three';

// Create scene
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

// Create renderer
const renderer = new THREE.WebGLRenderer({
  canvas: { // Mock canvas element
    addEventListener: () => {},
  },
  context: navigator.gl,
});

// Initialize your scene
const geometry = new THREE.BoxGeometry();
const material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } );
const cube = new THREE.Mesh( geometry, material );
scene.add(cube);

// Fit the scene to the proper size
function fitTo(scene, targetSize = 1) {
  const box = new THREE.Box3();
  scene.traverse(object => {
    if (object instanceof THREE.Mesh || object instanceof THREE.Group) {
      box.expandByObject(object);
    }
  });
  const size = box.getSize(new THREE.Vector3());
  const scale = targetSize / Math.max(size.x, size.y, size.z);
  scene.scale.set(scale, scale, scale);
}
fitTo(scene, 0.3);

// Request an XR session for initialized and fitted scene
navigator.xr.requestSession('immersive-ar', {}).then((session) => {
  const baseLayer = new XRWebGLLayer(session, navigator.gl);
  session.updateRenderState({ baseLayer });

  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  renderer.setAnimationLoop(() => {
    renderer.render(scene, camera);
  });
});
```
