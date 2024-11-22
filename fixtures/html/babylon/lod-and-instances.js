import 'babylonjs';

async function createScene(engine) {
  const scene = new BABYLON.Scene(engine);
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

  var light0 = new BABYLON.HemisphericLight("Hemi0", new BABYLON.Vector3(0, 1, 0), scene);
  light0.diffuse = new BABYLON.Color3(1, 1, 1);
  light0.specular = new BABYLON.Color3(1, 1, 1);
  light0.groundColor = new BABYLON.Color3(0, 0, 0);

  var count = 3;
  var scale = 4;

  var knot00 = BABYLON.Mesh.CreateTorusKnot("knot0", 0.5, 0.2, 128, 64, 2, 3, scene);
  var knot01 = BABYLON.Mesh.CreateTorusKnot("knot1", 0.5, 0.2, 32, 16, 2, 3, scene);
  var knot02 = BABYLON.Mesh.CreateTorusKnot("knot2", 0.5, 0.2, 24, 12, 2, 3, scene);
  var knot03 = BABYLON.Mesh.CreateTorusKnot("knot3", 0.5, 0.2, 16, 8, 2, 3, scene);

  var material1 = new BABYLON.StandardMaterial("colo1", scene);
  material1.diffuseColor = new BABYLON.Color3(0.49, 0.25, 0);
  var material2 = material1.clone("colo2");
  material2.diffuseColor = new BABYLON.Color3(1.0, 0.5, 0.7);
  var material3 = material1.clone("colo3");
  material3.diffuseColor = new BABYLON.Color3(0.8, 1.0, 0.7);
  var material4 = material1.clone("colo4");
  material4.diffuseColor = new BABYLON.Color3(1.0, 1.0, 1.0);

  knot00.material = material1;
  knot01.material = material2;
  knot02.material = material3;
  knot03.material = material4;

  knot00.setEnabled(false);

  knot00.addLODLevel(15, knot01);
  knot00.addLODLevel(30, knot02);
  knot00.addLODLevel(45, knot03);
  knot00.addLODLevel(55, null);

  for (var x = -count; x <= count; x++) {
    for (var y = -count; y <= count; y++) {
      for (var z = 5; z < 10; z++) {
        var knot = knot00.createInstance("knotI");
        knot.position = new BABYLON.Vector3(x * scale, y * scale, z * scale);
      }
    }
  }
}).catch(err => {
  console.error(err);
});
