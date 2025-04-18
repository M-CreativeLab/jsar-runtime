import { Model3dViewerOptions } from "../model3d";


export default function getTemplate(modelUrl, options: Model3dViewerOptions = {}): string {
  const playAnimation = options.playAnimation;
  return `
<html>

<head>
  <script type="importmap">
    {
      "imports": {
        "three": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/build/three.module.js",
        "three/addons/": "https://cdn.jsdelivr.net/npm/three@latest/examples/jsm/"
      }
    }
    </script>
  <script type="module">
    import * as  THREE from 'three';
    import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
    const rotateRootName = 'rotate-component-container';
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(75, 1.0, 0.1, 1000);
    const isShowLine = false;
    const isPlayAnimation = ${playAnimation};
    const controllerIndex = 1;//gl ? 1 : 1;
    const rotationSpeed = 15.0;
    const enableSingleAxisRotation = false;

    // Create lights
    const light = new THREE.DirectionalLight(0xffffff, 1.5);
    light.position.set(0, 1, 1);
    scene.add(light);

    // 全局变量
    const xrControl = {
      selectedObject: null,
      selectingController: null,
      isSelecting: false,
      initialControllerPosition: new THREE.Vector3(),
      initialControllerQuaternion: new THREE.Quaternion(),
      initialObjectQuaternion: new THREE.Quaternion(),
      raycaster: new THREE.Raycaster(),
      // 添加可重用对象
      _deltaPosition: new THREE.Vector3(),
      _quaternionX: new THREE.Quaternion(),
      _quaternionY: new THREE.Quaternion(),
      _targetQuaternion: new THREE.Quaternion(),
      _euler: new THREE.Euler(),
      _initialEuler: new THREE.Euler(),
    };

    // 动画相关变量
    const animationControl = {
      mixer: null,
      animations: [],
      clock: new THREE.Clock(),
      currentAnimation: null,
      isPlaying: false
    };

    // 可选择物体
    const group = new THREE.Group();
    scene.add(group);

    loadMesh('${modelUrl}');

    function loadMesh(url) {
      const onLoad = (gltf) => {
        console.info('Loaded GLTF scene.');
        // 创建父容器
        const parentGroup = new THREE.Group();
        // 添加原始模型
        const model = gltf.scene;

        // model.scale.set(0.01, 0.01, 0.01);
        parentGroup.add(model);
        parentGroup.name = rotateRootName;

        // 处理动画
        if (gltf.animations && gltf.animations.length > 0) {
          animationControl.mixer = new THREE.AnimationMixer(model);
          animationControl.animations = gltf.animations.map(anim => {
            const action = animationControl.mixer.clipAction(anim);
            return {
              name: anim.name,
              action: action,
              duration: anim.duration
            };
          });

          // 自动播放第一个动画
          if (animationControl.animations.length > 0 && isPlayAnimation) {
            playAnimation(0);
          }
        }

        // 计算包围盒并创建碰撞盒
        let box = new THREE.Box3().setFromObject(model);
        let size = box.getSize(new THREE.Vector3());
        const center = box.getCenter(new THREE.Vector3());
        model.position.sub(center); // 将模型移到以几何中心为原点

        // 创建可交互的碰撞盒几何体
        const boxGeometry = new THREE.BoxGeometry(size.x, size.y, size.z);
        const boxMaterial = new THREE.MeshBasicMaterial({
          color: 0xffff00,
          visible: false
        });
        const collisionBox = new THREE.Mesh(boxGeometry, boxMaterial);
        // collisionBox.position.copy(center);

        // 将碰撞盒和原始模型添加到父容器
        parentGroup.add(collisionBox);

        // 设置父容器位置
        const scaleFactor = 1.0 / (Math.max(size.x, size.y, size.z) * 5);
        parentGroup.scale.set(scaleFactor, scaleFactor, scaleFactor);
        // parentGroup.scale.set(1 / size.x, 1 / size.y, 1 / size.z);

        // 添加交互属性（重要！）
        collisionBox.userData.isInteractive = true;
        parentGroup.userData.isInteractive = true;

        parentGroup.position.set(0, 0, 0);

        // 添加到场景并注册为可选项
        group.add(parentGroup);
        // group.add(collisionBox);
      }
      const onProgress = ({ loaded, total }) => {
        console.log('加载进度:', loaded / total * 100 + '%');
      };
      const onError = (error) => {
        console.error('加载失败:', error);
        if (error instanceof Error) {
          console.error('错误详情:', error.message);
        }
      }
      const loader = new GLTFLoader();
      loader.load(url,
        onLoad,
        onProgress,
        onError
      );
    }

    // 播放指定索引的动画
    function playAnimation(index) {
      if (!animationControl.mixer || animationControl.animations.length === 0) return;

      // 停止当前播放的动画
      if (animationControl.currentAnimation) {
        animationControl.currentAnimation.action.stop();
      }

      // 播放新动画
      const anim = animationControl.animations[index];
      anim.action.reset();
      anim.action.clampWhenFinished = false;
      anim.action.setLoop(THREE.LoopRepeat);
      anim.action.play();

      animationControl.currentAnimation = anim;
      animationControl.isPlaying = true;
    }

    const gl = navigator.gl;
    navigator.xr.requestSession('immersive-ar', {

    }).then((session) => {
      if (gl) {
        const baseLayer = new XRWebGLLayer(session, gl);
        session.updateRenderState({ baseLayer });
      }

      const renderer = gl ? new THREE.WebGLRenderer({
        canvas: {
          addEventListener() { },
        },
        context: gl,
      }) : new THREE.WebGLRenderer({ antialias: true });
      renderer.xr.enabled = true;
      renderer.xr.setReferenceSpaceType('local');

      const controller = xrControl.selectingController = getXRController(renderer, controllerIndex);
      session.addEventListener('selectstart', onSelectStart(controller));
      session.addEventListener('selectend', onSelectEnd);
      session.addEventListener('end', cleanup);
      renderer.xr.setSession(session);


      // camera.position.z = -5;
      console.info('Started...');
      // 渲染循环
      renderer.setAnimationLoop(() => {
        // 更新动画混合器
        if (animationControl.mixer && animationControl.isPlaying) {
          const delta = animationControl.clock.getDelta();
          animationControl.mixer.update(delta);
        }

        handleIntersection();
        renderer.render(scene, camera);
      });

      function onSelectStart(controller) {
        return (evt) => {
          const { raycaster } = xrControl;
          const intersects = getIntersections(controller, raycaster, group);
          if (intersects.length > 0) {
            // Store the selected object and its initial rotation
            const obj = intersects[0].object;
            const selectObj = xrControl.selectedObject = obj.parent?.name === rotateRootName ? obj.parent : obj;
            xrControl.initialControllerPosition.setFromMatrixPosition(controller.matrixWorld);
            xrControl.initialObjectQuaternion.copy(selectObj.quaternion);
            xrControl.isSelecting = true;
          }
        }
      }
      function onSelectEnd(event) {
        xrControl.isSelecting = false;
        xrControl.selectedObject = null;
        // selectingController = null;
      }

      function getXRController(xrRenderer, controllerIndex) {
        let controller = xrRenderer.xr.getController(controllerIndex);
        scene.add(controller);
        // 添加射线可视化
        if (isShowLine) {
          const geometry = new THREE.BufferGeometry().setFromPoints([
            new THREE.Vector3(0, 0, 0),
            new THREE.Vector3(0, 0, -1)
          ]);
          const line = new THREE.Line(geometry);
          line.scale.z = 5;
          controller.add(line.clone());
        }
        // 事件处理
        // controller.addEventListener('selectstart', onSelectStart);
        // controller.addEventListener('selectend', onSelectEnd);
        return controller;
      }

      function getIntersections(controller, raycaster, group) {
        if (controller == null) {
          console.warn('getIntersections controller is null');
          return [];
        }
        controller.updateMatrixWorld();

        raycaster.setFromXRController(controller);

        return raycaster.intersectObjects(group.children, true);

      }

      function intersectObjects(controller, raycaster, group) {
        // Do not highlight in mobile-ar
        if (controller == null) return;
        if (controller.userData.targetRayMode === 'screen') return;
        // Do not highlight when already selected
        if (controller.userData.selected !== undefined) return;
        const intersections = getIntersections(controller, raycaster, group);
        const line = isShowLine ? controller.children.at(0) : null;//('line');
        if (intersections.length > 0) {
          const intersection = intersections[0];
          const point = intersection.point;
          const object = intersection.object;
          //改变line颜色
          if (line != null) {
            line.material.color.setHex(0x00ff00);
            line.scale.z = intersection.distance;
          }
          return point;
        } else {
          line?.material.color.setHex(0xffffff);
          // line.scale.z = 5;
          return null;
        }
      }

      function handleIntersection() {
        const {
          selectedObject, isSelecting, selectingController, raycaster,
          initialControllerPosition, initialObjectQuaternion,
          _deltaPosition, _quaternionX, _quaternionY, _targetQuaternion, _euler, _initialEuler
        } = xrControl;

        if (!isSelecting || !selectedObject) return;
        const point = controller.position;// intersectObjects(controller, raycaster, group);

        // 重用向量对象计算位置差
        _deltaPosition.copy(point).sub(initialControllerPosition);

        if (enableSingleAxisRotation) {
          // 计算X和Y轴的位移绝对值
          const absX = Math.abs(_deltaPosition.x);
          const absY = Math.abs(_deltaPosition.y);

          // 根据哪个轴偏移更大决定旋转哪个轴
          if (absX > absY) {
            // 只旋转Y轴
            const deltaRotationY = -_deltaPosition.x * rotationSpeed;
            _quaternionY.setFromAxisAngle(new THREE.Vector3(0, 1, 0), deltaRotationY);
            _targetQuaternion.copy(initialObjectQuaternion).multiply(_quaternionY);
          } else {
            // 只旋转X轴
            const deltaRotationX = -_deltaPosition.y * rotationSpeed;
            _quaternionX.setFromAxisAngle(new THREE.Vector3(1, 0, 0), -deltaRotationX);
            _targetQuaternion.copy(initialObjectQuaternion).multiply(_quaternionX);
          }
        } else {
          const deltaRotationX = _deltaPosition.y * rotationSpeed;// Math.min(maxRotation, _deltaPosition.y * rotationSpeed);
          const deltaRotationY = -_deltaPosition.x * rotationSpeed;// Math.min(maxRotation, -_deltaPosition.x * rotationSpeed);

          // 重用四元数对象
          _quaternionX.setFromAxisAngle(new THREE.Vector3(1, 0, 0), -deltaRotationX);
          _quaternionY.setFromAxisAngle(new THREE.Vector3(0, 1, 0), -deltaRotationY);

          // 重用目标四元数
          _targetQuaternion.copy(initialObjectQuaternion)
            .multiply(_quaternionY)
            .multiply(_quaternionX);
        }
        // 应用旋转
        // selectedObject.quaternion.copy(_targetQuaternion);
        selectedObject.quaternion.slerp(_targetQuaternion, 0.1);
      }


      // 添加清理函数
      function cleanup() {
        renderer.setAnimationLoop(null);
        scene.traverse(object => {
          if (object.geometry) object.geometry.dispose();
          if (object.material) object.material.dispose();
        });
      }
    }, (err) => {
      console.error('XR session failed to start:', {
        message: err.message,
        stack: err.stack
      });
    });
  </script>

</head>

</html>
`;
}