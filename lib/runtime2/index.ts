import 'babylonjs';
import { GLTFFileLoader } from '@babylonjs/loaders/glTF';
import { mat4, vec3 } from 'gl-matrix';

import { requestRendererReady } from '../bindings/renderer';
import { markRuntimeAvailable } from '../bindings/env';
import * as logger from '../bindings/logger';
import { addXsmlRequestListener, type XsmlRequestEvent } from '../bindings/messaging';
import {
  createBondXRSystem,
  XRWebGLLayer,
  type XRFrame,
  type XRView,
} from '../webxr';

const vertexSource = `
  attribute vec4 aVertexPosition;
  attribute vec4 aVertexColor;

  uniform mat4 uViewMatrix;
  uniform mat4 uModelMatrix;
  uniform mat4 uProjectionMatrix;

  varying lowp vec4 vColor;

  void main(void) {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aVertexPosition;
    vColor = aVertexColor;
  }
  `;

const fragmentSource = `
  varying lowp vec4 vColor;

  void main(void) {
    gl_FragColor = vColor;
  }
  `;

// register gltf loader
BABYLON.SceneLoader.RegisterPlugin(new GLTFFileLoader() as any);

function initBuffers(gl: WebGLRenderingContext) {
  const positionBuffer = initPositionBuffer(gl);
  const colorBuffer = initColorBuffer(gl);
  const indexBuffer = initIndexBuffer(gl);
  return {
    position: positionBuffer,
    color: colorBuffer,
    indices: indexBuffer,
  };
}

function initPositionBuffer(gl: WebGLRenderingContext) {
  // Create a buffer for the square's positions.
  const positionBuffer = gl.createBuffer();

  // Select the positionBuffer as the one to apply buffer
  // operations to from here out.
  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

  const biasX = 0;
  const biasZ = 0;
  const positions = [
    // Front face
    -1.0 + biasX, -1.0, 1.0 + biasZ, 1.0 + biasX, -1.0, 1.0 + biasZ, 1.0 + biasX, 1.0, 1.0 + biasZ, -1.0 + biasX, 1.0, 1.0 + biasZ,

    // Back face
    -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0,

    // Top face
    -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,

    // Bottom face
    -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0,

    // Right face
    1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0,

    // Left face
    -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, -1.0,
  ];

  // Now pass the list of positions into WebGL to build the
  // shape. We do this by creating a Float32Array from the
  // JavaScript array, then use it to fill the current buffer.
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

  return positionBuffer;
}

function initColorBuffer(gl: WebGLRenderingContext) {
  const faceColors = [
    [1.0, 1.0, 1.0, 1.0], // Front face: white
    [1.0, 0.0, 0.0, 1.0], // Back face: red
    [0.0, 1.0, 0.0, 1.0], // Top face: green
    [0.0, 0.0, 1.0, 1.0], // Bottom face: blue
    [1.0, 1.0, 0.0, 1.0], // Right face: yellow
    [1.0, 0.0, 1.0, 1.0], // Left face: purple
  ];

  // Convert the array of colors into a table for all the vertices.

  var colors = [];
  for (var j = 0; j < faceColors.length; ++j) {
    const c = faceColors[j];
    // Repeat each color four times for the four vertices of the face
    colors = colors.concat(c, c, c, c);
  }

  const colorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);

  return colorBuffer;
}

function initIndexBuffer(gl) {
  const indexBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);

  // This array defines each face as two triangles, using the
  // indices into the vertex array to specify each triangle's
  // position.
  const indices = [
    0, 1, 2,
    0, 2, 3,
    // front 👆
    4,
    5,
    6,
    4,
    6,
    7, // back
    8,
    9,
    10,
    8,
    10,
    11, // top
    12,
    13,
    14,
    12,
    14,
    15, // bottom
    16,
    17,
    18,
    16,
    18,
    19, // right
    20,
    21,
    22,
    20,
    22,
    23, // left
  ];

  // Now send the element array to GL

  gl.bufferData(
    gl.ELEMENT_ARRAY_BUFFER,
    new Uint16Array(indices),
    gl.STATIC_DRAW
  );

  return indexBuffer;
}

function drawScene(gl: WebGLRenderingContext, views: XRView[], programInfo, buffers, cubeRotation) {
  // gl.clearColor(0.0, 0.0, 0.0, 1.0); // Clear to black, fully opaque
  // gl.clearDepth(1.0); // Clear everything
  gl.enable(gl.DEPTH_TEST); // Enable depth testing
  gl.depthFunc(gl.LEQUAL); // Near things obscure far things

  // Clear the canvas before we start drawing on it.
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  // Create a perspective matrix, a special matrix that is
  // used to simulate the distortion of perspective in a camera.
  // Our field of view is 45 degrees, with a width/height
  // ratio that matches the display size of the canvas
  // and we only want to see objects between 0.1 units
  // and 100 units away from the camera.

  // const fieldOfView = (60 * Math.PI) / 180; // in radians
  // const aspect = gl.drawingBufferWidth / gl.drawingBufferHeight;
  // const zNear = 0.1;
  // const zFar = 100.0;
  const viewMatrix = views[0].transform.matrix;
  const projectionMatrix = views[0].projectionMatrix;

  // note: glmatrix.js always has the first argument
  // as the destination to receive the result.
  // mat4.perspective(projectionMatrix, fieldOfView, aspect, zNear, zFar);

  // Set the drawing position to the "identity" point, which is
  // the center of the scene.
  const scalingMatrix = mat4.create();
  mat4.scale(scalingMatrix, scalingMatrix, vec3.fromValues(0.1, 0.1, 0.1));

  const translationMatrix = mat4.create();
  mat4.translate(translationMatrix, translationMatrix, vec3.fromValues(0.1, 0.1, -1));

  const rotationMatrix = mat4.create();
  mat4.rotateX(rotationMatrix, rotationMatrix, cubeRotation * 0.3);
  mat4.rotateY(rotationMatrix, rotationMatrix, cubeRotation * 0.7);
  mat4.rotateZ(rotationMatrix, rotationMatrix, cubeRotation);

  const modelMatrix = mat4.create();
  mat4.multiply(modelMatrix, translationMatrix, rotationMatrix);
  mat4.multiply(modelMatrix, modelMatrix, scalingMatrix);

  // Tell WebGL how to pull out the positions from the position
  // buffer into the vertexPosition attribute.
  setPositionAttribute(gl, buffers, programInfo);
  setColorAttribute(gl, buffers, programInfo);

  // Tell WebGL which indices to use to index the vertices
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.indices);

  // Tell WebGL to use our program when drawing
  gl.useProgram(programInfo.program);

  logger.info(`[WebXR] view matrix(${programInfo.uniformLocations.viewMatrix}):`, viewMatrix);
  // Set the shader uniforms
  gl.uniformMatrix4fv(
    programInfo.uniformLocations.projectionMatrix,
    false,
    projectionMatrix
  );
  gl.uniformMatrix4fv(
    programInfo.uniformLocations.viewMatrix,
    false,
    viewMatrix
  );
  gl.uniformMatrix4fv(
    programInfo.uniformLocations.modelMatrix,
    false,
    modelMatrix
  );

  {
    const vertexCount = 36;
    const type = gl.UNSIGNED_SHORT;
    const offset = 0;
    gl.drawElements(gl.TRIANGLES, vertexCount, type, offset);
  }
}

// Tell WebGL how to pull out the positions from the position
// buffer into the vertexPosition attribute.
function setPositionAttribute(gl, buffers, programInfo) {
  const numComponents = 3;
  const type = gl.FLOAT; // the data in the buffer is 32bit floats
  const normalize = false; // don't normalize
  const stride = 0; // how many bytes to get from one set of values to the next
  // 0 = use type and numComponents above
  const offset = 0; // how many bytes inside the buffer to start from
  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.position);
  gl.vertexAttribPointer(
    programInfo.attribLocations.vertexPosition,
    numComponents,
    type,
    normalize,
    stride,
    offset
  );
  gl.enableVertexAttribArray(programInfo.attribLocations.vertexPosition);
}

// Tell WebGL how to pull out the colors from the color buffer
// into the vertexColor attribute.
function setColorAttribute(gl, buffers, programInfo) {
  const numComponents = 4;
  const type = gl.FLOAT;
  const normalize = false;
  const stride = 0;
  const offset = 0;
  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.color);
  gl.vertexAttribPointer(
    programInfo.attribLocations.vertexColor,
    numComponents,
    type,
    normalize,
    stride,
    offset
  );
  gl.enableVertexAttribArray(programInfo.attribLocations.vertexColor);
}

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

    const gl = this.gl;
    const vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vertexSource);
    gl.compileShader(vertexShader);

    const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fragmentSource);
    gl.compileShader(fragmentShader);

    const program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);
    const programInfo = {
      program,
      attribLocations: {
        vertexPosition: gl.getAttribLocation(program, "aVertexPosition"),
        vertexColor: gl.getAttribLocation(program, "aVertexColor"),
      },
      uniformLocations: {
        projectionMatrix: gl.getUniformLocation(
          program,
          "uProjectionMatrix"
        ),
        viewMatrix: gl.getUniformLocation(program, "uViewMatrix"),
        modelMatrix: gl.getUniformLocation(program, "uModelMatrix"),
      },
    };
    logger.info('program info:', JSON.stringify(programInfo));

    const buffers = initBuffers(gl);
    let cubeRotation = 0.0;
    let deltaTime = 0;
    let then = 0;

    const xrSystem = createBondXRSystem(event.sessionId);
    xrSystem.requestSession('immersive-ar')
      .then(async session => {
        session.updateRenderState({
          baseLayer: new XRWebGLLayer(session, this.gl),
        });

        const localSpace = await session.requestReferenceSpace('local');
        logger.info('[WebXR] local space:', localSpace);

        function handleXrFrame(time: number, frame: XRFrame) {
          session.requestAnimationFrame(handleXrFrame);
          try {
            const pose = frame.getViewerPose(localSpace);
            // pose.views.forEach(view => {
            //   logger.info('[WebXR] view:', view.eye, view.transform, view.projectionMatrix);
            // });
            const now = time * 0.001; // convert to seconds
            deltaTime = now - then;
            then = now;

            drawScene(gl, pose.views, programInfo, buffers, cubeRotation);
            cubeRotation += deltaTime;
            // Draw the scene
          } catch (err) {
            logger.error('[WebXR] frame error:', err);
          }
        }
        session.requestAnimationFrame(handleXrFrame);
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
