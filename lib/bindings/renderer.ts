import * as logger from './logger';
import { createWebGLContext } from '../webgl';

const {
  RenderLoop,
} = process._linkedBinding('transmute:renderer');

let globalRenderLoop = null;
let globalGlContext = null;
let isReady = false;

const onreadyCallbacks: Array<(gl: WebGLRenderingContext) => void> = [];
const onframeCallbacks: Array<(time: number) => void> = [];

export function requestRendererReady(callback: (gl: WebGLRenderingContext) => void) {
  if (isReady) {
    callback(globalGlContext);
    return;
  } else {
    onreadyCallbacks.push(callback);
  }
}

let globalId = 0;
export function requestAnimationFrame(callback: FrameRequestCallback): number {
  onframeCallbacks.push(callback);
  return globalId++;
}

export function connectRenderer() {
  if (globalRenderLoop != null) {
    throw new TypeError('renderer already connected.');
  }

  const loop = globalRenderLoop = new RenderLoop();
  const gl = globalGlContext = createWebGLContext(1, 1, null);
  onreadyCallbacks.forEach(cb => cb(gl));
  onreadyCallbacks.length = 0;
  isReady = true;

  loop.setFrameCallback(function () {
    try {
      const now = Date.now();
      const callbackThisFrame = onframeCallbacks.slice();
      onframeCallbacks.length = 0;

      while (callbackThisFrame.length > 0) {
        const onframe = callbackThisFrame.shift();
        onframe(now);
      }
    } catch (err) {
      logger.warn('error in frame callback:', err);
    } finally {
      loop.setFrameFinished();
    }
  });

  logger.info('connected to renderer.');
}
