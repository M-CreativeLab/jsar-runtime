import * as THREE from 'three';

const API = {
  offsetX: 0,
  offsetY: 0,
  repeatX: 0.25,
  repeatY: 0.25,
  rotation: Math.PI / 4, // positive is counterclockwise
  centerX: 0.5,
  centerY: 0.5
};

export default async function ({ scene, renderer }) {
  const geometry = new THREE.BoxGeometry(1, 1, 1);

  new THREE.TextureLoader().load('https://threejs.org/examples/textures/uv_grid_opengl.jpg', function (texture) {
    texture.wrapS = texture.wrapT = THREE.RepeatWrapping;
    texture.anisotropy = renderer.capabilities.getMaxAnisotropy();
    texture.colorSpace = THREE.SRGBColorSpace;
    //texture.matrixAutoUpdate = false; // default is true; set to false to update texture.matrix manually

    const material = new THREE.MeshBasicMaterial({ map: texture });
    const mesh = new THREE.Mesh(geometry, material);
    mesh.position.set(0, 0, -2);
    scene.add(mesh);
    updateUvTransform(mesh);

    setInterval(() => {
      mesh.rotation.y = Date.now() / 1000;
    }, 8);
  });
}

function updateUvTransform(mesh) {
  const texture = mesh.material.map;
  if (texture.matrixAutoUpdate === true) {
    texture.offset.set(API.offsetX, API.offsetY);
    texture.repeat.set(API.repeatX, API.repeatY);
    texture.center.set(API.centerX, API.centerY);
    texture.rotation = API.rotation; // rotation is around center
  } else {
    // setting the matrix uv transform directly
    texture.matrix.setUvTransform(API.offsetX, API.offsetY, API.repeatX, API.repeatY, API.rotation, API.centerX, API.centerY);
  }
}
