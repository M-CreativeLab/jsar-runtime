import 'babylonjs';
import { GLTFFileLoader } from '@babylonjs/loaders/glTF';

import { requestRendererReady } from '../bindings/renderer';
import * as logger from '../bindings/logger';

// register gltf loader
BABYLON.SceneLoader.RegisterPlugin(new GLTFFileLoader() as any);

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
    const exts = gl.getSupportedExtensions();
    logger.info(`[WebGL] supported extensions(${exts.length}):`, exts);

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

    var faceUV = new Array(6);
    //set all faces
    faceUV[0] = new BABYLON.Vector4(0, 0.5, 1 / 3, 1);
    faceUV[1] = new BABYLON.Vector4(1 / 3, 0, 2 / 3, 0.5);
    faceUV[2] = new BABYLON.Vector4(2 / 3, 0, 1, 0.5);
    faceUV[3] = new BABYLON.Vector4(0, 0, 1 / 3, 0.5);
    faceUV[4] = new BABYLON.Vector4(1 / 3, 0.5, 2 / 3, 1);
    faceUV[5] = new BABYLON.Vector4(2 / 3, 0.5, 1, 1);

    //wrap set
    var options = {
      faceUV: faceUV,
      wrap: true
    };
    var head = BABYLON.MeshBuilder.CreateSphere('head', {
      diameter: 2.01,
      segments: 64,
    }, scene);
    // head.material = mat;
    // head.showBoundingBox = true;
    // head.renderOutline = true;

    // rotate head by Babylonjs's AnimationGroup
    var animationGroup = new BABYLON.AnimationGroup("head-rotation");
    var animation = new BABYLON.Animation("head-rotation", "rotation.y", 5, BABYLON.Animation.ANIMATIONTYPE_FLOAT, BABYLON.Animation.ANIMATIONLOOPMODE_CYCLE);
    var keys = [];
    keys.push({ frame: 0, value: 0 });
    keys.push({ frame: 30, value: Math.PI });
    keys.push({ frame: 60, value: 2 * Math.PI });
    animation.setKeys(keys);
    animationGroup.addTargetedAnimation(animation, head);
    animationGroup.play(true);

    // const modelUrlToLoad = 'http://ar.rokidcdn.com/web-assets/pages/models/pirateFort.glb';
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
    //   });

    // BABYLON.SceneLoader.Append(
    //   "https://playground.babylonjs.com/scenes/BoomBox/", "BoomBox.gltf", scene, function (scene) {
    //   });

    var particleSystem = new BABYLON.ParticleSystem("particles", 2000, scene);

    //Texture of each particle
    particleSystem.particleTexture = new BABYLON.Texture("textures/flare.png", scene);

    // Where the particles come from
    particleSystem.emitter = BABYLON.Vector3.Zero(); // the starting location

    // Colors of all particles
    particleSystem.color1 = new BABYLON.Color4(0.7, 0.8, 1.0, 1.0);
    particleSystem.color2 = new BABYLON.Color4(0.2, 0.5, 1.0, 1.0);
    particleSystem.colorDead = new BABYLON.Color4(0, 0, 0.2, 0.0);

    // Size of each particle (random between...
    particleSystem.minSize = 0.1;
    particleSystem.maxSize = 0.5;

    // Life time of each particle (random between...
    particleSystem.minLifeTime = 0.3;
    particleSystem.maxLifeTime = 1.5;

    // Emission rate
    particleSystem.emitRate = 1000;


    /******* Emission Space ********/
    particleSystem.createSphereEmitter(2);


    // Speed
    particleSystem.minEmitPower = 1;
    particleSystem.maxEmitPower = 3;
    particleSystem.updateSpeed = 0.005;

    particleSystem.updateFunction = function (particles) {
      for (var index = 0; index < particles.length; index++) {
        var particle = particles[index];
        particle.age += this._scaledUpdateSpeed;

        if (particle.age >= particle.lifeTime) { // Recycle
          particles.splice(index, 1);
          this._stockParticles.push(particle);
          index--;
          continue;
        }
        else {
          particle.colorStep.scaleToRef(this._scaledUpdateSpeed, this._scaledColorStep);
          particle.color.addInPlace(this._scaledColorStep);
          particle.color = new BABYLON.Color4(Math.random(), Math.random(), Math.random(), 1)

          if (particle.color.a < 0)
            particle.color.a = 0;

          particle.angle += particle.angularSpeed * this._scaledUpdateSpeed;

          particle.direction.scaleToRef(this._scaledUpdateSpeed, this._scaledDirection);
          particle.position.addInPlace(this._scaledDirection);

          this.gravity.scaleToRef(this._scaledUpdateSpeed, this._scaledGravity);
          particle.direction.addInPlace(this._scaledGravity);
        }
      }
    }
    // Start the particle system
    particleSystem.start();
  }
}
