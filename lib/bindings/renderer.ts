import { getContext as createWebGLRenderingContext } from '../webgl';

const {
  AnimationFrameListener,
} = process._linkedBinding('transmute:renderer');

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

function onAnimationFrame(time: number) {
  try {
    const callbackThisFrame = onframeCallbacks.slice();
    onframeCallbacks.length = 0;

    while (callbackThisFrame.length > 0) {
      const onframe = callbackThisFrame.shift();
      onframe.callback(time);
    }
  } catch (err) {
    console.warn('#');
    console.warn('# Occurred error in an animation frame callback.');
    console.warn('#');
    console.warn('#', err?.stack || err);
  }
}

/**
 * Connects the runtime to the renderer backend.
 * 
 * It initializes the global WebGL context and starts the animation frame listener.
 * 
 * @param clientContext the context object, this method use this object to initialize the renderer.
 * @returns a boolean value indicates whether the renderer is connected successfully.
 */
export function connectRenderer(clientContext: Transmute.TrClientContext): boolean {
  if (isReady) {
    throw new TypeError('renderer is already connected.');
  }

  /**
   * Setup for animation frame listener.
   */
  globalAnimationFrameListener = new AnimationFrameListener();
  globalAnimationFrameListener.connect(onAnimationFrame);

  /**
   * Initialize the global WebGL context.
   */
  try {
    globalGlContext = createWebGLRenderingContext(clientContext.webglVersion === 1 ? 'webgl' : 'webgl2');
  } catch (err) {
    console.warn('failed to create webgl context:', err);
  }
  if (globalGlContext == null) {
    throw new Error('failed to create webgl context.');
  }

  const gl = globalGlContext;
  onreadyCallbacks.forEach(cb => cb(gl));
  onreadyCallbacks.length = 0;
  isReady = true;
  return isReady;
}

export function getWebGLRenderingContext(): WebGLRenderingContext | WebGL2RenderingContext {
  if (!isReady) {
    throw new Error('renderer is not ready, call connectRenderer() first.');
  }
  return globalGlContext;
}
