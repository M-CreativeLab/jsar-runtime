import * as THREE from 'three';
import * as CameraUtils from 'three/addons/utils/CameraUtils.js';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);

// Create lights
const light = new THREE.DirectionalLight(0xffffff, 0.5);
light.position.set(0, 1, 1);
scene.add(light);

let renderer;
let cameraControls;
let smallSphereOne, smallSphereTwo;
let portalCamera, leftPortal, rightPortal, leftPortalTexture, reflectedPosition,
  rightPortalTexture, bottomLeftCorner, bottomRightCorner, topLeftCorner;

// Mesh
{
  (function init() {
    const planeGeo = new THREE.PlaneGeometry(100.1, 100.1);

    // bouncing icosphere
    const portalPlane = new THREE.Plane(new THREE.Vector3(0, 0, 1), 0.0);
    const geometry = new THREE.IcosahedronGeometry(5, 0);
    const material = new THREE.MeshPhongMaterial({
      color: 0xffffff, emissive: 0x333333, flatShading: true,
      clippingPlanes: [portalPlane], clipShadows: true
    });
    smallSphereOne = new THREE.Mesh(geometry, material);
    scene.add(smallSphereOne);
    smallSphereTwo = new THREE.Mesh(geometry, material);
    scene.add(smallSphereTwo);

    // portals
    portalCamera = new THREE.PerspectiveCamera(45, 1.0, 0.1, 500.0);
    scene.add(portalCamera);
    //frustumHelper = new THREE.CameraHelper( portalCamera );
    //scene.add( frustumHelper );
    bottomLeftCorner = new THREE.Vector3();
    bottomRightCorner = new THREE.Vector3();
    topLeftCorner = new THREE.Vector3();
    reflectedPosition = new THREE.Vector3();

    leftPortalTexture = new THREE.WebGLRenderTarget(256, 256);
    leftPortal = new THREE.Mesh(planeGeo, new THREE.MeshBasicMaterial({ map: leftPortalTexture.texture }));
    leftPortal.position.x = - 30;
    leftPortal.position.y = 20;
    leftPortal.scale.set(0.35, 0.35, 0.35);
    scene.add(leftPortal);

    rightPortalTexture = new THREE.WebGLRenderTarget(256, 256);
    rightPortal = new THREE.Mesh(planeGeo, new THREE.MeshBasicMaterial({ map: rightPortalTexture.texture }));
    rightPortal.position.x = 30;
    rightPortal.position.y = 20;
    rightPortal.scale.set(0.35, 0.35, 0.35);
    scene.add(rightPortal);

    // walls
    const planeTop = new THREE.Mesh(planeGeo, new THREE.MeshPhongMaterial({ color: 0xffffff }));
    planeTop.position.y = 100;
    planeTop.rotateX(Math.PI / 2);
    scene.add(planeTop);

    const planeBottom = new THREE.Mesh(planeGeo, new THREE.MeshPhongMaterial({ color: 0xffffff }));
    planeBottom.rotateX(- Math.PI / 2);
    scene.add(planeBottom);

    const planeFront = new THREE.Mesh(planeGeo, new THREE.MeshPhongMaterial({ color: 0x7f7fff }));
    planeFront.position.z = 50;
    planeFront.position.y = 50;
    planeFront.rotateY(Math.PI);
    scene.add(planeFront);

    const planeBack = new THREE.Mesh(planeGeo, new THREE.MeshPhongMaterial({ color: 0xff7fff }));
    planeBack.position.z = - 50;
    planeBack.position.y = 50;
    //planeBack.rotateY( Math.PI );
    scene.add(planeBack);

    const planeRight = new THREE.Mesh(planeGeo, new THREE.MeshPhongMaterial({ color: 0x00ff00 }));
    planeRight.position.x = 50;
    planeRight.position.y = 50;
    planeRight.rotateY(- Math.PI / 2);
    scene.add(planeRight);

    const planeLeft = new THREE.Mesh(planeGeo, new THREE.MeshPhongMaterial({ color: 0xff0000 }));
    planeLeft.position.x = - 50;
    planeLeft.position.y = 50;
    planeLeft.rotateY(Math.PI / 2);
    scene.add(planeLeft);

    // lights
    const mainLight = new THREE.PointLight(0xe7e7e7, 2.5, 250, 0);
    mainLight.position.y = 60;
    scene.add(mainLight);

    const greenLight = new THREE.PointLight(0x00ff00, 0.5, 1000, 0);
    greenLight.position.set(550, 50, 0);
    scene.add(greenLight);

    const redLight = new THREE.PointLight(0xff0000, 0.5, 1000, 0);
    redLight.position.set(- 550, 50, 0);
    scene.add(redLight);

    const blueLight = new THREE.PointLight(0xbbbbfe, 0.5, 1000, 0);
    blueLight.position.set(0, 50, 550);
    scene.add(blueLight);

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
  })();
}

function renderPortal(thisPortalMesh, otherPortalMesh, thisPortalTexture) {
  // set the portal camera position to be reflected about the portal plane
  thisPortalMesh.worldToLocal(reflectedPosition.copy(camera.position));
  reflectedPosition.x *= - 1.0; reflectedPosition.z *= - 1.0;
  otherPortalMesh.localToWorld(reflectedPosition);
  portalCamera.position.copy(reflectedPosition);

  // grab the corners of the other portal
  // - note: the portal is viewed backwards; flip the left/right coordinates
  otherPortalMesh.localToWorld(bottomLeftCorner.set(50.05, - 50.05, 0.0));
  otherPortalMesh.localToWorld(bottomRightCorner.set(- 50.05, - 50.05, 0.0));
  otherPortalMesh.localToWorld(topLeftCorner.set(50.05, 50.05, 0.0));
  // set the projection matrix to encompass the portal's frame
  CameraUtils.frameCorners(portalCamera, bottomLeftCorner, bottomRightCorner, topLeftCorner, false);

  // render the portal
  thisPortalTexture.texture.colorSpace = renderer.outputColorSpace;
  renderer.setRenderTarget(thisPortalTexture);
  renderer.state.buffers.depth.setMask(true); // make sure the depth buffer is writable so it can be properly cleared, see #18897
  if (renderer.autoClear === false) renderer.clear();
  thisPortalMesh.visible = false; // hide this portal from its own rendering
  renderer.render(scene, portalCamera);
  thisPortalMesh.visible = true; // re-enable this portal's visibility for general rendering
}

const gl = navigator.gl;
navigator.xr.requestSession('immersive-ar', {}).then((session) => {
  const baseLayer = new XRWebGLLayer(session, gl);
  session.updateRenderState({ baseLayer });

  renderer = new THREE.WebGLRenderer({
    canvas: {
      addEventListener() { },
    },
    context: gl,
  });
  renderer.shadowMap.enabled = true;
  renderer.localClippingEnabled = true;
  renderer.toneMapping = THREE.ACESFilmicToneMapping;
  renderer.xr.enabled = true;
  renderer.xr.setReferenceSpaceType('local');
  renderer.xr.setSession(session);

  function animate() {
    // move the bouncing sphere(s)
    const timerOne = Date.now() * 0.01;
    const timerTwo = timerOne + Math.PI * 10.0;

    smallSphereOne.position.set(
      Math.cos(timerOne * 0.1) * 30,
      Math.abs(Math.cos(timerOne * 0.2)) * 20 + 5,
      Math.sin(timerOne * 0.1) * 30
    );
    smallSphereOne.rotation.y = (Math.PI / 2) - timerOne * 0.1;
    smallSphereOne.rotation.z = timerOne * 0.8;

    smallSphereTwo.position.set(
      Math.cos(timerTwo * 0.1) * 30,
      Math.abs(Math.cos(timerTwo * 0.2)) * 20 + 5,
      Math.sin(timerTwo * 0.1) * 30
    );
    smallSphereTwo.rotation.y = (Math.PI / 2) - timerTwo * 0.1;
    smallSphereTwo.rotation.z = timerTwo * 0.8;

    // save the original camera properties
    const currentRenderTarget = renderer.getRenderTarget();
    const currentXrEnabled = renderer.xr.enabled;
    const currentShadowAutoUpdate = renderer.shadowMap.autoUpdate;
    renderer.xr.enabled = false; // Avoid camera modification
    renderer.shadowMap.autoUpdate = false; // Avoid re-computing shadows

    // render the portal effect
    renderPortal(leftPortal, rightPortal, leftPortalTexture);
    renderPortal(rightPortal, leftPortal, rightPortalTexture);

    // restore the original rendering properties
    renderer.xr.enabled = currentXrEnabled;
    renderer.shadowMap.autoUpdate = currentShadowAutoUpdate;
    renderer.setRenderTarget(currentRenderTarget);

    // render the main scene
    renderer.render(scene, camera);
  }

  camera.position.z = 5;
  renderer.setAnimationLoop(animate);
}, (err) => {
  console.warn('Failed to start XR session:', err);
});
