import 'babylonjs';
import { requestRendererReady } from '../bindings/renderer';
import * as logger from '../bindings/logger';

export class TransmuteRuntime2 {
  private scene: BABYLON.Scene;
  private engine: BABYLON.Engine;
  private mainCamera: BABYLON.Camera;
  private defaultLight: BABYLON.Light;

  constructor() {
    requestRendererReady(gl => {
      this.prepare(gl);
      this.appEntry(this.scene);

      // run loop
      this.engine.runRenderLoop(() => {
        this.scene.render();
      });
    });
  }

  private prepare(gl: WebGLRenderingContext) {
    const engine = new BABYLON.Engine(gl, true, {
      disableWebGL2Support: true,
      xrCompatible: false,
    });

    logger.info('[Babylonjs] gl caps:', engine.getCaps());
    logger.info('[Babylonjs] gl info:', engine.getGlInfo());
    logger.info('[Babylonjs] shader platform name =', engine.shaderPlatformName);
    logger.info('[Babylonjs] shader version =', engine.webGLVersion);
    logger.info('[Babylonjs] support ubo =', engine.supportsUniformBuffers);

    BABYLON.Logger.OnNewCacheEntry = (entry) => {
      logger.info('[Babylonjs]', entry);
    };
    this.engine = engine;

    const scene = this.scene = new BABYLON.Scene(engine);
    const camera = new BABYLON.FreeCamera('camera1', new BABYLON.Vector3(0, 0, 0.1), scene);
    camera.fov = (60 * Math.PI) / 180;
    camera.minZ = 0.1;
    camera.maxZ = 100.0;
    camera.setTarget(BABYLON.Vector3.Zero());
    this.mainCamera = camera;

    const light = this.defaultLight = new BABYLON.HemisphericLight('light', new BABYLON.Vector3(0, 1, 0), scene);
    light.intensity = 0.7;
  }

  private appEntry(scene: BABYLON.Scene) {
    const main = BABYLON.MeshBuilder.CreateTorus('torus', { thickness: 0.25, diameter: 2 }, scene);
    main.position.z = -8;
    main.rotation = new BABYLON.Vector3(0, 30, 30);

    // create animation
    // const frameRate = 120;
    // const xSlide = new BABYLON.Animation(
    //   'xSlide',
    //   'position.x',
    //   frameRate,
    //   BABYLON.Animation.ANIMATIONTYPE_FLOAT,
    //   BABYLON.Animation.ANIMATIONLOOPMODE_CYCLE
    // );
    // const keyFrames = [
    //   { frame: 0, value: 2 },
    //   { frame: frameRate, value: -2 },
    //   { frame: 2 * frameRate, value: 2 },
    // ];
    // xSlide.setKeys(keyFrames);
    // cube.animations.push(xSlide);
    // scene.beginAnimation(cube, 0, 2 * frameRate, true);
  }
}
