/**
 * Browser Pollyfills for Node.js
 */
import * as undici from 'undici';
import { XMLHttpRequestImpl } from './xhr';
import { WorkerImpl } from './webworkers/worker';

export function loadPolyfills() {
  // Load Web APIs
  process._linkedBinding('transmute:dom');

  globalThis['WebSocket'] = undici.WebSocket as unknown as typeof WebSocket;
  globalThis['XMLHttpRequest'] = XMLHttpRequestImpl as any;

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
}
