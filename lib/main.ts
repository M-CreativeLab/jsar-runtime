import minimist from 'minimist';
import * as logger from '@transmute/logger';
import { getClientContext } from '@transmute/env';

const args = minimist(process.argv.slice(1));
const clientContext = getClientContext();

logger.info('The command line arguments:', args);

const bootstrapStart = performance.now();
const id = args.id || 'unknown';
logger.info('Starting the JavaScript runtime => ' + id, process.argv);
process.title = `TrScript ${id}`;

setInterval(() => {
  logger.info(`${id} is running...`)
}, 2000);

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

// import { InitializeOffscreenCanvas } from './polyfills'; // load polyfills after the global error handler
// import { connectRenderer, requestGpuBusyCallback } from './bindings/renderer';
// import { prepareXRSystem } from './webxr';
// import { TransmuteRuntime2 } from './runtime2';

// let runtime: TransmuteRuntime2;
// requestGpuBusyCallback(() => {
//   if (runtime && typeof runtime.onGpuBusy === 'function') {
//     runtime.onGpuBusy();
//   } else {
//     process.exit(1);
//   }
// });

(async function main() {
  try {
    const runtimeStart = performance.now();
    logger.info('The Node.js runtime bootstrap takes', runtimeStart - bootstrapStart, 'ms');
    logger.info('Starting the TransmuteRuntime entry with the environment:');
    for (const [key, value] of Object.entries(process.env)) {
      logger.info(`  ${key}: ${value}`);
    }

    // createEnv();
    // const init = getRuntimeInit();
    logger.info('The context init is:', clientContext);

    // Initialize the OffscreenCanvas polyfill.
    // await InitializeOffscreenCanvas({ loadSystemFonts: true });
    // logger.info(`The Skia initialization takes ${performance.now() - runtimeStart}ms`);

    // connectRenderer();
    // await prepareXRSystem();

    // runtime = new TransmuteRuntime2();
    // runtime.start();

    const initializedEnd = performance.now();
    logger.info('Finished TransmuteRuntime2() instance creation');
    logger.info('Time summary:', {
      bootstrap: runtimeStart - bootstrapStart,
      initialize: initializedEnd - runtimeStart,
      total: initializedEnd - bootstrapStart,
    });
  } catch (err) {
    logger.error('failed to start the runtime, occurs an error:', err);
  }
})();
