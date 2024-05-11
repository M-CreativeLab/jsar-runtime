import * as logger from './logger';
import { getContext as getWebGLRenderingContext } from '../webgl';

const {
  RenderLoop,
} = process._linkedBinding('transmute:renderer');

let globalRenderLoop: Transmute.RenderLoop = null;
let globalGlContext: WebGLRenderingContext | WebGL2RenderingContext = null;
let isReady = false;

const onreadyCallbacks: Array<(gl: WebGLRenderingContext) => void> = [];
const onframeCallbacks: Array<{
  callback: Transmute.FrameRequestCallback,
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
export function requestAnimationFrame(callback: Transmute.FrameRequestCallback): number {
  const handle = globalId++;
  onframeCallbacks.push({ callback, handle });
  return handle;
}

export function cancelAnimationFrame(handle: number) {
  onframeCallbacks.splice(onframeCallbacks.findIndex(cb => cb.handle === handle), 1);
}

type GpuBusyCallback = () => void;
const ongpubusyCallbacks: Array<GpuBusyCallback> = [];
export function requestGpuBusyCallback(callback: GpuBusyCallback) {
  ongpubusyCallbacks.push(callback);
}

export function connectRenderer() {
  if (globalRenderLoop != null) {
    throw new TypeError('renderer already connected.');
  }

  const loop = globalRenderLoop = new RenderLoop();
  loop.setExceptionCallback(function (code) {
    if (code === 0x03 /** kFrameExecutionGpuBusy */) {
      ongpubusyCallbacks.forEach(cb => cb());
    } else {
      logger.error(`Unknown renderer exception occurred, the code is: ${code}`);
    }
  });
  loop.setFrameCallback(function (time, data) {
    try {
      const callbackThisFrame = onframeCallbacks.slice();
      onframeCallbacks.length = 0;

      while (callbackThisFrame.length > 0) {
        const onframe = callbackThisFrame.shift();
        onframe.callback(time, data);
      }
    } catch (err) {
      logger.warn('error in frame callback:', err);
    }
  });

  /**
   * Initialize the global WebGL context.
   */
  try {
    globalGlContext = getWebGLRenderingContext(loop.supportsWebGL2() ? 'webgl2' : 'webgl');
  } catch (err) {
    logger.warn('error creating webgl context:', err);
  }
  if (globalGlContext == null) {
    throw new Error('failed to create webgl context.');
  }

  const gl = globalGlContext;
  onreadyCallbacks.forEach(cb => cb(gl));
  onreadyCallbacks.length = 0;
  isReady = true;
  logger.info('connected to renderer.');
}
