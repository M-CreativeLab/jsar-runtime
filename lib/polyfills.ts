/**
 * Browser Pollyfills for Node.js
 */
import { navigator } from './navigator';
import { createWindow } from './window';
import { XMLHttpRequestImpl } from './xhr';
import { ErrorEventImpl } from './events/error-event';
import { PointerEventImpl } from './events/pointer-event';
import {
  requestAnimationFrame as requestAnimationFrameImpl,
  cancelAnimationFrame as cancelAnimationFrameImpl,
} from './bindings/renderer';
import {
  ImageData,
  ImageBitmap,
  createImageBitmap,
  OffscreenCanvas,
} from './webcanvas/canvas2d';
import {
  WebGLRenderingContextImpl,
  WebGL2RenderingContextImpl
} from './webgl';
import {
  XRWebGLLayerImpl,
  XRRigidTransformImpl
} from './webxr';
import {
  WorkerImpl,
} from './webworkers/worker';

export function loadPolyfills() {
  globalThis.XMLHttpRequest = XMLHttpRequestImpl;
  // globalThis.AudioContext = AudioContextImpl;

  /**
   * Events
   */
  Object.defineProperties(globalThis, {
    'ErrorEvent': {
      value: ErrorEventImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'PointerEvent': {
      value: PointerEventImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
  });

  /**
   * Add WebGL interfaces
   */
  Object.defineProperties(globalThis, {
    'WebGLRenderingContext': {
      value: WebGLRenderingContextImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'WebGL2RenderingContext': {
      value: WebGL2RenderingContextImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
  });

  /**
   * Add WebXR interfaces
   */
  Object.defineProperties(globalThis, {
    'XRRigidTransform': {
      value: XRRigidTransformImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'XRWebGLLayer': {
      value: XRWebGLLayerImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
  });

  /**
   * Canvas2D classes
   */
  Object.defineProperties(globalThis, {
    'ImageData': {
      value: ImageData,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'ImageBitmap': {
      value: ImageBitmap,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'OffscreenCanvas': {
      value: OffscreenCanvas,
      writable: false,
      enumerable: true,
      configurable: false,
    },
  });

  /**
   * Global functions
   */
  Object.defineProperties(globalThis, {
    'navigator': {
      value: navigator,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'createImageBitmap': {
      value: createImageBitmap,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'requestAnimationFrame': {
      value: requestAnimationFrameImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
    'cancelAnimationFrame': {
      value: cancelAnimationFrameImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
  });

  /**
   * Web Workers
   */
  Object.defineProperties(globalThis, {
    'Worker': {
      value: WorkerImpl,
      writable: false,
      enumerable: true,
      configurable: false,
    },
  });

  globalThis.window = createWindow();

  console.info('Polyfills have been loaded.');
  console.info(`window() =>`, typeof window, typeof globalThis.window);
  console.info('=============================');
}

