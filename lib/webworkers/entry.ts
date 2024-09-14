import { workerData, parentPort } from 'node:worker_threads';
import { ResourceLoaderOnTransmute } from '../runtime2/jsardom/ResourceLoader';
import type { WorkerRequest } from './worker';
const { WorkerContext } = process._linkedBinding('transmute:dom');

{
  // Define the global object before creating the WorkerContext.
  Object.defineProperty(globalThis, 'postMessage', {
    value: parentPort.postMessage.bind(parentPort),
    configurable: true,
    writable: true,
  });
}

const workerRequest: WorkerRequest = workerData;
const workerContext = new WorkerContext(workerRequest.baseURI, workerRequest?.options);
workerContext.setResourceLoader(new ResourceLoaderOnTransmute());

parentPort.on('message', (message) => {
  workerContext.dispatchEvent(new MessageEvent('message', { data: message }));
});
parentPort.on('messageerror', (error) => {
  workerContext.dispatchEvent(new ErrorEvent('messageerror', { error }));
});
parentPort.on('error', (error) => {
  workerContext.dispatchEvent(new ErrorEvent('error', { error }));
});

// Executing the worker script
if (workerRequest.scriptSource) {
  workerRequest.scriptSource.text().then((t) => workerContext.startFromSource(t));
} else {
  workerContext.start(workerRequest.requestUrl);
}
