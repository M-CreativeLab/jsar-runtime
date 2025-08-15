// Compatibility layer for the old messaging system
// Most functionality is now handled by the C++ Navigator implementation

import { navigator } from '../navigator';
import { 
  addEventListener as addEventListenerCompat, 
  removeEventListener as removeEventListenerCompat,
  dispatchEventToHost as dispatchEventToHostCompat,
  DocumentRequestEvent as DocumentRequestEventCompat,
  DocumentRequestInit as DocumentRequestInitCompat
} from './messaging_simplified';

// Re-export types for compatibility
export type DocumentRequestInit = DocumentRequestInitCompat;
export class DocumentRequestEvent extends DocumentRequestEventCompat {}

// Compatibility functions that use the new C++ navigator where appropriate
export function addEventListener(type: string, listener: EventListener) {
  addEventListenerCompat(type, listener);
}

export function removeEventListener(type: string, listener: EventListener) {
  removeEventListenerCompat(type, listener);
}

export function dispatchEventToHost(eventName: string, eventDetail?: unknown): number {
  return dispatchEventToHostCompat(eventName, eventDetail);
}

/**
 * Add a document request listener - now uses the simplified messaging layer
 */
export function addDocumentRequestListener(listener: (event: DocumentRequestEvent) => void) {
  addEventListener('documentrequest', listener as EventListener);
}

/**
 * Report document event - now uses the C++ Navigator
 */
export function reportDocumentEvent(documentId: string | number, eventType: string) {
  // Convert to number if string
  const docId = typeof documentId === 'string' ? parseInt(documentId, 10) : documentId;
  
  // Use the navigator's document event handling
  if (navigator && (navigator as unknown as { nativeNavigator?: unknown }).nativeNavigator) {
    // Call native navigator's document event handling
    dispatchEventToHost('documentEvent', {
      documentId: docId,
      eventType: eventType,
      timestamp: Date.now()
    });
  } else {
    console.warn('Native navigator not available for reportDocumentEvent');
  }
}

/**
 * @deprecated RPC calls are now handled by the C++ Navigator
 */
export const makeRpcCall = function makeRpcCallToNative(method: string, args: unknown[]) {
  console.warn('makeRpcCall from messaging is deprecated. Use navigator.makeRpcCall() instead.');
  
  // Try to use the navigator's RPC call if available
  if (navigator && (navigator as unknown as { makeRpcCall?: (method: string, args: unknown[]) => unknown }).makeRpcCall) {
    return (navigator as unknown as { makeRpcCall: (method: string, args: unknown[]) => unknown }).makeRpcCall(method, args);
  }
  
  throw new Error('RPC calls are now handled by the C++ Navigator. Use navigator.makeRpcCall() instead.');
};

/**
 * Dispose function for cleanup
 */
export function dispose() {
  // Import and call the simplified messaging dispose
  // eslint-disable-next-line @typescript-eslint/no-require-imports
  const { dispose: disposeSimplified } = require('./messaging_simplified') as { dispose: () => void };
  disposeSimplified();
}