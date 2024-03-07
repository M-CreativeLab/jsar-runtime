import * as logger from './logger';
import { createWebGLContext } from '../webgl';

const {
  RenderLoop,
} = process._linkedBinding('transmute:renderer');

let globalRenderLoop = null;
let globalGlContext: WebGLRenderingContext | WebGL2RenderingContext = null;
let isReady = false;

const onreadyCallbacks: Array<(gl: WebGLRenderingContext) => void> = [];
const onframeCallbacks: Array<{
  callback: (time: number) => void,
  handle: number,
}> = [];

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
  const handle = globalId++;
  onframeCallbacks.push({ callback, handle });
  return handle;
}

export function cancelAnimationFrame(handle: number) {
  onframeCallbacks.splice(onframeCallbacks.findIndex(cb => cb.handle === handle), 1);
}

export function connectRenderer() {
  if (globalRenderLoop != null) {
    throw new TypeError('renderer already connected.');
  }

  const loop = globalRenderLoop = new RenderLoop();
  try {
    globalGlContext = createWebGLContext(1, 1, null);
  } catch (err) {
    logger.warn('error creating webgl context:', err);
  }
  if (globalGlContext == null) {
    return;
  }

  const gl = globalGlContext;
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
        onframe.callback(now);
      }
    } catch (err) {
      logger.warn('error in frame callback:', err);
    } finally {
      loop.setFrameFinished();
    }
  });
  logger.info('connected to renderer.');
}
