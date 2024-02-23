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
    var pl = new BABYLON.PointLight("pl", new BABYLON.Vector3(0, 0, 0), scene);
    pl.diffuse = new BABYLON.Color3(1, 1, 1);
    pl.specular = new BABYLON.Color3(1, 0, 0);
    pl.intensity = 0.95;

    var mat = new BABYLON.StandardMaterial("mat1", scene);
    mat.alpha = 1.0;
    mat.diffuseColor = new BABYLON.Color3(0.5, 0.5, 1.0);
    mat.backFaceCulling = false;
    //mat.wireframe = true;

    // tube creation
    var sideOrientation = BABYLON.Mesh.FRONTSIDE;
    var radius = 5;
    var path = [];
    for (var i = -20; i < 20; i++) {
      var x = i * 2;
      var y = 0;
      var z = 0;
      path.push(new BABYLON.Vector3(x, y, z));
    }
    var mesh = BABYLON.Mesh.CreateTube("tube", path, 5, 16, null, 0, scene, true, sideOrientation);
    mesh.material = mat;

    var updatePath = function (path, k) {
      for (var i = 0; i < path.length; i++) {
        var x = path[i].x;
        var y = 5 * Math.sin(i / 10 + k * 2);
        var z = path[i].z;
        path[i].x = x;
        path[i].y = y;
        path[i].z = z;
      }
    };

    var updateFixedRadius = function (radius, k) {
      return (radius + 2 * Math.sin(k));
    };

    // morphing
    var k = 0;
    scene.registerBeforeRender(() => {
      //radiusFunction  
      var radiusFunction = function (i, distance) {
        return radius + 2 * Math.sin(i / 2 + k);
      };

      updatePath(path, k);
      var rad = updateFixedRadius(radius, k);
      mesh = BABYLON.Mesh.CreateTube(null, path, rad, null, radiusFunction, null, null, null, null, mesh);
      k += 0.05;
      pl.position = this.mainCamera.position;
    });
  }
}
