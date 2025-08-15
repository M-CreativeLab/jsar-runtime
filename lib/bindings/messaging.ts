// Compatibility layer for the old messaging system
// Most functionality is now handled by the C++ Navigator implementation

import { navigator } from '../navigator';

// Get native messaging components  
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

  readonly url: string;
  readonly documentId: number;
  readonly disableCache?: boolean;
  readonly isPreview?: boolean;
  readonly runScripts?: string;
  readonly defaultHTTPHeaders?: string;

  constructor(init: DocumentRequestInit) {
    super(DocumentRequestEvent.Name);
    this.url = init.url;
    this.documentId = init.documentId;
    this.disableCache = init.disableCache;
    this.isPreview = init.isPreview;
    this.runScripts = init.runScripts;
    this.defaultHTTPHeaders = init.defaultHTTPHeaders;
  }
}

// Native event listener for legacy messaging compatibility
function onNativeEventListener(_eventId: number, eventType: number, peerId: number, message: string) {
  switch (eventType) {
    case EventType.RpcRequest:
      // RPC requests are now handled by C++ Navigator
      break;
    case EventType.RpcResponse:
      // Handle RPC responses for compatibility with old Promise-based API
      {
        const waitCallback = RpcRequestWaitlist.get(_eventId);
        if (waitCallback) {
          RpcRequestWaitlist.delete(_eventId);
          waitCallback(message);
        }
      }
      break;
    case EventType.DocumentRequest:
      // DocumentRequest handling for compatibility
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
export function dispatchEventToHost(eventName: string, eventDetail?: unknown): number {
  const eventType = eventNameToType(eventName);
  const eventInit: { type: number, detail?: string } = { type: eventType };
  if (eventDetail) {
    eventInit.detail = JSON.stringify(eventDetail);
  }
  return nativeEventTarget.dispatchEvent(eventInit);
}

/**
 * Add a document request listener
 */
export function addDocumentRequestListener(listener: (event: DocumentRequestEvent) => void) {
  addEventListener('documentrequest', listener as EventListener);
}

/**
 * Report document event
 */
export function reportDocumentEvent(documentId: string | number, eventType: string) {
  // Convert to number if string
  const docId = typeof documentId === 'string' ? parseInt(documentId, 10) : documentId;
  
  // Use the legacy event system for compatibility
  dispatchEventToHost('documentEvent', {
    documentId: docId,
    eventType: eventType,
    timestamp: Date.now()
  });
}

/**
 * Make RPC call - now uses C++ Navigator for better performance
 * but maintains original Promise-based API for compatibility
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const makeRpcCall = function makeRpcCallToNative(method: string, args: any[]) {
  // Use the C++ Navigator for RPC calls which provides better performance
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  return new Promise<any>((resolve, reject) => {
    try {
      // eslint-disable-next-line @typescript-eslint/no-explicit-any
      (navigator as any).makeRpcCall(method, args, (success: boolean, response: string) => {
        if (success) {
          try {
            const data = JSON.parse(response);
            resolve(data);
          } catch (err) {
            resolve(response); // Return raw response if not JSON
          }
        } else {
          reject(new Error(response || 'RPC call failed'));
        }
      });
    } catch (err) {
      reject(err);
    }
  });
};

/**
 * Dispose function for cleanup
 */
export function dispose() {
  if (typeof nativeEventTarget?.dispose === 'function') {
    nativeEventTarget.dispose();
  }
  // Clear any pending RPC requests
  RpcRequestWaitlist.clear();
}