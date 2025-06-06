export default function createViewer(url: string) {
  return `
<html>
<head>
  <meta charset="utf-8" />
  <title>Simple HTML</title>
  <script type="importmap">
    {
      "imports": {
        "three": "https://unpkg.com/three@0.149.0/build/three.module.js",
        "three/examples/": "https://unpkg.com/three@0.149.0/examples/",
        "@splinetool/loader": "https://unpkg.com/@splinetool/loader@1.9.25/build/SplineLoader.js"
      }
    }
  </script>
  <script type="module">
    import * as THREE from 'three';
    import SplineLoader from '@splinetool/loader';
    console.info(SplineLoader);

    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);

    // Create lights
    const light = new THREE.DirectionalLight(0xffffff, 0.5);
    light.position.set(0, 1, 1);
    scene.add(light);

    const loader = new SplineLoader();
    loader.load(
      '${url}',
      (splineScene) => {
        console.info('Loaded Spline scene.', Object.keys(splineScene));
        scene.add(splineScene);

        const box = new THREE.Box3();
        scene.traverse(object => {
          if (object instanceof THREE.Mesh || object instanceof THREE.Group) {
            box.expandByObject(object);
          }
        });
        const size = box.getSize(new THREE.Vector3());
        const scale = 0.3 / Math.max(size.x, size.y, size.z);
        console.info('Scaling scene by', scale);
        scene.scale.set(scale, scale, scale);
      },
      null,
      (error) => {
        console.error('Failed to load Spline scene:', error?.stack || error?.message || error);
      }
    );

    const gl = navigator.gl;
    navigator.xr.requestSession('immersive-ar', {}).then((session) => {
      const baseLayer = new XRWebGLLayer(session, gl);
      session.updateRenderState({ baseLayer });

      const renderer = new THREE.WebGLRenderer({
        canvas: gl._screenCanvas,
        context: gl,
      });
      renderer.preserveDrawingBuffer = true;
      renderer.autoClear = false;
      renderer.xr.enabled = true;
      renderer.xr.setReferenceSpaceType('local');
      renderer.xr.setSession(session);

      function animate() {
        renderer.render(scene, camera);
      }

      camera.position.z = 5;
      renderer.setAnimationLoop(animate);
      console.info('Started...');
    }, (err) => {
      console.warn('Failed to start XR session:', err);
    });
  </script>
</head>
<body>
</body>
</html>
  `;
}
