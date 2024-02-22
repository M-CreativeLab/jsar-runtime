import 'babylonjs';
import { requestRendererReady } from '../bindings/renderer';
import * as logger from '../bindings/logger';

export class TransmuteRuntime2 {
  constructor() {
    requestRendererReady(gl => {
      const engine = new BABYLON.Engine(gl, true, {
        disableWebGL2Support: true,
        xrCompatible: false,
      });
      BABYLON.Logger.OnNewCacheEntry = (entry) => {
        logger.info('[Babylonjs]', entry);
      };

      const scene = new BABYLON.Scene(engine);
      const camera = new BABYLON.FreeCamera('camera1', new BABYLON.Vector3(0, 0, 0.1), scene);
      camera.setTarget(BABYLON.Vector3.Zero());

      const light = new BABYLON.HemisphericLight('light', new BABYLON.Vector3(0, 1, 0), scene);
      light.intensity = 0.7;

      const sphere = BABYLON.MeshBuilder.CreateSphere('sphere', {diameter: 2, segments: 32}, scene);
      sphere.position.z = -8;
      sphere.onBeforeRenderObservable.add((mesh) => {
        logger.info(`render sphere at ${performance.now()}`, mesh.subMeshes.length);
      });
      sphere.onBeforeBindObservable.add(() => {
        logger.info(`bind sphere at ${performance.now()}`);
      });
      sphere.onBeforeDrawObservable.add(() => {
        logger.info(`draw sphere at ${performance.now()}`);
      });
      
      scene._beforeRenderingMeshStage.registerStep(
        0,
        null,
        () => {
          logger.info(`before rendering mesh stage at ${performance.now()}`);
        }
      );
      logger.info('rendering mesh stags', scene._beforeRenderingMeshStage);
      
      scene.onNewMaterialAddedObservable.add((mat) => {
        logger.info(`new material added at ${performance.now()}`, mat.name, mat);
      });

      engine.runRenderLoop(() => {
        scene.render();
      });
    });
  }
}
