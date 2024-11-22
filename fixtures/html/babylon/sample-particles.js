import 'babylonjs';

async function createScene(engine) {
  const scene = new BABYLON.Scene(engine);
  scene.useRightHandedSystem = false;
  scene.autoClear = false;
  scene.autoClearDepthAndStencil = false;
  scene.blockMaterialDirtyMechanism = true;
  scene.performancePriority = BABYLON.ScenePerformancePriority.Intermediate;

  scene.createDefaultSkybox();
  scene.createDefaultCamera(true);

  // Run the render loop
  engine.runRenderLoop(() => scene.render());

  // Create XR experience and enter XR
  const { baseExperience, renderTarget } = await scene.createDefaultXRExperienceAsync({
    disableDefaultUI: true,
    disablePointerSelection: true,
    disableNearInteraction: true,
    disableHandTracking: true,
    disableTeleportation: true,
    outputCanvasOptions: {
      renderingContext: navigator.gl,
    },
  });
  await baseExperience.enterXRAsync('immersive-ar', 'local', renderTarget, {
    optionalFeatures: [],
  });
  return scene;
}

let engine;
if (!navigator.gl) {
  const canvas = document.createElement('canvas');
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;
  document.body.appendChild(canvas);
  engine = new BABYLON.Engine(canvas, true, {
    xrCompatible: true
  });
} else {
  engine = new BABYLON.Engine(navigator.gl, true, {
    xrCompatible: true
  });
}
engine.disableUniformBuffers = true;

createScene(engine).then(scene => {
  scene.createDefaultLight();

  // Create a particle system
  var particleSystem = new BABYLON.ParticleSystem('particles', 2000, scene);

  //Texture of each particle
  particleSystem.particleTexture = new BABYLON.Texture('https://playground.babylonjs.com/textures/flare.png', scene);

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
}, err => {
  console.error(err);
});
