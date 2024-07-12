/**
 * A patch to the Node.js TextDecoder.
 *
 * Node.js TextDecoder with samll ICU doesn't support ascii encoding, however the fontkit library depends on
 * the ASCII decoder to parse the font file headers.
 */
import './polyfills/textdecoder';
import minimist from 'minimist';
import { getClientContext } from '@transmute/env';

import './polyfills';
import {
  connectRenderer,
  getWebGLRenderingContext,
  requestGpuBusyCallback,
} from './bindings/renderer';
import { prepareXRSystem } from './webxr';
import { TransmuteRuntime2 } from './runtime2';
import { dispatchXsmlEvent } from '@transmute/messaging';

const bootstrapStart = performance.now();
const args = minimist(process.argv.slice(1));
const clientContext = getClientContext();
const id = args.id || 'unknown';
console.info(`Starting the JavaScript runtime(${process.pid}) => ${id}`, process.argv);
process.title = `TrScript ${id}`;

let runtime: TransmuteRuntime2;
requestGpuBusyCallback(() => {
  // if (runtime && typeof runtime.onGpuBusy === 'function') {
  //   runtime.onGpuBusy();
  // } else {
  //   process.exit(1);
  // }
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
    const runtimeStart = performance.now();
    console.info('The context init is:', clientContext);
    if (!connectRenderer(clientContext)) {
      throw new Error('failed to connect to the renderer.');
    }
    await prepareXRSystem();

    runtime = new TransmuteRuntime2(getWebGLRenderingContext());
    runtime.start(clientContext.url, clientContext.id);

    const initializedEnd = performance.now();
    console.info('Time summary:', {
      bootstrap: runtimeStart - bootstrapStart,
      initialize: initializedEnd - runtimeStart,
      total: initializedEnd - bootstrapStart,
    });
  } catch (err) {
    console.error('failed to start the runtime, occurs an error:', err);
    dispatchXsmlEvent(id, 'error');
  }
});
