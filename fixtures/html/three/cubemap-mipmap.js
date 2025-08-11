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

async function loadCubeTextureWithMipmaps() {
  const path = 'https://threejs.org/examples/textures/cube/angus/';
  const format = '.jpg';
  const mipmaps = [];
  const maxLevel = 8;

  async function loadCubeTexture(urls) {
    return new Promise(function (resolve) {
      new THREE.CubeTextureLoader().load(urls, function (cubeTexture) {
        resolve(cubeTexture);
      });
    });
  }

  // load mipmaps
  const pendings = [];

  for (let level = 0; level <= maxLevel; ++level) {
    const urls = [];
    for (let face = 0; face < 6; ++face) {
      urls.push(path + 'cube_m0' + level + '_c0' + face + format);
    }

    const mipmapLevel = level;
    pendings.push(loadCubeTexture(urls).then(function (cubeTexture) {
      mipmaps[mipmapLevel] = cubeTexture;
    }));
  }

  await Promise.all(pendings);

  const customizedCubeTexture = mipmaps.shift();
  customizedCubeTexture.mipmaps = mipmaps;
  customizedCubeTexture.colorSpace = THREE.SRGBColorSpace;
  customizedCubeTexture.minFilter = THREE.LinearMipMapLinearFilter;
  customizedCubeTexture.magFilter = THREE.LinearFilter;
  customizedCubeTexture.generateMipmaps = false;
  customizedCubeTexture.needsUpdate = true;
  return customizedCubeTexture;
}

init(async () => {
  const cubeTexture = await loadCubeTextureWithMipmaps();
  console.info('loaded cube texture', cubeTexture);

  //model
  const sphere = new THREE.SphereGeometry(0.5, 128, 128);

  //manual mipmaps
  let material = new THREE.MeshBasicMaterial({ color: 0xffffff, envMap: cubeTexture });
  material.name = 'manual mipmaps';

  let mesh = new THREE.Mesh(sphere, material);
  mesh.position.set(-0.5, 0, -1.5);
  scene.add(mesh);

  //webgl mipmaps
  material = material.clone();
  material.name = 'auto mipmaps';

  const autoCubeTexture = cubeTexture.clone();
  autoCubeTexture.mipmaps = [];
  autoCubeTexture.generateMipmaps = true;
  autoCubeTexture.needsUpdate = true;

  material.envMap = autoCubeTexture;

  mesh = new THREE.Mesh(sphere, material);
  mesh.position.set(0.5, 0, -1.5);
  scene.background = cubeTexture;
  scene.add(mesh);
});
