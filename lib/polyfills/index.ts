import * as logger from '@transmute/logger';

/**
 * Browser Pollyfills for Node.js
 */
import { XMLHttpRequestImpl } from './xhr2';
import { ErrorEventImpl } from './events/error-event';
import { PointerEventImpl } from './events/pointer-event';
// import {
//   OffscreenCanvasImpl,
//   ImageDataImpl,
//   createImageBitmapImpl,
//   InitializeOffscreenCanvas
// } from './offscreencanvas';
import {
  ImageData,
  ImageBitmap,
  createImageBitmap,
} from './canvas2d';
import { createNavigator } from './navigator';
import { createWindow } from './window';
import {
  requestAnimationFrame as requestAnimationFrameImpl,
  cancelAnimationFrame as cancelAnimationFrameImpl,
} from '../bindings/renderer';
import {
  WebGLRenderingContextImpl,
  WebGL2RenderingContextImpl
} from '../webgl';
import {
  XRWebGLLayerImpl,
  XRRigidTransformImpl
} from '../webxr';

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
});

/**
 * Global functions
 */
Object.defineProperties(globalThis, {
  'navigator': {
    value: createNavigator(),
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
globalThis.window = createWindow();

logger.info('Polyfills have been loaded.');
logger.info(`window() =>`, typeof window, typeof globalThis.window);
logger.info('=============================');
