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
    var mat = new BABYLON.StandardMaterial("mat", scene);
    var texture = new BABYLON.Texture("https://i.imgur.com/vxH5bCg.jpg", scene);
    mat.diffuseTexture = texture;

    var faceUV = new Array(6);

    //set all faces
    faceUV[0] = new BABYLON.Vector4(0, 0.5, 1 / 3, 1);
    faceUV[1] = new BABYLON.Vector4(1 / 3, 0, 2 / 3, 0.5);
    faceUV[2] = new BABYLON.Vector4(2 / 3, 0, 1, 0.5);
    faceUV[3] = new BABYLON.Vector4(0, 0, 1 / 3, 0.5);
    faceUV[4] = new BABYLON.Vector4(1 / 3, 0.5, 2 / 3, 1);
    faceUV[5] = new BABYLON.Vector4(2 /3, 0.5, 1, 1);

    //wrap set
    var options = {
        faceUV: faceUV,
        wrap: true
    };
    var head = BABYLON.MeshBuilder.CreateBox('head', options, scene);
    head.material = mat;
  }
}
