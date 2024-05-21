import * as logger from './logger';
import { getContext as getWebGLRenderingContext } from '../webgl';

const {
  RenderLoop,
  AnimationFrameListener,
} = process._linkedBinding('transmute:renderer');

let globalRenderLoop: Transmute.RenderLoop = null;
let globalAnimationFrameListener: Transmute.AnimationFrameListener = null;
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
/**
 * It requests the runtime to call the specified function to update the animation before the next repaint.
 * 
 * @param callback the callback function to be invoked before the next repaint.
 * @returns a handle that can be used to cancel the callback.
 */
export function requestAnimationFrame(callback: Transmute.FrameRequestCallback): number {
  const handle = globalId++;
  onframeCallbacks.push({ callback, handle });
  return handle;
}

/**
 * Cancels a callback previously scheduled by `requestAnimationFrame()`.
 * 
 * @param handle the handle returned by requestAnimationFrame.
 */
export function cancelAnimationFrame(handle: number) {
  onframeCallbacks.splice(onframeCallbacks.findIndex(cb => cb.handle === handle), 1);
}

type GpuBusyCallback = () => void;
const ongpubusyCallbacks: Array<GpuBusyCallback> = [];
export function requestGpuBusyCallback(callback: GpuBusyCallback) {
  ongpubusyCallbacks.push(callback);
}

function onAnimationFrame(time: number, data: any) {
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
}

export function connectRenderer() {
  if (isReady) {
    throw new TypeError('renderer is already connected.');
  }

  // const loop = globalRenderLoop = new RenderLoop();
  // loop.setExceptionCallback(function (code) {
  //   if (code === 0x03 /** kFrameExecutionGpuBusy */) {
  //     ongpubusyCallbacks.forEach(cb => cb());
  //   } else {
  //     logger.error(`Unknown renderer exception occurred, the code is: ${code}`);
  //   }
  // });
  // loop.setFrameCallback(onAnimationFrame);

  /**
   * Setup for animation frame listener.
   */
  globalAnimationFrameListener = new AnimationFrameListener();
  globalAnimationFrameListener.connect(onAnimationFrame);

  /**
   * Initialize the global WebGL context.
   */
  // try {
  //   globalGlContext = getWebGLRenderingContext('webgl2');
  // } catch (err) {
  //   logger.warn('error creating webgl context:', err);
  // }
  // if (globalGlContext == null) {
  //   throw new Error('failed to create webgl context.');
  // }

  // const gl = globalGlContext;
  // onreadyCallbacks.forEach(cb => cb(gl));
  // onreadyCallbacks.length = 0;
  isReady = true;
  logger.info('connected to renderer.');
}
