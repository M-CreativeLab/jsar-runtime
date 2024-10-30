import minimist from 'minimist';
import * as env from '@transmute/env';
import { reportDocumentEvent, addDocumentRequestListener } from '@transmute/messaging';

import * as Navigator from './navigator';
import {
  connectRenderer,
  getWebGLRenderingContext
} from './bindings/renderer';
import { loadPolyfills } from './polyfills';
import { initDevice as initXRDevice, createXRSystem } from './webxr';
import { TransmuteRuntime2 } from './runtime2';

const bootstrapStarted = performance.now();
loadPolyfills();

const args = minimist(process.argv.slice(1));
const id = args.id || 'unknown';
console.info(`Starting the JavaScript runtime(${process.pid}) => ${id}`, process.argv);

/**
 * FIXME: The unhandled rejection means network or other async operations failed, we just make a warning and keep the
 * application running still.
 */
process.on('unhandledRejection', (reason) => {
  const message: string = reason instanceof Error ? (reason.stack || reason.message) : String(reason);
  console.warn('#');
  console.warn('# Unhandled rejection:');
  console.warn('#');
  console.warn(message);
  console.warn('\n');
});
process.on('uncaughtException', (e) => {
  const message: string = e instanceof Error ? (e.stack || e.message) : String(e);
  console.warn('#');
  console.warn('# Unhandled rejection:');
  console.warn('#');
  console.warn(message);
  console.warn('Exiting the process after 500ms...\n');
  setTimeout(() => process.exit(1), 500);
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
    initXRDevice();
    Navigator.configureXRSystem(createXRSystem());

    const gl = getWebGLRenderingContext();
    Navigator.configureGL(gl);
    reportDocumentEvent(id, 'beforeloading');

    /**
     * Create the runtime.
     */
    const runtime = new TransmuteRuntime2(gl, id);
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
