import * as THREE from 'three';

export default async function ({ scene }) {
  const textures = getTexturesFromAtlasFile('https://threejs.org/examples/textures/cube/sun_temple_stripe.jpg', 6);
  const materials = [];
  for (let i = 0; i < 6; i++) {
    materials.push(new THREE.MeshBasicMaterial({ map: textures[i] }));
  }

  const skyBox = new THREE.Mesh(new THREE.BoxGeometry(1, 1, 2), materials);
  skyBox.geometry.scale(1, 1, -1);
  scene.add(skyBox);
}

function getTexturesFromAtlasFile(atlasImgUrl, tilesNum) {
  const textures = [];

  for (let i = 0; i < tilesNum; i++) {
    textures[i] = new THREE.Texture();
  }

  new THREE.ImageLoader()
    .load(atlasImgUrl, (image) => {
      let canvas, context;
      const tileWidth = image.height;

      for (let i = 0; i < textures.length; i++) {
        canvas = document.createElement('canvas');
        context = canvas.getContext('2d');
        canvas.height = tileWidth;
        canvas.width = tileWidth;
        context.drawImage(image, tileWidth * i, 0, tileWidth, tileWidth, 0, 0, tileWidth, tileWidth);

        textures[i].colorSpace = THREE.SRGBColorSpace;
        textures[i].image = canvas;
        textures[i].needsUpdate = true;
      }
    });
  return textures;

}