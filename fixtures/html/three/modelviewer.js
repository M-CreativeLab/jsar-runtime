import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';

function init() {

  function initScene() {
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);
    const light = new THREE.DirectionalLight(0xffffff, 1.5);
    light.position.set(0, 1, 1);
    scene.add(light);
    return { scene, camera };
  }

  function createLoadingBalls() {
    const loadingGroup = new THREE.Group();
    loadingGroup.name = 'loadingBalls';

    const estimatedScaleFactor = 0.01;
    const ballGeometry = new THREE.SphereGeometry(1, 32, 32);
    const ballMaterial = new THREE.MeshPhongMaterial({ color: 0x2ecc71 });

    for (let i = 0; i < 3; i++) {
      const ball = new THREE.Mesh(ballGeometry, ballMaterial);
      ball.position.set((i - 1) * 3, 0, 0);
      loadingGroup.add(ball);
    }

    loadingGroup.scale.setScalar(estimatedScaleFactor);
    loadingGroup.position.set(0, 0, 0);

    return loadingGroup;
  }

  function animateLoadingBallsScale(loadingGroup, duration, targetScale, onComplete) {
    if (!loadingGroup) {
      if (onComplete) onComplete();
      return;
    }

    const startTime = Date.now();
    const initialScale = loadingGroup.scale.x;
    const scaleRange = targetScale - initialScale;


    function updateScale() {
      const elapsed = Date.now() - startTime;
      const progress = Math.min(elapsed / duration, 1);

      // 使用easeInOutQuad缓动函数
      const easeProgress = progress < 0.5
        ? 2 * progress * progress
        : 1 - Math.pow(-2 * progress + 2, 2) / 2;

      const currentScale = initialScale + scaleRange * easeProgress;
      loadingGroup.scale.setScalar(currentScale);

      if (progress < 1) {
        requestAnimationFrame(updateScale);
      } else {
        loadingGroup.scale.setScalar(targetScale);
        if (onComplete) onComplete();
      }
    }

    requestAnimationFrame(updateScale);
  }

  function animateLoadingBalls(loadingGroup) {
    if (!loadingGroup || !loadingGroup.visible) return;

    const time = Date.now() * 0.005;
    const amplitude = 1;

    loadingGroup.children.forEach((ball, i) => {
      ball.position.y = Math.sin(time + i * 0.8) * amplitude;
    });
  }

  function removeLoadingBalls(scene, callback) {
    const loadingGroup = scene.getObjectByName('loadingBalls');
    if (!loadingGroup) {
      if (callback) callback();
      return;
    }

    animateLoadingBallsScale(loadingGroup, 500, 0, () => {
      loadingGroup.visible = false;
      scene.remove(loadingGroup);

      // 清理资源
      loadingGroup.children.forEach(ball => {
        if (ball.geometry) ball.geometry.dispose();
        if (ball.material) ball.material.dispose();
      });

      if (callback) callback();
    });
  }

  function loadModel(url, onLoaded) {
    if (!url) {
      if (onLoaded) onLoaded(new Error('model url is null'));
      return;
    }

    const loader = new GLTFLoader();

    loader.load(
      url,
      (gltf) => {

        // 移除加载动画
        removeLoadingBalls(scene, () => {
          setTimeout(() => {
            processLoadedModel(gltf);
            if (onLoaded) onLoaded();
          }, 100);
        });
      },
      null,
      (error) => {
        removeLoadingBalls(scene, () => {
          if (onLoaded) onLoaded(error);
        });
      }
    );
  }

  function processLoadedModel(gltf) {
    const parentGroup = new THREE.Group();
    const model = gltf.scene;

    model.traverse((node) => {
      if (node.isMesh) {
        node.raycast = () => { };
      }
    });

    parentGroup.add(model);
    parentGroup.name = rotateRootName;

    setupModelAnimations(gltf, model);

    setupModelTransform(model, parentGroup);

    group.add(parentGroup);
  }

  function setupModelAnimations(gltf, model) {
    if (gltf.animations && gltf.animations.length > 0) {
      animationControl.mixer = new THREE.AnimationMixer(model);
      animationControl.animations = gltf.animations.map(anim => ({
        name: anim.name,
        action: animationControl.mixer.clipAction(anim),
        duration: anim.duration
      }));

      if (animationControl.animations.length > 0 && isPlayAnimation) {
        playAnimation(animationControl, 0);
      }
    }
  }

  function setupModelTransform(model, parentGroup) {
    const box = new THREE.Box3().setFromObject(model);
    const size = box.getSize(new THREE.Vector3());
    const center = box.getCenter(new THREE.Vector3());

    model.position.sub(center);

    const collisionBox = new THREE.Mesh(
      new THREE.BoxGeometry(size.x, size.y, size.z),
      new THREE.MeshBasicMaterial({ color: 0xffff00, visible: false })
    );
    parentGroup.add(collisionBox);

    const scaleFactor = 1.0 / (Math.max(size.x, size.y, size.z) * 5);
    parentGroup.scale.setScalar(scaleFactor);
    parentGroup.position.set(0, 0, 0);

  }

  function playAnimation(animationControl, index) {
    if (!animationControl.mixer || animationControl.animations.length === 0) {
      return;
    }

    if (animationControl.currentAnimation) {
      animationControl.currentAnimation.action.stop();
    }

    const anim = animationControl.animations[index];
    anim.action.reset();
    anim.action.clampWhenFinished = false;
    anim.action.setLoop(THREE.LoopRepeat);
    anim.action.play();

    animationControl.currentAnimation = anim;
    animationControl.isPlaying = true;

  }

  function getXRController(renderer, index, scene, isShowLine) {
    const controller = renderer.xr.getController(index);
    scene.add(controller);

    if (isShowLine) {
      const geometry = new THREE.BufferGeometry().setFromPoints([
        new THREE.Vector3(0, 0, 0),
        new THREE.Vector3(0, 0, -1)
      ]);
      const line = new THREE.Line(geometry);
      line.scale.z = 5;
      controller.add(line.clone());
    }

    return controller;
  }

  function startXRSession() {
    if (!navigator.xr) {
      return;
    }

    navigator.xr.requestSession('immersive-ar', {})
      .then((session) => {
        setupXRSession(session);
      })
      .catch((err) => {
        console.error('Failed to start XR session:', err);
      });
  }

  function setupXRSession(session) {
    const renderer = gl
      ? new THREE.WebGLRenderer({ context: gl })
      : new THREE.WebGLRenderer({ antialias: true });

    renderer.xr.enabled = true;
    renderer.xr.setReferenceSpaceType('local');

    if (gl) {
      const baseLayer = new XRWebGLLayer(session, gl);
      session.updateRenderState({ baseLayer });
    }

    const controller = xrControl.selectingController = getXRController(
      renderer, controllerIndex, scene, isShowLine
    );

    session.addEventListener('selectstart', onSelectStart(controller, xrControl, group));
    session.addEventListener('selectend', () => {
      xrControl.isDragging = false;
      xrControl.selectedObject = null;
    });
    session.addEventListener('end', () => cleanup(renderer, scene));

    renderer.xr.setSession(session);

    renderer.setAnimationLoop(() => {
      if (animationControl.mixer && animationControl.isPlaying) {
        const delta = animationControl.clock.getDelta();
        animationControl.mixer.update(delta);
      }

      handleControllerSelect(controller, xrControl);

      const loadingGroup = scene.getObjectByName('loadingBalls');
      if (loadingGroup && loadingGroup.visible) {
        animateLoadingBalls(loadingGroup);
      }

      renderer.render(scene, camera);
    });
  }

  function onSelectStart(controller, xrControl, group) {
    return (evt) => {
      const { raycaster } = xrControl;
      const intersects = getIntersections(controller, raycaster, group);

      if (intersects.length > 0) {
        const position = getControllerPosition(controller);
        xrControl.selectedModel = intersects[0].object.parent ?? intersects[0].object;
        xrControl.isDragging = true;
        xrControl.controllerPrev.copy(position);
      }
    };
  }

  function handleControllerSelect(controller, xrControl) {
    const { selectedModel, isDragging, controllerPrev } = xrControl;

    if (isDragging && selectedModel) {
      const currentPosition = getControllerPosition(controller);
      const delta = currentPosition.clone().sub(controllerPrev);

      delta.multiplyScalar(xrControl.rotationSpeed);
      delta.set(delta.x, -delta.y, 0);

      applyCameraRelativeRotation(selectedModel, delta);
      xrControl.controllerPrev.copy(currentPosition);
    }
  }

  /**
   * 
   * @param {THREE.XRTargetRaySpace} controller 
   * @param {THREE.Raycaster} raycaster 
   * @param {THREE.Group} group 
   * @returns {Array} 
   */
  function getIntersections(controller, raycaster, group) {
    if (!controller) return [];

    controller.updateMatrixWorld();
    raycaster.setFromXRController(controller);
    return raycaster.intersectObjects(group.children, true);
  }

  function getControllerPosition(controller) {
    if (!controller) {
      return new THREE.Vector3();
    }

    controller.updateMatrixWorld();
    const position = new THREE.Vector3();
    position.setFromMatrixPosition(controller.matrixWorld);
    return position;
  }


  function applyCameraRelativeRotation(model, deltaMove, rotationSpeed = 0.01) {
    if (!model || !deltaMove) {
      return;
    }

    if (!deltaMove.x && !deltaMove.y) {
      return;
    }

    const quaternionY = new THREE.Quaternion();
    const quaternionX = new THREE.Quaternion();

    quaternionY.setFromAxisAngle(new THREE.Vector3(0, 1, 0), deltaMove.x * rotationSpeed);

    const currentEuler = new THREE.Euler();
    currentEuler.setFromQuaternion(model.quaternion, 'YXZ');

    const newXRotation = currentEuler.x + deltaMove.y * rotationSpeed;
    const clampedXRotation = Math.max(-Math.PI / 2, Math.min(Math.PI / 2, newXRotation));

    quaternionX.setFromAxisAngle(
      new THREE.Vector3(1, 0, 0),
      clampedXRotation - currentEuler.x
    );

    model.quaternion.multiplyQuaternions(quaternionY, model.quaternion);
    model.quaternion.multiplyQuaternions(model.quaternion, quaternionX);
  }


  function cleanup(renderer, scene) {

    renderer.setAnimationLoop(null);

    scene.traverse(object => {
      if (object.geometry) object.geometry.dispose();
      if (object.material) {
        if (Array.isArray(object.material)) {
          object.material.forEach(material => material.dispose());
        } else {
          object.material.dispose();
        }
      }
    });

  }

  const modelUrl = 'https://rokid-xr.oss-cn-hangzhou.aliyuncs.com/dev/RokidTerminalMuseum/Apple_II_Computer1.glb';
  const isPlayAnimation = false

  if (!modelUrl) {
    return;
  }


  const rotateRootName = 'rotate-component-container';
  const isShowLine = false;
  const controllerIndex = 1;
  const gl = navigator.gl;

  const { scene, camera } = initScene();
  const group = new THREE.Group();
  scene.add(group);

  const animationControl = {
    mixer: null,
    animations: [],
    clock: new THREE.Clock(),
    currentAnimation: null,
    isPlaying: false
  };

  const xrControl = {
    selectedObject: null,
    selectingController: null,
    isSelecting: false,
    selectedModel: null,
    isDragging: false,
    isMouseDown: false,
    controllerPrev: new THREE.Vector3(),
    raycaster: new THREE.Raycaster(),
    rotationSpeed: 3000.0,
  };

  const loadingGroup = createLoadingBalls();
  group.add(loadingGroup);
  loadingGroup.visible = true;

  startXRSession();

  loadModel(modelUrl, (err) => {
    if (err) {
      console.error('load model error', err);
      removeLoadingBalls(scene, () => {
        console.log('load model error');
      });
    } else {
      console.log('load model success');
    }
  });
}

try {
  init();
} catch (error) {
  console.error('init error', error);
}