/**
 * A patch to the Node.js TextDecoder.
 *
 * Node.js TextDecoder with samll ICU doesn't support ascii encoding, however the fontkit library depends on
 * the ASCII decoder to parse the font file headers.
 */
import './polyfills/textdecoder';
import minimist from 'minimist';
import * as logger from '@transmute/logger';
import { getClientContext } from '@transmute/env';

const args = minimist(process.argv.slice(1));
const clientContext = getClientContext();
logger.info('The command line arguments:', args);

const bootstrapStart = performance.now();
const id = args.id || 'unknown';
logger.info(`Starting the JavaScript runtime(${process.pid}) => ${id}`, process.argv);
process.title = `TrScript ${id}`;

import './polyfills';
import {
  connectRenderer,
  getWebGLRenderingContext,
  requestGpuBusyCallback,
} from './bindings/renderer';
import { prepareXRSystem } from './webxr';
import { TransmuteRuntime2 } from './runtime2';

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
  return setTimeout(fn, seconds * 1000);
}

bootwait(async function main() {
  try {
    const runtimeStart = performance.now();
    logger.info('The Node.js runtime bootstrap takes', runtimeStart - bootstrapStart, 'ms');
    logger.info('Starting the TransmuteRuntime entry with the environment:');
    for (const [key, value] of Object.entries(process.env)) {
      logger.info(`  ${key}: ${value}`);
    }

    logger.info('The context init is:', clientContext);
    if (!connectRenderer(clientContext)) {
      throw new Error('failed to connect to the renderer.');
    }
    await prepareXRSystem();

    runtime = new TransmuteRuntime2(getWebGLRenderingContext());
    runtime.start(clientContext.url, clientContext.id);

    const initializedEnd = performance.now();
    logger.info('Time summary:', {
      bootstrap: runtimeStart - bootstrapStart,
      initialize: initializedEnd - runtimeStart,
      total: initializedEnd - bootstrapStart,
    });
  } catch (err) {
    logger.error('failed to start the runtime, occurs an error:', err);
  }
});
