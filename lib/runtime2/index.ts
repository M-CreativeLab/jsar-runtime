import 'babylonjs';
import { GLTFFileLoader } from '@babylonjs/loaders/glTF';

import { requestRendererReady } from '../bindings/renderer';
import { markRuntimeAvailable } from '../bindings/env';
import * as logger from '../bindings/logger';
import { addXsmlRequestListener, type XsmlRequestEvent } from '../bindings/messaging';
import { createBondXRSystem, XRWebGLLayer } from '../webxr';

// register gltf loader
BABYLON.SceneLoader.RegisterPlugin(new GLTFFileLoader() as any);

export class TransmuteRuntime2 extends EventTarget {
  private gl: WebGLRenderingContext;
  private scene: BABYLON.Scene;
  private engine: BABYLON.Engine;
  private mainCamera: BABYLON.Camera;
  private defaultLight: BABYLON.Light;

  start() {
    requestRendererReady(this.onRendererReady.bind(this));
    markRuntimeAvailable([
      `version=${process.env['JSAR_VERSION']}`,
      `babylonjs=${BABYLON.Engine.Version}`,
      `nodejs=${process.versions.node}`,
      `v8=${process.versions.v8}`,
    ].join(','));
    addXsmlRequestListener(this.onXsmlRequest.bind(this));
  }

  private onRendererReady(gl: WebGLRenderingContext) {
    this.gl = gl;
    this.dispatchEvent(new Event('rendererReady'));
    this.prepare(gl);
    // this.appEntry(this.scene);

    // run loop
    // this.engine.runRenderLoop(() => {
    //   this.scene.render();
    // });
  }

  private onXsmlRequest(event: XsmlRequestEvent) {
    logger.info(`xsml request:`, event.url, event.sessionId);

    const xrSystem = createBondXRSystem(event.sessionId);
    xrSystem.requestSession('immersive-ar')
      .then(async session => {
        session.updateRenderState({
          baseLayer: new XRWebGLLayer(session, this.gl),
        });

        const localSpace = await session.requestReferenceSpace('local');
        const viewerSpace = await session.requestReferenceSpace('viewer');
        logger.info('[WebXR] local[0] & viewer[1] spaces:', localSpace, viewerSpace);
        session.requestAnimationFrame((time, frame) => {
          logger.info('[WebXR] frame:', time, frame);

          // const pose = frame.getViewerPose(localSpace);
          // logger.info('[WebXR] viewer pose:', pose);
          // Draw the scene
        });
      });
  }

  private prepare(gl: WebGLRenderingContext) {
    const exts = gl.getSupportedExtensions();
    logger.info(`[WebGL] supported extensions(${exts.length}):`, exts);

    // const engine = new BABYLON.Engine(gl, true, {
    //   disableWebGL2Support: true,
    //   xrCompatible: true,
    // });
    // logger.info('[Babylonjs] gl caps:', engine.getCaps());
    // logger.info('[Babylonjs] gl info:', engine.getGlInfo());
    // logger.info('[Babylonjs] shader platform name =', engine.shaderPlatformName);
    // logger.info('[Babylonjs] shader version =', engine.webGLVersion);
    // logger.info('[Babylonjs] support ubo =', engine.supportsUniformBuffers);

    // BABYLON.Logger.OnNewCacheEntry = (entry) => {
    //   logger.info('[Babylonjs]', entry);
    // };
    // this.engine = engine;

    // const scene = this.scene = new BABYLON.Scene(engine);
    // const camera = new BABYLON.ArcRotateCamera('Camera', 0, 0, 0, BABYLON.Vector3.Zero(), scene);
    // camera.fov = (60 * Math.PI) / 180;
    // camera.minZ = 0.1;
    // camera.maxZ = 100.0;
    // camera.setPosition(new BABYLON.Vector3(0, 0, -20));
    // this.mainCamera = camera;

    // const light = this.defaultLight = new BABYLON.DirectionalLight('light', new BABYLON.Vector3(0, 1, 0), scene);
    // light.intensity = 0.7;
  }

  private appEntry(scene: BABYLON.Scene) {
    var mat = new BABYLON.StandardMaterial("mat", scene);
    var texture = new BABYLON.Texture('https://i.imgur.com/Wk1cGEq.png', scene);
    // var texture = new BABYLON.DynamicTexture('dynamic texture', { width: 512, height: 512 }, scene);
    mat.diffuseTexture = texture;
    // mat.bumpTexture = new BABYLON.Texture('https://i.imgur.com/wGyk6os.png', scene);
    // mat.diffuseColor = new BABYLON.Color3(1, 0, 0);

    // {
    //   const ctx = texture.getContext();
    //   ctx.beginPath();
    //   ctx.moveTo(75 * 2, 25 * 2);
    //   ctx.quadraticCurveTo(25 * 2, 25 * 2, 25 * 2, 62.5 * 2);
    //   ctx.quadraticCurveTo(25 * 2, 100 * 2, 50 * 2, 100 * 2);
    //   ctx.quadraticCurveTo(50 * 2, 120 * 2, 30 * 2, 125 * 2);
    //   ctx.quadraticCurveTo(60 * 2, 120 * 2, 65 * 2, 100 * 2);
    //   ctx.quadraticCurveTo(125 * 2, 100 * 2, 125 * 2, 62.5 * 2);
    //   ctx.quadraticCurveTo(125 * 2, 25 * 2, 75 * 2, 25 * 2);
    //   ctx.fillStyle = 'white';
    //   ctx.fill();
    //   texture.update();
    // }

    // var faceUV = new Array(6);
    // //set all faces
    // faceUV[0] = new BABYLON.Vector4(0, 0.5, 1 / 3, 1);
    // faceUV[1] = new BABYLON.Vector4(1 / 3, 0, 2 / 3, 0.5);
    // faceUV[2] = new BABYLON.Vector4(2 / 3, 0, 1, 0.5);
    // faceUV[3] = new BABYLON.Vector4(0, 0, 1 / 3, 0.5);
    // faceUV[4] = new BABYLON.Vector4(1 / 3, 0.5, 2 / 3, 1);
    // faceUV[5] = new BABYLON.Vector4(2 / 3, 0.5, 1, 1);

    //wrap set
    // var options = {
    //   faceUV: faceUV,
    //   wrap: true
    // };
    const head = BABYLON.MeshBuilder.CreateSphere('head', {
      diameter: 2.01,
      segments: 64,
    }, scene);
    head.material = mat;
    head.showBoundingBox = true;
    head.renderOutline = true;

    // rotate head by Babylonjs's AnimationGroup
    // var animationGroup = new BABYLON.AnimationGroup("head-rotation");
    // var animation = new BABYLON.Animation("head-rotation", "rotation.y", 5, BABYLON.Animation.ANIMATIONTYPE_FLOAT, BABYLON.Animation.ANIMATIONLOOPMODE_CYCLE);
    // var keys = [];
    // keys.push({ frame: 0, value: 0 });
    // keys.push({ frame: 30, value: Math.PI });
    // keys.push({ frame: 60, value: 2 * Math.PI });
    // animation.setKeys(keys);
    // animationGroup.addTargetedAnimation(animation, head);
    // animationGroup.play(true);

    // const modelUrlToLoad = 'http://ar.rokidcdn.com/web-assets/pages/models/welcome.glb';
    // fetch(modelUrlToLoad).then(res => res.arrayBuffer())
    //   .then(async arrayBuffer => {
    //     const imported = await BABYLON.SceneLoader.ImportMeshAsync(
    //       '',
    //       '',
    //       new Uint8Array(arrayBuffer, 0, arrayBuffer.byteLength),
    //       scene,
    //       null,
    //       '.glb'
    //     );
    //     logger.info('[Babylonjs] glb loaded', imported);

    //     const animationGroups = imported.animationGroups;
    //     if (animationGroups.length > 0) {
    //       animationGroups[0].play(true);
    //     }
    //   });

    // BABYLON.SceneLoader.Append(
    //   "https://playground.babylonjs.com/scenes/BoomBox/", "BoomBox.gltf", scene, function (scene) {
    //   });

    // BABYLON.SceneLoader.ImportMesh("", "https://playground.babylonjs.com/scenes/Dude/", "Dude.babylon", scene,
    //   function (meshes, particleSystems, skeletons) {
    //     // scene.createDefaultCameraOrLight(true, true, true);
    //     // scene.createDefaultEnvironment();
    //     scene.beginAnimation(skeletons[0], 0, 100, true, 1.0);
    //   }
    // );
  }
}
