import 'babylonjs';
import { requestRendererReady } from '../bindings/renderer';
import * as logger from '../bindings/logger';

export class TransmuteRuntime2 {
  constructor() {
    requestRendererReady(gl => {
      const engine = new BABYLON.Engine(gl, true, {
        disableWebGL2Support: true,
      });
      const scene = new BABYLON.Scene(engine);
      const camera = new BABYLON.FreeCamera('camera1', new BABYLON.Vector3(0, 0, 0), scene);
      camera.setTarget(BABYLON.Vector3.Zero());

      var light = new BABYLON.HemisphericLight('light', new BABYLON.Vector3(0, 1, 0), scene);
      light.intensity = 0.7;

      var sphere = BABYLON.MeshBuilder.CreateSphere('sphere', {diameter: 2, segments: 32}, scene);
      sphere.position.y = 8;
      var ground = BABYLON.MeshBuilder.CreateGround('ground', {width: 6, height: 6}, scene);
      ground.position.z = 8;
      logger.info('The BabylonJS scene is created');

      engine.runRenderLoop(() => {
        scene.render(true, false);
      });
    });
  }
}
