import * as THREE from 'three';
import { PDBLoader } from 'three/addons/loaders/PDBLoader.js';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);

// Create lights
const light = new THREE.DirectionalLight(0xffffff, 0.5);
light.position.set(0, 1, 1);
scene.add(light);

let root;
// Mesh
{
  const MOLECULES = {
    'Ethanol': 'ethanol.pdb',
    'Aspirin': 'aspirin.pdb',
    'Caffeine': 'caffeine.pdb',
    'Nicotine': 'nicotine.pdb',
    'LSD': 'lsd.pdb',
    'Cocaine': 'cocaine.pdb',
    'Cholesterol': 'cholesterol.pdb',
    'Lycopene': 'lycopene.pdb',
    'Glucose': 'glucose.pdb',
    'Aluminium oxide': 'Al2O3.pdb',
    'Cubane': 'cubane.pdb',
    'Copper': 'cu.pdb',
    'Fluorite': 'caf2.pdb',
    'Salt': 'nacl.pdb',
    'YBCO superconductor': 'ybco.pdb',
    'Buckyball': 'buckyball.pdb',
    'Graphite': 'graphite.pdb'
  };

  const params = {
    molecule: 'caffeine.pdb'
  };
  const loader = new PDBLoader();
  const offset = new THREE.Vector3();

  (function init() {
    root = new THREE.Group();
    scene.add(root);
    loadMolecule(params.molecule);
  })();

  function fitTo(targetSize = 1) {
    const box = new THREE.Box3();
    scene.traverse(object => {
      if (object instanceof THREE.Mesh || object instanceof THREE.Group) {
        box.expandByObject(object);
      }
    });
    const size = box.getSize(new THREE.Vector3());
    const scale = 1 / Math.max(size.x, size.y, size.z);
    console.info('Scaling scene by', scale);
    scene.scale.set(scale, scale, scale);
  }

  function loadMolecule(model) {
    const url = 'https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/examples/models/pdb/' + model;
    while (root.children.length > 0) {
      const object = root.children[0];
      object.parent.remove(object);
    }

    loader.load(url, function (pdb) {
      console.info('Loaded', pdb);
      const geometryAtoms = pdb.geometryAtoms;
      const geometryBonds = pdb.geometryBonds;

      const boxGeometry = new THREE.BoxGeometry(1, 1, 1);
      const sphereGeometry = new THREE.IcosahedronGeometry(1, 3);

      geometryAtoms.computeBoundingBox();
      geometryAtoms.boundingBox.getCenter(offset).negate();
      geometryAtoms.translate(offset.x, offset.y, offset.z);
      geometryBonds.translate(offset.x, offset.y, offset.z);

      let positions = geometryAtoms.getAttribute('position');
      const colors = geometryAtoms.getAttribute('color');
      const position = new THREE.Vector3();
      const color = new THREE.Color();

      for (let i = 0; i < positions.count; i++) {
        position.x = positions.getX(i);
        position.y = positions.getY(i);
        position.z = positions.getZ(i);

        color.r = colors.getX(i);
        color.g = colors.getY(i);
        color.b = colors.getZ(i);

        const material = new THREE.MeshPhongMaterial({ color: color });
        const object = new THREE.Mesh(sphereGeometry, material);
        object.position.copy(position);
        object.position.multiplyScalar(75);
        object.scale.multiplyScalar(25);
        root.add(object);
      }
      positions = geometryBonds.getAttribute('position');

      const start = new THREE.Vector3();
      const end = new THREE.Vector3();

      for (let i = 0; i < positions.count; i += 2) {
        start.x = positions.getX(i);
        start.y = positions.getY(i);
        start.z = positions.getZ(i);

        end.x = positions.getX(i + 1);
        end.y = positions.getY(i + 1);
        end.z = positions.getZ(i + 1);

        start.multiplyScalar(75);
        end.multiplyScalar(75);

        const object = new THREE.Mesh(boxGeometry, new THREE.MeshPhongMaterial({ color: 0xffffff }));
        object.position.copy(start);
        object.position.lerp(end, 0.5);
        object.scale.set(5, 5, start.distanceTo(end));
        object.lookAt(end);
        root.add(object);
      }
      fitTo(1);
    });
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
  renderer.shadowMap.enabled = true;
  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  function animate() {
    const time = Date.now() * 0.0004;
    root.rotation.x = time;
    root.rotation.y = time * 0.7;
    renderer.render(scene, camera);
  }

  camera.position.z = 5;
  renderer.setAnimationLoop(animate);
}, (err) => {
  console.warn('Failed to start XR session:', err);
});