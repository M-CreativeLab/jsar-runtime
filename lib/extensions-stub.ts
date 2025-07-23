/**
 * Placeholder extension types for the C++ extension system
 * This will be replaced with proper C++ bindings
 */

export interface ExtensionLoadOptions {
  enabled?: boolean;
  persistent?: boolean;
}

export interface ExtensionEvent {
  type: string;
  extensionId: string;
  data?: any;
}

/**
 * Placeholder ExtensionManager that stubs the interface
 * until proper C++ bindings are implemented
 */
export class ExtensionManager {
  constructor() {
    console.warn('[ExtensionManager] Using placeholder implementation - C++ extension system not yet bound');
  }

  async loadExtension(extensionPath: string, options?: ExtensionLoadOptions): Promise<void> {
    console.warn(`[ExtensionManager] C++ extension system: Would load extension from ${extensionPath}`);
    // TODO: Call C++ extension system via bindings
  }

  async loadExtensionsFromDirectory(extensionsDir: string): Promise<void> {
    console.warn(`[ExtensionManager] C++ extension system: Would load extensions from directory ${extensionsDir}`);
    // TODO: Call C++ extension system via bindings
    return Promise.resolve();
  }

  async unloadAllExtensions(): Promise<void> {
    console.warn('[ExtensionManager] C++ extension system: Would unload all extensions');
    // TODO: Call C++ extension system via bindings
  }

  onExtensionEvent(eventType: string, callback: (event: ExtensionEvent) => void): void {
    console.warn(`[ExtensionManager] C++ extension system: Would listen for ${eventType} events`);
    // TODO: Wire up C++ extension events via bindings
  }
}