import * as logger from './logger';

const binding = process._linkedBinding('transmute:messaging');
const nativeEventTarget = new binding.NativeEventTarget();
const eventTarget = new EventTarget();
const RpcRequestWaitlist = new Map<number, (responseText: string) => void>();
let eventId = 0;

enum EventType {
  rpcRequest = 0x100,
  rpcResponse = 0x101,
  message = 0x200,
  error = 0x210,
  close = 0x220,
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
    default:
      throw new TypeError(`unknown event type: ${type}`);
  }
}

nativeEventTarget.setNativeEventListener(onNativeEventListener);
function onNativeEventListener(id: number, type: number, message: string) {
  if (type === EventType.rpcResponse) {
    const callback = RpcRequestWaitlist.get(id);
    if (typeof callback === 'function') {
      RpcRequestWaitlist.delete(id);
      callback(message);
    }
    return;
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

export const removeEventListener = eventTarget.removeEventListener.bind(eventTarget);
// export const dispatchEvent = eventTarget.dispatchEvent.bind(eventTarget);
// export const emit = function emitMessagingEvent(type: string, message?: string) {
//   if (typeof type !== 'string') {
//     throw new TypeError('event type must be a string');
//   }
//   return eventTarget.dispatchEvent(new CustomEvent(type, { detail: message }));
// };

export const dispatchEvent = function dispatchEventToNative(event: CustomEvent) {
  const id = eventId++;
  const type = getEventType(event.type);
  const message = JSON.stringify(event.detail);
  nativeEventTarget.dispatchEvent(id, type, message);
  return id;
};

export const makeRpcCall = function makeRpcCallToNative(method: string, args: any[]) {
  const detail = { method, args: JSON.stringify(args) };
  const id = dispatchEvent(new CustomEvent('rpcRequest', { detail }));
  return new Promise<any>((resolve, reject) => {
    RpcRequestWaitlist.set(id, (responseText: string) => {
      let json: any;
      try {
        json = JSON.parse(responseText) || {};
      } catch (err) {
        json = {
          success: false,
          message: 'Failed to parse response from rpc call.',
        };
      }
      if (json.success === true) {
        let data: any;
        let isDataParsed = false;
        let dataParsingError: Error;
        try {
          data = JSON.parse(json.data);
          isDataParsed = true;
        } catch (err) {
          dataParsingError = err;
        }
        if (!isDataParsed) {
          reject(dataParsingError);
        } else {
          resolve(data);
        }
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
