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

  loop.setFrameCallback(function () {
    try {
      const vertexShader = gl.createShader(gl.VERTEX_SHADER);
      gl.shaderSource(vertexShader, `
#version 460 core
void main() {
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    gl_PointSize = 64.0;
}
      `.trim());
      gl.compileShader(vertexShader);

      // gl.enable(gl.SCISSOR_TEST);
      // gl.scissor(40, 20, 60, 130);

      // gl.viewport(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight);
      gl.clearColor(1, 1, 0, 1);
      gl.clear(gl.COLOR_BUFFER_BIT);
    } catch (err) {
      logger.warn('error in frame callback:', err);
    } finally {
      loop.setFrameFinished();
    }
  });
  logger.info('connected to renderer.');
}
