import * as logger from './bindings/logger';
import { createEnv, getRuntimeInit } from './bindings/env';

/**
 * See https://nodejs.org/api/events.html#eventtarget-error-handling
 *
 * Becauset Node.js EventTarget will cause an uncaught exception on `process.nextTick()`, thus we
 * have no way to handle the error in better way, so we just log the error and not exiting the process.
 */
function handleGlobalExceptionOrRejection(err) {
  logger.warn(`
==============================
uncaught exception or rejection
Stack: ${err?.stack || 'null'}
Message: ${err?.message || err || 'null'}
==============================
  `);
  process.exit(1);
}
process.on('uncaughtException', handleGlobalExceptionOrRejection);
process.on('unhandledRejection', handleGlobalExceptionOrRejection);

/**
 * Browser Pollyfills for Node.js
 */
import { XMLHttpRequestImpl } from './polyfills/xhr2';

/**
 * A patch to the Node.js TextDecoder.
 *
 * Node.js TextDecoder with samll ICU doesn't support ascii encoding, however the fontkit library depends on
 * the ASCII decoder to parse the font file headers.
 */
import './polyfills/textdecoder';
import { ErrorEventImpl } from './polyfills/events/error-event';
import {
  OffscreenCanvasImpl,
  ImageDataImpl,
  createImageBitmapImpl,
  InitializeOffscreenCanvas
} from './polyfills/offscreencanvas';
import { createNavigator } from './polyfills/navigator';

// for testing
import { connectRenderer, requestAnimationFrame as requestAnimationFrameImpl } from './bindings/renderer';
import { createXRSystem } from './webxr';
import { TransmuteRuntime2 } from './runtime2';
// import runExample from './webgl-examples/textures';

globalThis.ErrorEvent = ErrorEventImpl;
globalThis.XMLHttpRequest = XMLHttpRequestImpl;
// globalThis.AudioContext = AudioContextImpl;
globalThis.OffscreenCanvas = OffscreenCanvasImpl;
globalThis.ImageData = ImageDataImpl;

globalThis.navigator = createNavigator();
globalThis.createImageBitmap = createImageBitmapImpl;
globalThis.requestAnimationFrame = requestAnimationFrameImpl;

(async function main() {
  try {
    const now = performance.now();
    logger.info('Start the TransmuteRuntime entry');

    createEnv();
    const init = getRuntimeInit();
    logger.info('The runtimeInit:', init);

    // Initialize the OffscreenCanvas polyfill.
    await InitializeOffscreenCanvas({ loadSystemFonts: true });
    logger.info(`The Skia initialization takes ${performance.now() - now}ms`);

    connectRenderer();
    createXRSystem();

    const runtime = new TransmuteRuntime2();
    runtime.start();
    logger.info('Started Transmute Runtime, it takes', performance.now() - now, 'ms');
  } catch (err) {
    logger.error('failed to start the runtime, occurs an error:', err);
  }
})();
