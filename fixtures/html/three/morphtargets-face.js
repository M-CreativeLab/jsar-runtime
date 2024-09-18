import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { KTX2Loader } from 'three/addons/loaders/KTX2Loader.js';
import { MeshoptDecoder } from 'three/addons/libs/meshopt_decoder.module.js';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);

// Create lights
const light = new THREE.DirectionalLight(0xffffff, 0.5);
light.position.set(0, 1, 1);
scene.add(light);

let mixer;
const clock = new THREE.Clock();

// Create meshes
{
  function init() {
    const ktx2Loader = new KTX2Loader()
      .setTranscoderPath('https://threejs.org/examples/jsm/libs/basis/');

    new GLTFLoader()
      .setKTX2Loader(ktx2Loader)
      .setMeshoptDecoder(MeshoptDecoder)
      .load('https://threejs.org/examples/models/gltf/facecap.glb', (gltf) => {
        console.info('Loaded:', gltf);
        const mesh = gltf.scene.children[0];
        scene.add(mesh);

        mixer = new THREE.AnimationMixer(mesh);
        mixer.clipAction(gltf.animations[0]).play();
      });
  }
  init();
}

const gl = navigator.gl;
navigator.xr.requestSession('immersive-ar', {}).then((session) => {
  const baseLayer = new XRWebGLLayer(session, gl);
  session.updateRenderState({ baseLayer });

  const renderer = new THREE.WebGLRenderer({
    canvas: {
      addEventListener() { },
    },
    context: gl,
  });
  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  function animate() {
    const delta = clock.getDelta();
    if (mixer) {
      mixer.update(delta);
    }
    renderer.render(scene, camera);
  }

  camera.position.z = 5;
  renderer.setAnimationLoop(animate);
  console.info('Started...');
}, (err) => {
  console.warn('Failed to start XR session:', err);
});
console.info('navigator.xr', navigator.xr);
