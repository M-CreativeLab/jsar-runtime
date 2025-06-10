import * as THREE from 'three';
import { UltraHDRLoader } from 'three/addons/loaders/UltraHDRLoader.js';

export default async function ({ scene, renderer }) {
  const params = {
    autoRotate: true,
    metalness: 1.0,
    roughness: 0.0,
    exposure: 1.0,
    resolution: '2k',
    type: 'HalfFloatType'
  };
  renderer.toneMapping = THREE.ACESFilmicToneMapping;
  renderer.toneMappingExposure = params.exposure;

  const torusMesh = new THREE.Mesh(
    new THREE.TorusKnotGeometry(1, 0.4, 128, 128, 1, 3),
    new THREE.MeshStandardMaterial({ roughness: params.roughness, metalness: params.metalness })
  );
  scene.add(torusMesh);

  const loader = new UltraHDRLoader();
  loader.setDataType(THREE.FloatType);

  const loadEnvironment = function (resolution = '2k', type = 'HalfFloatType') {
    loader.setDataType(THREE[type]);
    loader.load(`https://threejs.org/examples/textures/equirectangular/spruit_sunrise_${resolution}.hdr.jpg`, function (texture) {
      texture.mapping = THREE.EquirectangularReflectionMapping;
      texture.needsUpdate = true;
      // scene.background = texture;
      scene.environment = texture;
    });
  };
  loadEnvironment(params.resolution, params.type);
}
