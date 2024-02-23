import 'babylonjs';
import 'babylonjs-loaders';

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
    const camera = new BABYLON.ArcRotateCamera('Camera', 0, 0, 0, BABYLON.Vector3.Zero(), scene);
    camera.fov = (60 * Math.PI) / 180;
    camera.minZ = 0.1;
    camera.maxZ = 100.0;
    camera.setPosition(new BABYLON.Vector3(0.0, 3.0, -8.0));
    this.mainCamera = camera;

    const light = this.defaultLight = new BABYLON.HemisphericLight('light', new BABYLON.Vector3(0, 1, 0), scene);
    light.intensity = 0.7;
  }

  private appEntry(scene: BABYLON.Scene) {
    // var mat0 = new BABYLON.StandardMaterial("mat0", scene);
    // mat0.diffuseColor = new BABYLON.Color3(1, 0, 0);
    // mat0.bumpTexture = new BABYLON.Texture("https://i.imgur.com/wGyk6os.png", scene);

    var mat1 = new BABYLON.StandardMaterial("mat1", scene);
    var tex = mat1.diffuseTexture = new BABYLON.Texture("https://i.imgur.com/Wk1cGEq.png", scene);
    // mat1.bumpTexture = new BABYLON.Texture("https://i.imgur.com/wGyk6os.png", scene);
    tex.getInternalTexture().onErrorObservable.add((err) => {
      logger.error('bumpTexture error:', err);
    });

    // var mat2 = new BABYLON.StandardMaterial("mat2", scene);
    // mat2.diffuseTexture = new BABYLON.Texture("textures/grass.png", scene);
    // mat2.bumpTexture = new BABYLON.Texture("https://i.imgur.com/wGyk6os.png", scene);

    //colour and bump
    // var sphere0 = BABYLON.MeshBuilder.CreateSphere("sphere0", {}, scene);
    // sphere0.position.x = -1.5;
    // sphere0.material = mat0;

    //original image and bump
    var sphere1 = BABYLON.MeshBuilder.CreateSphere("sphere1", {}, scene);
    sphere1.material = mat1;

    // //other image and bump
    // var sphere2 = BABYLON.MeshBuilder.CreateSphere("sphere2", {}, scene);
    // sphere2.material = mat2;
    // sphere2.position.x = 1.5;
  }
}
