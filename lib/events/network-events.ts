/**
 * Network Online/Offline Events Implementation for JSAR
 * 
 * This module provides support for detecting network connectivity changes
 * and dispatching 'online' and 'offline' events on the window object,
 * compatible with the Web API standards.
 */

export class NetworkStatusMonitor extends EventTarget {
  private _isOnline: boolean = true;
  private _monitoringInterval: NodeJS.Timeout | null = null;
  private readonly POLLING_INTERVAL = 5000; // 5 seconds

  constructor() {
    super();
    this.initializeNetworkStatus();
  }

  /**
   * Get the current network online status
   */
  get isOnline(): boolean {
    return this._isOnline;
  }

  /**
   * Initialize network status monitoring
   */
  private async initializeNetworkStatus(): Promise<void> {
    // Set initial status
    this._isOnline = await this.checkNetworkStatus();
    
    // Start monitoring for changes
    this.startMonitoring();
  }

  /**
   * Check the current network status
   * Uses a simple connectivity check approach
   */
  private async checkNetworkStatus(): Promise<boolean> {
    try {
      // Simple heuristic: if we're in a Node.js environment with access to network
      // we'll assume online by default. In real implementation, this might use
      // native bindings to check actual network connectivity
      
      // For now, we'll use a simple approach that should work in most cases
      // Check if we have access to standard networking APIs
      if (typeof globalThis.fetch === 'function') {
        // Try a simple data URL fetch which should work if networking is available
        try {
          await globalThis.fetch('data:text/plain;base64,', { 
            method: 'HEAD',
            signal: AbortSignal.timeout(1000)
          });
          return true;
        } catch {
          // If even data URL fails, likely offline or severely restricted
          return false;
        }
      }
      
      // Fallback: assume online
      return true;
    } catch {
      // If any error occurs, default to online (conservative approach)
      return true;
    }
  }

  /**
   * Start monitoring network status changes
   */
  private startMonitoring(): void {
    this._monitoringInterval = setInterval(async () => {
      const currentStatus = await this.checkNetworkStatus();
      
      if (currentStatus !== this._isOnline) {
        this._isOnline = currentStatus;
        this.dispatchNetworkEvent(currentStatus);
      }
    }, this.POLLING_INTERVAL);
  }

  /**
   * Stop monitoring network status changes
   */
  public stopMonitoring(): void {
    if (this._monitoringInterval) {
      clearInterval(this._monitoringInterval);
      this._monitoringInterval = null;
    }
  }

  /**
   * Dispatch network status change event
   */
  private dispatchNetworkEvent(isOnline: boolean): void {
    const eventType = isOnline ? 'online' : 'offline';
    const event = new Event(eventType);
    
    // Dispatch on this EventTarget (for internal listeners)
    this.dispatchEvent(event);
    
    // Also dispatch on window if it's available
    if (typeof globalThis.window !== 'undefined' && globalThis.window.dispatchEvent) {
      globalThis.window.dispatchEvent(event);
    }
  }

  /**
   * Manually update network status (for testing or external updates)
   */
  public updateNetworkStatus(isOnline: boolean): void {
    if (isOnline !== this._isOnline) {
      this._isOnline = isOnline;
      this.dispatchNetworkEvent(isOnline);
    }
  }
}

// Global network monitor instance
let networkMonitor: NetworkStatusMonitor | null = null;

/**
 * Get the global network monitor instance
 */
export function getNetworkMonitor(): NetworkStatusMonitor {
  if (!networkMonitor) {
    networkMonitor = new NetworkStatusMonitor();
  }
  return networkMonitor;
}

/**
 * Initialize network monitoring for the window object
 */
export function initializeNetworkMonitoring(): void {
  const monitor = getNetworkMonitor();
  
  // Update navigator.onLine when network status changes
  monitor.addEventListener('online', () => {
    if (typeof globalThis.navigator !== 'undefined') {
      (globalThis.navigator as any).onLine = true;
    }
  });
  
  monitor.addEventListener('offline', () => {
    if (typeof globalThis.navigator !== 'undefined') {
      (globalThis.navigator as any).onLine = false;
    }
  });
}

/**
 * Cleanup network monitoring
 */
export function cleanupNetworkMonitoring(): void {
  if (networkMonitor) {
    networkMonitor.stopMonitoring();
    networkMonitor = null;
  }
}