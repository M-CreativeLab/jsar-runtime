import * as THREE from 'three';

var camera, scene, renderer;
var container = {
  width: 0,
  height: 0
};

async function init(cb) {
  container.width = window.innerWidth;
  container.height = window.innerHeight;

  camera = new THREE.PerspectiveCamera(65, container.width / container.height, 1, 2000);
  camera.position.z = 2000;
  camera.position.y = 400;
  camera.lookAt(new THREE.Vector3(0, 0, 0));

  scene = new THREE.Scene();

  const gl = navigator.gl;
  const session = await navigator.xr.requestSession('immersive-ar', {});
  const baseLayer = new XRWebGLLayer(session, gl);
  session.updateRenderState({ baseLayer });

  renderer = new THREE.WebGLRenderer({
    canvas: {
      addEventListener() { },
    },
    context: gl,
  });
  renderer.shadowMap.enabled = true;
  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  try {
    cb();
  } catch (err) {
    console.error('Error during initialization:', err);
  }

  // Start rendering
  renderer.setAnimationLoop(() => renderer.render(scene, camera));
}

init(async () => {
  const url = new URL(window.location.href);
  const modulePath = url.searchParams.get('module');

  let fn = await import(`./three/${modulePath}.js`);
  if (typeof fn !== 'function' && fn.default) {
    fn = fn.default;
  }
  if (typeof fn !== 'function') {
    console.error('Module does not export a function:', modulePath, fn);
  } else {
    fn({ scene, camera, renderer });
  }
});
