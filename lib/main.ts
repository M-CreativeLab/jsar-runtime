import minimist from 'minimist';
import * as env from '@transmute/env';
import { reportDocumentEvent, addDocumentRequestListener } from '@transmute/messaging';

import {
  connectRenderer,
  getWebGLRenderingContext,
  requestGpuBusyCallback,
} from './bindings/renderer';
import { loadPolyfills } from './polyfills';
import { prepareXRSystem } from './webxr';
import { TransmuteRuntime2 } from './runtime2';

const bootstrapStarted = performance.now();
loadPolyfills();

const args = minimist(process.argv.slice(1));
const id = args.id || 'unknown';
console.info(`Starting the JavaScript runtime(${process.pid}) => ${id}`, process.argv);

let runtime: TransmuteRuntime2;
requestGpuBusyCallback(() => {
  // if (runtime && typeof runtime.onGpuBusy === 'function') {
  //   runtime.onGpuBusy();
  // } else {
  //   process.exit(1);
  // }
});

/**
 * FIXME: The unhandled rejection means network or other async operations failed, we just make a warning and keep the
 * application running still.
 */
process.on('unhandledRejection', (reason) => {
  console.warn('Received an unhandled rejection:', reason);
});

function bootwait(fn: () => void) {
  let seconds = parseInt(process.env.JSAR_BOOTWAIT, 10);
  if (isNaN(seconds)) {
    seconds = 0;
  }
  if (seconds == 0) {
    fn();
  } else {
    setTimeout(fn, seconds * 1000);
  }
}

bootwait(async function main() {
  try {
    const runtimeStarted = performance.now();
    const clientContext = env.getClientContext();
    env.printSummary();

    if (!connectRenderer(clientContext)) {
      throw new Error('failed to connect to the renderer.');
    }
    await prepareXRSystem();
    reportDocumentEvent(id, 'beforeloading');

    runtime = new TransmuteRuntime2(getWebGLRenderingContext(), id);
    await runtime.prepare();
    const initializedEnded = performance.now();

    console.info('Time summary:', {
      bootstrap: runtimeStarted - bootstrapStarted,
      initialize: initializedEnded - runtimeStarted,
      total: initializedEnded - bootstrapStarted,
    });

    // Start handling the request.
    addDocumentRequestListener(async event => {
      try {
        await runtime.start(event.url);
      } catch (err) {
        console.error('failed to handle the request event, occurs an error:', err);
        reportDocumentEvent(id, 'error');
        process.exit(1);
      }
    });
  } catch (err) {
    console.error('failed to start the runtime, occurs an error:', err);
    reportDocumentEvent(id, 'error');
  }
});
