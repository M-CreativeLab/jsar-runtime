// Simplified messaging layer - most functionality moved to C++ Navigator
const { NativeEventTarget } = process._linkedBinding('transmute:messaging');

const nativeEventTarget = new NativeEventTarget(onNativeEventListener);
const eventTarget = new EventTarget();

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
      throw new Error(`Unknown event type: ${type}`);
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
  readonly url: string;
  readonly documentId: number;
  readonly disableCache?: boolean;
  readonly isPreview?: boolean;
  readonly runScripts?: string;
  readonly defaultHTTPHeaders?: string;

  constructor(init: DocumentRequestInit) {
    super('documentrequest');
    this.url = init.url;
    this.documentId = init.documentId;
    this.disableCache = init.disableCache;
    this.isPreview = init.isPreview;
    this.runScripts = init.runScripts;
    this.defaultHTTPHeaders = init.defaultHTTPHeaders;
  }
}

// Simplified event listener - now mostly passes events through to C++ Navigator
function onNativeEventListener(_eventId: number, eventType: number, peerId: number, message: string) {
  switch (eventType) {
    case EventType.RpcRequest:
    case EventType.RpcResponse:
      // RPC handling now done in C++ Navigator - no need to handle here
      break;
    case EventType.DocumentRequest:
      // DocumentRequest handling now done in C++ Navigator
      // Still dispatch for legacy compatibility
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
 * @param type the event type, such as "documentRequest".
 * @param listener the event listener to be called when the event is triggered.
 */
export function addEventListener(type: string, listener: EventListener) {
  eventTarget.addEventListener(type, listener);
}

/**
 * Remove a listener for the specified event type.
 * 
 * @param type the event type, such as "documentRequest".
 * @param listener the event listener to be removed.
 */
export function removeEventListener(type: string, listener: EventListener) {
  eventTarget.removeEventListener(type, listener);
}

/**
 * Dispatch an event to the host process.
 * 
 * @param eventName the event name, such as "documentRequest".
 * @param eventDetail the event detail.
 * @returns the event id.
 */
export function dispatchEventToHost(eventName: string, eventDetail?: any): number {
  const eventType = eventNameToType(eventName);
  const eventInit: { type: number, detail?: string } = { type: eventType };
  if (eventDetail) {
    eventInit.detail = JSON.stringify(eventDetail);
  }
  return nativeEventTarget.dispatchEvent(eventInit);
}

/**
 * NOTE: RPC calls are now handled by the C++ Navigator class.
 * This function is kept for backward compatibility but deprecated.
 * 
 * @deprecated Use navigator.makeRpcCall() instead
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const makeRpcCall = function makeRpcCallToNative(method: string, args: any[]) {
  console.warn('makeRpcCall from messaging is deprecated. Use navigator.makeRpcCall() instead.');
  throw new Error('RPC calls are now handled by the C++ Navigator. Use navigator.makeRpcCall() instead.');
};

/**
 * Dispose the native listener, this drops the reference of the low-level tsfn instance.
 */
export function dispose() {
  if (typeof nativeEventTarget?.dispose === 'function') {
    nativeEventTarget.dispose();
  }
}