import * as logger from './logger';
import { createWebGLContext } from '../webgl';

const {
  RenderLoop,
} = process._linkedBinding('transmute:renderer');

let globalRenderLoop = null;

export function connectRenderer() {
  if (globalRenderLoop != null) {
    throw new TypeError('renderer already connected.');
  }

  const loop = globalRenderLoop = new RenderLoop();
  const gl = createWebGLContext(1, 1, null);
  const vertexShader = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(vertexShader, `
in vec2 aVertexPosition;
layout(binding = 0) uniform vec2 uScalingFactor;
layout(binding = 1) uniform vec2 uRotationVector;
out vec4 vColor;

void main() {
  vec2 rotatedPosition = vec2(
    aVertexPosition.x * uRotationVector.y +
    aVertexPosition.y * uRotationVector.x,
    aVertexPosition.y * uRotationVector.y -
    aVertexPosition.x * uRotationVector.x
  );
  gl_Position = vec4(rotatedPosition * uScalingFactor, 0.0, 1.0);
}
  `);
  gl.compileShader(vertexShader);

  const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(fragmentShader, `
#version 460 core
out vec4 fragColor;
void main() {
  fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
      `);
  gl.compileShader(fragmentShader);
  logger.info('compiled shaders.');

  const program = gl.createProgram();
  gl.attachShader(program, vertexShader);
  gl.attachShader(program, fragmentShader);
  gl.linkProgram(program);

  (function initializeAttributes() {
    gl.enableVertexAttribArray(0);
    const buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.vertexAttribPointer(0, 1, gl.FLOAT, false, 0, 0);
  })();
  gl.useProgram(program);

  loop.setFrameCallback(function () {
    try {
      gl.drawArrays(gl.POINTS, 0, 1);
      // gl.clearColor(1, 1, 0, 1);
      // gl.clear(gl.COLOR_BUFFER_BIT);
    } catch (err) {
      logger.warn('error in frame callback:', err);
    } finally {
      loop.setFrameFinished();
    }
  });
  logger.info('connected to renderer.');
}
