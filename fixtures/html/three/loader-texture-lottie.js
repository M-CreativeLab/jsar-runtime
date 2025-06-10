import * as THREE from 'three';
import { RoomEnvironment } from 'three/addons/environments/RoomEnvironment.js';
import { RoundedBoxGeometry } from 'three/addons/geometries/RoundedBoxGeometry.js';
import { LottieLoader } from 'three/addons/loaders/LottieLoader.js';
import lottie from 'https://cdn.jsdelivr.net/npm/lottie-web@5.12.2/+esm';

export default async function ({ scene, renderer }) {
  const loader = new THREE.FileLoader();
  loader.setResponseType('json');
  loader.load('https://threejs.org/examples/textures/lottie/24017-lottie-logo-animation.json', function (data) {
    const container = document.createElement('div');
    container.style.width = data.w + 'px';
    container.style.height = data.h + 'px';
    document.body.appendChild(container);

    const animation = lottie.loadAnimation({
      container: container,
      animType: 'canvas',
      loop: true,
      autoplay: true,
      animationData: data,
      rendererSettings: { dpr: 1 }
    });

    const texture = new THREE.CanvasTexture(animation.container);
    texture.minFilter = THREE.NearestFilter;
    texture.generateMipmaps = false;
    texture.colorSpace = THREE.SRGBColorSpace;

    animation.addEventListener('enterFrame', function () {
      texture.needsUpdate = true;
    });
    container.style.display = 'none'; // must be done after loadAnimation() otherwise canvas has 0 dimensions

    const geometry = new RoundedBoxGeometry(1, 1, 1, 7, 0.2);
    const material = new THREE.MeshStandardMaterial({ roughness: 0.1, map: texture });
    mesh = new THREE.Mesh(geometry, material);
    scene.add(mesh);

    const environment = new RoomEnvironment();
    const pmremGenerator = new THREE.PMREMGenerator(renderer);
    scene.environment = pmremGenerator.fromScene(environment).texture;
  });
}
