const { NativeEventTarget } = process._linkedBinding('transmute:messaging');

const nativeEventTarget = new NativeEventTarget(onNativeEventListener);
const eventTarget = new EventTarget();
const RpcRequestWaitlist = new Map<number, (responseText: string) => void>();

enum EventType {
  RpcRequest = NativeEventTarget.EventTypes.RpcRequest,
  RpcResponse = NativeEventTarget.EventTypes.RpcResponse,
  DocumentRequest = NativeEventTarget.EventTypes.DocumentRequest,
  DocumentEvent = NativeEventTarget.EventTypes.DocumentEvent,
}

function eventNameToType(type: string): EventType {
  switch (type) {
    case 'rpcRequest':
      return EventType.RpcRequest;
    case 'rpcResponse':
      return EventType.RpcResponse;
    case 'documentRequest':
      return EventType.DocumentRequest;
    case 'documentEvent':
      return EventType.DocumentEvent;
    default:
      throw new TypeError(`unknown event type: ${type}`);
  }
}

export type DocumentRequestInit = {
  url: string;
  documentId: number;
  disableCache?: boolean;
  isPreview?: boolean;
  runScripts?: string;
  defaultHTTPHeaders?: string;
};

export class DocumentRequestEvent extends Event {
  static Name = 'documentRequest';

  url: string;
  documentId: number;
  disableCache: boolean = false;
  isPreview: boolean = false;
  runScripts: string = 'dangerously';
  defaultHTTPHeaders: string;

  constructor(init: DocumentRequestInit) {
    super(DocumentRequestEvent.Name);

    this.url = init.url;
    this.documentId = init.documentId;
    this.disableCache = init.disableCache || false;
    this.isPreview = init.isPreview || false;
    this.runScripts = init.runScripts || 'dangerously';
    this.defaultHTTPHeaders = init.defaultHTTPHeaders || '';
  }

  get [Symbol.toStringTag]() {
    return JSON.stringify({
      url: this.url,
      documentId: this.documentId,
      disableCache: this.disableCache,
      isPreview: this.isPreview,
      runScripts: this.runScripts,
    }, null, 2);
  }
}

function onNativeEventListener(_eventId: number, eventType: number, peerId: number, message: string) {
  switch (eventType) {
    case EventType.RpcRequest:
      // Client-side will never receive a RPC request.
      break;
    case EventType.RpcResponse:
      {
        const callback = RpcRequestWaitlist.get(peerId);
        if (typeof callback === 'function') {
          RpcRequestWaitlist.delete(peerId);
          callback(message);
        }
      }
      break;
    case EventType.DocumentRequest:
      {
        let init: DocumentRequestInit;
        try {
          init = JSON.parse(message);
        } catch (_err) {
          // Do nothing.
        }
        if (init && init.url && init.documentId) {
          eventTarget.dispatchEvent(new DocumentRequestEvent(init));
        } else {
          console.warn('Invalid document request, the JSON source is:', message);
        }
      }
      break;
    default:
      console.warn('Unknown or unsupported (at client-side) event type:', eventType);
      break;
  }
}

/**
 * Add a listener for the specified event type.
 * 
 * @param type the type name of the event to listen for.
 * @param callback the callback function that receives a notification when an event of the specified type occurs.
 * @param options an options object that specifies characteristics about the event listener.
 * @returns 
 */
export function addEventListener(
  type: string,
  callback: EventListenerOrEventListenerObject,
  options?: boolean | AddEventListenerOptions
) {
  return eventTarget.addEventListener(type, callback, options);
}

/**
 * Add a listener for document request event.
 * @param callback 
 * @returns 
 */
export function addDocumentRequestListener(callback: (event: DocumentRequestEvent) => void) {
  return eventTarget.addEventListener(DocumentRequestEvent.Name, callback);
}

/**
 * Remove a listener for the specified event type.
 */
export const removeEventListener = eventTarget.removeEventListener.bind(eventTarget);

/**
 * Dispatch an event to the host process.
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
function dispatchEventToHost(type: 'rpcRequest', detail: { method: string, args: any[] }): number;
// eslint-disable-next-line @typescript-eslint/no-explicit-any
function dispatchEventToHost(type: 'rpcResponse', detail: { success: boolean, data?: any, message?: string }): number;
function dispatchEventToHost(type: 'documentEvent', detail: { documentId: number, eventType: number, timestamp: number });
// eslint-disable-next-line @typescript-eslint/no-explicit-any
function dispatchEventToHost(type: 'rpcRequest' | 'rpcResponse' | 'documentEvent', detail: any): number {
  return nativeEventTarget.dispatchEvent({
    type: eventNameToType(type),
    detail: JSON.stringify(detail),
  });
}

/**
 * Report a document event to the host process.
 * @param id 
 * @param eventType 
 * @returns 
 */
export function reportDocumentEvent(
  documentId: number,
  /**
   * TODO: deprecate the "loaded" type, reason: not standard.
   * 
   * See: https://web.dev/articles/lcp
   */
  eventName: 'beforeloading' | 'load' | 'loaded' | 'DOMContentLoaded' | 'error'
) {
  // Just duplicate a "load" event when receiving "loaded" event.
  if (eventName === 'loaded') {
    reportDocumentEvent(documentId, 'load');
  }

  const eventType = NativeEventTarget.DocumentEventTypes[eventName];
  if (typeof eventType !== 'number') {
    throw new TypeError(`Unknown document event type: ${eventName}`);
  }
  return dispatchEventToHost('documentEvent', {
    documentId,
    eventType,
    timestamp: Date.now(),
  });
}

/**
 * Make a new rpc call to the host process, it returns a promise that resolves with the response data.
 * 
 * TODO: support introspection for the host SDK generation.
 * 
 * @param method the remote method name.
 * @param args the arguments to pass to the remote method.
 * @returns a promise that resolves with the response data.
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const makeRpcCall = function makeRpcCallToNative(method: string, args: any[]) {
  const reqId = dispatchEventToHost('rpcRequest', { method, args });
  if (typeof reqId !== 'number') {
    throw new Error('Failed to make rpc call to the host process: invalid request id.');
  }

  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  return new Promise<any>((resolve, reject) => {
    RpcRequestWaitlist.set(reqId, (responseText: string) => {
      // eslint-disable-next-line @typescript-eslint/no-explicit-any
      let json: any;
      try {
        json = JSON.parse(responseText) || {};
      } catch (err) {
        json = {
          success: false,
          message: `Failed to parse response(${typeof responseText} '${responseText}') from rpc call: ${err}.`,
        };
      }
      if (json.success === true) {
        let data = json.data;
        if (typeof data === 'string') {
          let dataParsingError: Error;
          try {
            data = JSON.parse(json.data);
          } catch (err) {
            dataParsingError = err;
          }
          if (dataParsingError) {
            return reject(dataParsingError);
          }
        }
        resolve(data);
      } else {
        reject(new Error(json.message || 'Unknown error when receiving rpc response.'));
      }
    });
  });
};

/**
 * Dispose the native listener, this drops the reference of the low-level tsfn instance.
 */
export function dispose() {
  if (typeof nativeEventTarget?.dispose === 'function') {
    nativeEventTarget.dispose();
  }
  RpcRequestWaitlist.clear();
}
