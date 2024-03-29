import * as logger from '../bindings/logger';

/**
 * Browser Pollyfills for Node.js
 */
import { XMLHttpRequestImpl } from './xhr2';

/**
 * A patch to the Node.js TextDecoder.
 *
 * Node.js TextDecoder with samll ICU doesn't support ascii encoding, however the fontkit library depends on
 * the ASCII decoder to parse the font file headers.
 */
import './textdecoder';
import { ErrorEventImpl } from './events/error-event';
import {
  OffscreenCanvasImpl,
  ImageDataImpl,
  createImageBitmapImpl,
  InitializeOffscreenCanvas
} from './offscreencanvas';
import { createNavigator } from './navigator';
import { createWindow } from './window';
import {
  requestAnimationFrame as requestAnimationFrameImpl,
  cancelAnimationFrame as cancelAnimationFrameImpl,
} from '../bindings/renderer';
import {
  XRWebGLLayerImpl,
  XRRigidTransformImpl
} from '../webxr';

globalThis.ErrorEvent = ErrorEventImpl;
globalThis.XMLHttpRequest = XMLHttpRequestImpl;
// globalThis.AudioContext = AudioContextImpl;
globalThis.OffscreenCanvas = OffscreenCanvasImpl;
globalThis.ImageData = ImageDataImpl;

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
    value: createImageBitmapImpl,
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
logger.info('navigator =>', typeof navigator, typeof globalThis.navigator);
logger.info('=============================');

export {
  InitializeOffscreenCanvas,
}
