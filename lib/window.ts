import { getNetworkMonitor } from './events/network-events';

/**
 * Custom Window implementation that extends EventTarget
 * to support network online/offline events
 */
class WindowImpl extends EventTarget implements Window {
  // Online/offline event handlers
  private _ononline: ((this: Window, ev: Event) => any) | null = null;
  private _onoffline: ((this: Window, ev: Event) => any) | null = null;

  constructor() {
    super();
    
    // Set up network event forwarding
    const networkMonitor = getNetworkMonitor();
    
    // Forward online/offline events from network monitor to this window
    networkMonitor.addEventListener('online', (event) => {
      this.dispatchEvent(new Event('online'));
    });
    
    networkMonitor.addEventListener('offline', (event) => {
      this.dispatchEvent(new Event('offline'));
    });
  }

  // Implement ononline property
  get ononline(): ((this: Window, ev: Event) => any) | null {
    return this._ononline;
  }

  set ononline(handler: ((this: Window, ev: Event) => any) | null) {
    // Remove old listener if exists
    if (this._ononline) {
      this.removeEventListener('online', this._ononline);
    }
    
    this._ononline = handler;
    
    // Add new listener if handler is provided
    if (handler) {
      this.addEventListener('online', handler);
    }
  }

  // Implement onoffline property
  get onoffline(): ((this: Window, ev: Event) => any) | null {
    return this._onoffline;
  }

  set onoffline(handler: ((this: Window, ev: Event) => any) | null) {
    // Remove old listener if exists
    if (this._onoffline) {
      this.removeEventListener('offline', this._onoffline);
    }
    
    this._onoffline = handler;
    
    // Add new listener if handler is provided
    if (handler) {
      this.addEventListener('offline', handler);
    }
  }

  // We need to satisfy the Window interface, but we'll delegate most properties to globalThis
  [key: string]: any;
}

export function createWindow() {
  const windowImpl = new WindowImpl();
  
  // Merge with globalThis to get all other window properties
  return Object.assign(windowImpl, globalThis) as Window;
}
