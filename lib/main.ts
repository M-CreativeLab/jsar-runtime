import * as logger from './bindings/logger';
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

import path from 'path';
import crypto from 'crypto';

/**
 * Browser Pollyfills for Node.js
 */

import XMLHttpRequest from 'xhr2';
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

// for testing
import { TransmuteRuntime2 } from './runtime2';
import { connectRenderer, requestAnimationFrame as requestAnimationFrameImpl } from './bindings/renderer';
// import runExample from './webgl-examples/textures';

globalThis.ErrorEvent = ErrorEventImpl;
globalThis.XMLHttpRequest = XMLHttpRequest;
// globalThis.AudioContext = AudioContextImpl;
globalThis.OffscreenCanvas = OffscreenCanvasImpl;
globalThis.ImageData = ImageDataImpl;
globalThis.createImageBitmap = createImageBitmapImpl;
globalThis.requestAnimationFrame = requestAnimationFrameImpl;

// import fsPromises from 'node:fs/promises';
// import { TransmuteRuntime } from './runtime';

// function getHashOfUri(uri: string) {
//   const hash = crypto.createHash('sha256');
//   hash.update(uri);
//   return hash.digest('hex');
// }

// function md5(content: string | NodeJS.ArrayBufferView) {
//   const hash = crypto.createHash('md5');
//   hash.update(content);
//   return hash.digest('hex');
// }

(async function main() {
  try {
    const now = performance.now();
    logger.info('Start the TransmuteRuntime entry');

    // Initialize the OffscreenCanvas polyfill.
    await InitializeOffscreenCanvas({ loadSystemFonts: true });
    logger.info(`The Skia initialization takes ${performance.now() - now}ms`);

    connectRenderer();
    // runExample();

    // Create runtime.
    // const runtime = new TransmuteRuntime({
    //   /// Implements the below hooks.
    //   onReadTextFile(pathname: string) {
    //     return fsPromises.readFile(pathname, 'utf8');
    //   },
    //   onReadBinaryFile(pathname: string) {
    //     return fsPromises.readFile(pathname);
    //   },
    //   async isResourceCached(runtime, uri: string): Promise<[boolean, string?]> {
    //     const cacheDir = runtime.readyContext.applicationCacheDirectory;
    //     const filename = getHashOfUri(uri);
    //     const cachedPath = path.join(cacheDir, filename);
    //     try {
    //       await fsPromises.access(cachedPath);
    //       return [true, cachedPath];
    //     } catch (_err) {
    //       return [false];
    //     }
    //   },
    //   async cacheResource(runtime, uri: string, content: string | NodeJS.ArrayBufferView) {
    //     try {
    //       const cacheDir = runtime.readyContext.applicationCacheDirectory;
    //       const filename = getHashOfUri(uri);
    //       const contentPath = path.join(cacheDir, filename);
    //       await fsPromises.writeFile(contentPath, content);
    //       const md5filePath = path.join(cacheDir, `${filename}.md5`);
    //       await fsPromises.writeFile(md5filePath, md5(content));
    //     } catch (err) {
    //       logger.error('failed to cache resource', err, uri);
    //     }
    //   },
    // });
    // runtime.start();
    new TransmuteRuntime2();
    logger.info('Started Transmute Runtime, it takes', performance.now() - now, 'ms');
  } catch (err) {
    logger.error(`failed to start the runtime, the error is: ${err?.stack || err}`);
  }
})();
