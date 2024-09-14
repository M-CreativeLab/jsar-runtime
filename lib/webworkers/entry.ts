import { workerData, parentPort } from 'node:worker_threads';
import { ResourceLoaderOnTransmute } from '../runtime2/jsardom/ResourceLoader';
import type { WorkerRequest } from './worker';
const { WorkerContext } = process._linkedBinding('transmute:dom');

const workerRequest: WorkerRequest = workerData;
const workerContext = new WorkerContext(workerRequest?.options);
workerContext.setResourceLoader(new ResourceLoaderOnTransmute());
console.info('Started a new worker entry', workerRequest);

parentPort.on('message', (message) => {
  // workerScripting.dispatchEvent(message);
});

// Executing the worker script
if (workerRequest.code) {
  const codeBlob = workerRequest.code;
  codeBlob.text().then((codeText) => {
    workerContext.start(codeText);
  });
}
