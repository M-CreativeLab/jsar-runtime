import path from 'node:path';
import * as WorkerThreads from 'node:worker_threads';
import { resolveObjectURL } from 'node:buffer';

export type WorkerRequest = {
  url: string;
  code?: Blob;
  options?: WorkerOptions;
};

export class WorkerImpl extends EventTarget implements Worker {
  #handle: WorkerThreads.Worker;

  onmessage: (this: Worker, ev: MessageEvent) => any;
  onmessageerror: (this: Worker, ev: MessageEvent) => any;
  onerror: (this: AbstractWorker, ev: ErrorEvent) => any;

  postMessage(message: any, transfer: Transferable[]): void;
  postMessage(message: any, options?: StructuredSerializeOptions): void;
  postMessage(message: unknown, options?: unknown): void {
    this.#handle?.postMessage(message);
  }
  terminate(): void {
    this.#handle?.terminate();
  }

  constructor(url: string | URL, options?: WorkerOptions) {
    super();

    const workerScriptUrl = url instanceof URL ? url.href : url;
    const workerRequest: WorkerRequest = {
      url: workerScriptUrl,
      options,
    };
    if (workerScriptUrl.startsWith('blob:')) {
      workerRequest.code = resolveObjectURL(workerScriptUrl);
    }

    const entryPath = path.resolve(__dirname, './jsar-webworkers-entry.js');
    try {
      this.#handle = new WorkerThreads.Worker(entryPath, {
        workerData: workerRequest,
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
      const event = new MessageEvent('message', { data: message });
      if (typeof this.onmessage === 'function') {
        this.onmessage(event);
      }
      this.dispatchEvent(event);
    });
    this.#handle.on('error', (error) => {
      const event = new ErrorEvent('error', { error });
      if (typeof this.onerror === 'function') {
        this.onerror(event);
      }
      this.dispatchEvent(event);
    });
  }
}
