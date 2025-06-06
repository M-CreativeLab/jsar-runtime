import * as THREE from 'three';
import { LightProbeHelper } from 'three/addons/helpers/LightProbeHelper.js';
import { LightProbeGenerator } from 'three/addons/lights/LightProbeGenerator.js';

var camera, scene, renderer;
var container = {
  width: 0,
  height: 0
};

// linear color space
const API = {
  lightProbeIntensity: 1.0,
  directionalLightIntensity: 0.6,
  envMapIntensity: 1
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
}

init(() => {
  const lightProbe = new THREE.LightProbe();
  scene.add(lightProbe);

  const directionalLight = new THREE.DirectionalLight(0xffffff, API.directionalLightIntensity);
  directionalLight.position.set(10, 10, 10);
  scene.add(directionalLight);

  const genCubeUrls = function (prefix, postfix) {
    return [
      prefix + 'px' + postfix, prefix + 'nx' + postfix,
      prefix + 'py' + postfix, prefix + 'ny' + postfix,
      prefix + 'pz' + postfix, prefix + 'nz' + postfix
    ];
  };
  const urls = genCubeUrls('https://threejs.org/examples/textures/cube/pisa/', '.png');
  console.log(urls);

  new THREE.CubeTextureLoader().load(urls, function (cubeTexture) {
    console.info('Loaded cube texture:', cubeTexture);
    try {
      // scene.background = cubeTexture;

      lightProbe.copy(LightProbeGenerator.fromCubeTexture(cubeTexture));
      lightProbe.intensity = API.lightProbeIntensity;
      lightProbe.position.set(- 10, 0, 0); // position not used in scene lighting calculations (helper honors the position, however)

      const geometry = new THREE.SphereGeometry(5, 64, 32);
      const material = new THREE.MeshStandardMaterial({
        color: 0xffffff,
        metalness: 0,
        roughness: 0,
        envMap: cubeTexture,
        envMapIntensity: API.envMapIntensity,
      });
      console.info(geometry, material);

      // mesh
      mesh = new THREE.Mesh(geometry, material);
      mesh.position.set(0, 0, -15);
      scene.add(mesh);

      // helper
      const helper = new LightProbeHelper(lightProbe, 1);
      scene.add(helper);
    } catch (err) {
      console.error('Error setting up scene:', err.stack);
    }
  }, (evt) => {
    console.info('Loading cube texture progress:', evt.loaded / evt.total * 100 + '%');
  }, (err) => {
    console.error('Error loading cube texture:', err);
  });

  function loop() {
    renderer.render(scene, camera);
  }
  renderer.setAnimationLoop(loop);
});
