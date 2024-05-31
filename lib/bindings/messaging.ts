import * as logger from './logger';

const binding = process._linkedBinding('transmute:messaging');
const nativeEventTarget = new binding.NativeEventTarget(onNativeEventListener);
const eventTarget = new EventTarget();
const RpcRequestWaitlist = new Map<number, (responseText: string) => void>();
let eventId = 0;

enum EventType {
  rpcRequest = 0x100,
  rpcResponse = 0x101,
  message = 0x200,
  error = 0x210,
  close = 0x220,
  xsmlRequest = 0x300,
  xsmlEvent = 0x301,
}

function getEventType(type: string): EventType {
  switch (type) {
    case 'rpc':
    case 'rpcRequest': // 'rpc' is an alias of 'rpc.request
      return EventType.rpcRequest;
    case 'rpcResponse':
      return EventType.rpcResponse;
    case 'message':
      return EventType.message;
    case 'error':
      return EventType.error;
    case 'close':
      return EventType.close;
    case 'xsmlRequest':
      return EventType.xsmlRequest;
    case 'xsmlEvent':
      return EventType.xsmlEvent;
    default:
      throw new TypeError(`unknown event type: ${type}`);
  }
}

export const XsmlRequestEventName = 'xsmlRequest';

export type XsmlRequestInit = {
  url: string;
  sessionId: number;
  disableCache?: boolean;
  isPreview?: boolean;
  runScripts?: string;
};

export class XsmlRequestEvent extends Event {
  url: string;
  sessionId: number;
  disableCache: boolean = false;
  isPreview: boolean = false;
  runScripts: string = 'dangerously';

  constructor(init: XsmlRequestInit) {
    super(XsmlRequestEventName);

    this.url = init.url;
    this.sessionId = init.sessionId;
    this.disableCache = init.disableCache || false;
    this.isPreview = init.isPreview || false;
    this.runScripts = init.runScripts || 'dangerously';
  }

  get [Symbol.toStringTag]() {
    return JSON.stringify({
      url: this.url,
      sessionId: this.sessionId,
      disableCache: this.disableCache,
      isPreview: this.isPreview,
      runScripts: this.runScripts,
    }, null, 2);
  }
}

function onNativeEventListener(id: number, type: number, message: string) {
  if (type === EventType.rpcResponse) {
    const callback = RpcRequestWaitlist.get(id);
    if (typeof callback === 'function') {
      RpcRequestWaitlist.delete(id);
      callback(message);
    }
    return;
  } else if (type === EventType.xsmlRequest) {
    const req = <XsmlRequestInit>JSON.parse(message);
    if (!req.url) {
      logger.warn('invalid xsml request, `url` is required.');
      return;
    }
    if (!req.sessionId) {
      logger.warn('invalid xsml request, `sessionId` is required.');
      return;
    }
    const requestEvent = new XsmlRequestEvent(req);
    eventTarget.dispatchEvent(requestEvent);
  } else if (type === EventType.message) {
    const eventData = JSON.parse(message);
    const newEvent = new CustomEvent(eventData.subType, { detail: eventData.message });
    eventTarget.dispatchEvent(newEvent);
  } else {
    logger.warn(`unknown event type: ${type}`);
  }
}

export function addEventListener(
  type: string,
  callback: EventListenerOrEventListenerObject,
  options?: boolean | AddEventListenerOptions
) {
  return eventTarget.addEventListener(type, callback, options);
}

export function addXsmlRequestListener(listener: (event: XsmlRequestEvent) => void) {
  return eventTarget.addEventListener(XsmlRequestEventName, listener);
}

export const removeEventListener = eventTarget.removeEventListener.bind(eventTarget);
// export const dispatchEvent = eventTarget.dispatchEvent.bind(eventTarget);
// export const emit = function emitMessagingEvent(type: string, message?: string) {
//   if (typeof type !== 'string') {
//     throw new TypeError('event type must be a string');
//   }
//   return eventTarget.dispatchEvent(new CustomEvent(type, { detail: message }));
// };

export const dispatchEvent = function dispatchEventToNative(event: CustomEvent): number {
  return nativeEventTarget.dispatchEvent({
    type: getEventType(event.type),
    detail: JSON.stringify(event.detail),
  });
};

export const dispatchXsmlEvent = function (id: number, eventType: 'loaded' | 'fcp' | 'error') {
  return dispatchEvent(
    new CustomEvent('xsmlEvent', {
      detail: {
        id,
        eventType,
      },
    })
  );
};

export const makeRpcCall = function makeRpcCallToNative(method: string, args: any[]) {
  const detail = { method, args };
  const id = dispatchEvent(new CustomEvent('rpcRequest', { detail }));
  return new Promise<any>((resolve, reject) => {
    RpcRequestWaitlist.set(id, (responseText: string) => {
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
export const dispose = () => {
  if (typeof nativeEventTarget?.dispose === 'function') {
    nativeEventTarget.dispose();
  }
};
