import * as THREE from 'three';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);

// Create lights
const light = new THREE.DirectionalLight(0xffffff, 0.5);
light.position.set(0, 1, 1);
scene.add(light);

const materials = [];
// Mesh
{
  (function init() {
    scene.fog = new THREE.FogExp2(0x000000, 0.0008);

    const geometry = new THREE.BufferGeometry();
    const vertices = [];
    const textureLoader = new THREE.TextureLoader();

    const assignSRGB = (texture) => {
      texture.colorSpace = THREE.SRGBColorSpace;
    };

    const sprite1 = textureLoader.load('https://threejs.org/examples/textures/sprites/snowflake1.png', assignSRGB);
    const sprite2 = textureLoader.load('https://threejs.org/examples/textures/sprites/snowflake2.png', assignSRGB);
    const sprite3 = textureLoader.load('https://threejs.org/examples/textures/sprites/snowflake3.png', assignSRGB);
    const sprite4 = textureLoader.load('https://threejs.org/examples/textures/sprites/snowflake4.png', assignSRGB);
    const sprite5 = textureLoader.load('https://threejs.org/examples/textures/sprites/snowflake5.png', assignSRGB);

    for (let i = 0; i < 10000; i++) {
      const x = Math.random() * 2000 - 1000;
      const y = Math.random() * 2000 - 1000;
      const z = Math.random() * 2000 - 1000;
      vertices.push(x, y, z);
    }
    geometry.setAttribute('position', new THREE.Float32BufferAttribute(vertices, 3));

    parameters = [
      [[1.0, 0.2, 0.5], sprite2, 20],
      [[0.95, 0.1, 0.5], sprite3, 15],
      [[0.90, 0.05, 0.5], sprite1, 10],
      [[0.85, 0, 0.5], sprite5, 8],
      [[0.80, 0, 0.5], sprite4, 5]
    ];

    for (let i = 0; i < parameters.length; i++) {
      const color = parameters[i][0];
      const sprite = parameters[i][1];
      const size = parameters[i][2];

      materials[i] = new THREE.PointsMaterial({ size: size, map: sprite, blending: THREE.AdditiveBlending, depthTest: false, transparent: true });
      materials[i].color.setHSL(color[0], color[1], color[2], THREE.SRGBColorSpace);

      const particles = new THREE.Points(geometry, materials[i]);
      particles.rotation.x = Math.random() * 6;
      particles.rotation.y = Math.random() * 6;
      particles.rotation.z = Math.random() * 6;
      scene.add(particles);
    }

    const box = new THREE.Box3();
    scene.traverse(object => {
      if (object instanceof THREE.Mesh || object instanceof THREE.Group || object instanceof THREE.Points) {
        box.expandByObject(object);
      }
    });
    const size = box.getSize(new THREE.Vector3());
    const scale = 1 / Math.max(size.x, size.y, size.z);
    console.info('Scaling scene by', scale);
    scene.scale.set(scale, scale, scale);
  })();
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
  renderer.shadowMap.enabled = true;
  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  function animate() {
    const time = Date.now() * 0.00005;

    for (let i = 0; i < scene.children.length; i++) {
      const object = scene.children[i];
      if (object instanceof THREE.Points) {
        object.rotation.y = time * (i < 4 ? i + 1 : - (i + 1));
      }
    }
    for (let i = 0; i < materials.length; i++) {
      const color = parameters[i][0];
      const h = (360 * (color[0] + time) % 360) / 360;
      materials[i].color.setHSL(h, color[1], color[2], THREE.SRGBColorSpace);
    }
    renderer.render(scene, camera);
  }

  camera.position.z = 5;
  renderer.setAnimationLoop(animate);
}, (err) => {
  console.warn('Failed to start XR session:', err);
});
