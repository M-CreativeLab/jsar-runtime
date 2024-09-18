import * as THREE from 'three';
import { OBB } from 'three/addons/math/OBB.js';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);

// Create lights
const light = new THREE.DirectionalLight(0xffffff, 0.5);
light.position.set(0, 1, 1);
scene.add(light);

const clock = new THREE.Clock();
const objects = [];
let raycaster, hitbox;

// Create meshes
{
  function init() {
    raycaster = new THREE.Raycaster();

    const size = new THREE.Vector3(10, 5, 6);
    const geometry = new THREE.BoxGeometry(size.x, size.y, size.z);

    // setup OBB on geometry level (doing this manually for now)
    geometry.userData.obb = new OBB();
    geometry.userData.obb.halfSize.copy(size).multiplyScalar(0.5);

    for (let i = 0; i < 100; i++) {
      const object = new THREE.Mesh(geometry, new THREE.MeshLambertMaterial({ color: 0x00ff00 }));
      object.matrixAutoUpdate = false;

      object.position.x = Math.random() * 80 - 40;
      object.position.y = Math.random() * 80 - 40;
      object.position.z = Math.random() * 80 - 40;

      object.rotation.x = Math.random() * 2 * Math.PI;
      object.rotation.y = Math.random() * 2 * Math.PI;
      object.rotation.z = Math.random() * 2 * Math.PI;

      object.scale.x = Math.random() + 0.5;
      object.scale.y = Math.random() + 0.5;
      object.scale.z = Math.random() + 0.5;

      scene.add(object);

      object.userData.obb = new OBB();
      objects.push(object);
    }
    hitbox = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({ color: 0x000000, wireframe: true }));
  }

  try {
    init();
  } catch (err) {
    console.error('Failed to initialize:', err);
  }
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
    for (let i = 0, il = objects.length; i < il; i++) {

      const object = objects[i];
      object.rotation.x += delta * Math.PI * 0.20;
      object.rotation.y += delta * Math.PI * 0.1;

      object.updateMatrix();
      object.updateMatrixWorld();

      // update OBB
      object.userData.obb.copy(object.geometry.userData.obb);
      object.userData.obb.applyMatrix4(object.matrixWorld);

      // reset
      object.material.color.setHex(0x00ff00);
    }

    // collision detection
    for (let i = 0, il = objects.length; i < il; i++) {
      const object = objects[i];
      const obb = object.userData.obb;

      for (let j = i + 1, jl = objects.length; j < jl; j++) {
        const objectToTest = objects[j];
        const obbToTest = objectToTest.userData.obb;

        // now perform intersection test
        if (obb.intersectsOBB(obbToTest) === true) {
          object.material.color.setHex(0xff0000);
          objectToTest.material.color.setHex(0xff0000);
        }
      }
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