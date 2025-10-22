import path from 'node:path';
import * as WorkerThreads from 'node:worker_threads';
import { type Blob, resolveObjectURL } from 'node:buffer';
import { ErrorEvent } from './events';

export type WorkerRequest = {
  baseURI: string;
  requestUrl: string;
  scriptSource?: Blob | undefined;
  options?: WorkerOptions;
};

export class WorkerImpl {
  #handle: WorkerThreads.Worker;
  #request: WorkerRequest;

  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  onmessage: (ev: MessageEvent) => any;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  onmessageerror: (ev: MessageEvent) => any;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  onerror: (ev: ErrorEvent) => any;

  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  postMessage(message: any, transfer: Transferable[]): void;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  postMessage(message: any, options?: StructuredSerializeOptions): void;
  postMessage(message: unknown, _options?: unknown): void {
    this.#handle?.postMessage(message);
  }

  terminate(): void {
    this.#handle?.terminate();
  }

  constructor(url: string | URL, options?: WorkerOptions) {
    if (typeof document === 'undefined' || document.baseURI === undefined) {
      throw new Error('Workers are only supported in the browser environment');
    }

    const workerScriptUrl = url instanceof URL ? url.href : url;
    this.#request = {
      baseURI: document.baseURI,
      requestUrl: workerScriptUrl,
      options,
    };
    if (workerScriptUrl.startsWith('blob:')) {
      this.#request.scriptSource = resolveObjectURL(workerScriptUrl);
    }
  }

  start() {
    const entryPath = path.resolve(__dirname, './jsar-webworkers-entry.js');
    try {
      this.#handle = new WorkerThreads.Worker(entryPath, {
        workerData: this.#request,
      });
    } catch (err) {
      console.error('Failed to start a worker:', err);
    }
    if (this.#handle) {
      this.#initHandle();
    }
  }

  #initHandle() {
    this.#handle.on('message', (message) => {
      if (typeof this.onmessage === 'function') {
        const event = new MessageEvent('message', { data: message });
        this.onmessage(event);
      }
    });
    this.#handle.on('messageerror', (error) => {
      if (typeof this.onmessageerror === 'function') {
        const event = new MessageEvent('messageerror', { data: error });
        this.onmessageerror(event);
      }
    });
    this.#handle.on('error', (error) => {
      console.warn('Occurred error in worker thread:', error);
      if (typeof this.onerror === 'function') {
        const event = new ErrorEvent('error', {
          message: error.message,
          error
        });
        this.onerror(event);
      }
    });
  }
}
