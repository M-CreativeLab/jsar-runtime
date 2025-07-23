import { promises as fs } from 'node:fs';
import { join } from 'node:path';
import { Extension } from './Extension';
import { 
  ExtensionManifest, 
  ExtensionState, 
  ExtensionLoadOptions,
  ExtensionEvent 
} from './types';

/**
 * ExtensionManager manages the lifecycle of extensions
 * Inspired by Chrome Extension management
 */
export class ExtensionManager extends EventTarget {
  private _extensions = new Map<string, Extension>();
  private _extensionPaths = new Map<string, string>();

  constructor() {
    super();
  }

  /**
   * Get all loaded extensions
   */
  getExtensions(): Extension[] {
    return Array.from(this._extensions.values());
  }

  /**
   * Get extension by ID
   */
  getExtension(extensionId: string): Extension | undefined {
    return this._extensions.get(extensionId);
  }

  /**
   * Check if extension is loaded
   */
  hasExtension(extensionId: string): boolean {
    return this._extensions.has(extensionId);
  }

  /**
   * Load extension from directory
   */
  async loadExtension(extensionPath: string, options: ExtensionLoadOptions = {}): Promise<Extension> {
    try {
      console.info(`[ExtensionManager] Loading extension from: ${extensionPath}`);
      
      const extension = await Extension.loadFromDirectory(extensionPath);
      const extensionId = extension.id;
      
      // Check if extension is already loaded
      if (this._extensions.has(extensionId)) {
        throw new Error(`Extension with ID '${extensionId}' is already loaded`);
      }
      
      // Store the extension and its path
      this._extensions.set(extensionId, extension);
      this._extensionPaths.set(extensionId, extensionPath);
      
      // Set up event forwarding from extension to manager
      this._setupExtensionEventForwarding(extension);
      
      // Load the extension
      await extension.load();
      
      // Enable immediately if requested (default: true)
      if (options.enabled !== false) {
        await extension.enable();
      }
      
      // Emit extension loaded event
      this._emitExtensionEvent('extensionLoaded', extensionId, {
        extension: extension.context,
        options
      });
      
      console.info(`[ExtensionManager] Extension '${extensionId}' loaded successfully`);
      return extension;
      
    } catch (error) {
      console.error(`[ExtensionManager] Failed to load extension from ${extensionPath}:`, error);
      throw error;
    }
  }

  /**
   * Load multiple extensions from a directory containing extension folders
   */
  async loadExtensionsFromDirectory(extensionsDir: string): Promise<Extension[]> {
    try {
      const entries = await fs.readdir(extensionsDir, { withFileTypes: true });
      const loadedExtensions: Extension[] = [];
      
      for (const entry of entries) {
        if (entry.isDirectory()) {
          const extensionPath = join(extensionsDir, entry.name);
          const manifestPath = join(extensionPath, 'manifest.json');
          
          try {
            // Check if manifest.json exists
            await fs.access(manifestPath);
            const extension = await this.loadExtension(extensionPath);
            loadedExtensions.push(extension);
          } catch (error) {
            console.warn(`[ExtensionManager] Skipping directory '${entry.name}': ${error.message}`);
          }
        }
      }
      
      console.info(`[ExtensionManager] Loaded ${loadedExtensions.length} extensions from ${extensionsDir}`);
      return loadedExtensions;
      
    } catch (error) {
      console.error(`[ExtensionManager] Failed to load extensions from directory ${extensionsDir}:`, error);
      throw error;
    }
  }

  /**
   * Enable an extension
   */
  async enableExtension(extensionId: string): Promise<void> {
    const extension = this._extensions.get(extensionId);
    if (!extension) {
      throw new Error(`Extension '${extensionId}' not found`);
    }
    
    await extension.enable();
    
    this._emitExtensionEvent('extensionEnabled', extensionId, {
      extension: extension.context
    });
  }

  /**
   * Disable an extension
   */
  async disableExtension(extensionId: string): Promise<void> {
    const extension = this._extensions.get(extensionId);
    if (!extension) {
      throw new Error(`Extension '${extensionId}' not found`);
    }
    
    await extension.disable();
    
    this._emitExtensionEvent('extensionDisabled', extensionId, {
      extension: extension.context
    });
  }

  /**
   * Unload an extension
   */
  async unloadExtension(extensionId: string): Promise<void> {
    const extension = this._extensions.get(extensionId);
    if (!extension) {
      throw new Error(`Extension '${extensionId}' not found`);
    }
    
    try {
      await extension.unload();
      
      // Remove from maps
      this._extensions.delete(extensionId);
      this._extensionPaths.delete(extensionId);
      
      this._emitExtensionEvent('extensionUnloaded', extensionId, {
        extension: extension.context
      });
      
      console.info(`[ExtensionManager] Extension '${extensionId}' unloaded successfully`);
      
    } catch (error) {
      console.error(`[ExtensionManager] Failed to unload extension '${extensionId}':`, error);
      throw error;
    }
  }

  /**
   * Unload all extensions
   */
  async unloadAllExtensions(): Promise<void> {
    const extensionIds = Array.from(this._extensions.keys());
    
    for (const extensionId of extensionIds) {
      try {
        await this.unloadExtension(extensionId);
      } catch (error) {
        console.error(`[ExtensionManager] Failed to unload extension '${extensionId}':`, error);
      }
    }
    
    console.info(`[ExtensionManager] All extensions unloaded`);
  }

  /**
   * Get extension states summary
   */
  getExtensionStates(): Record<string, ExtensionState> {
    const states: Record<string, ExtensionState> = {};
    
    for (const [id, extension] of this._extensions) {
      states[id] = extension.state;
    }
    
    return states;
  }

  /**
   * Get extensions by state
   */
  getExtensionsByState(state: ExtensionState): Extension[] {
    return Array.from(this._extensions.values()).filter(
      extension => extension.state === state
    );
  }

  /**
   * Set up event forwarding from extension to manager
   */
  private _setupExtensionEventForwarding(extension: Extension): void {
    const eventTypes = ['loaded', 'enabled', 'disabled', 'unloaded', 'error', 'stateChanged'];
    
    for (const eventType of eventTypes) {
      extension.addEventListener(eventType, (event: any) => {
        this._emitExtensionEvent(`extension${eventType.charAt(0).toUpperCase() + eventType.slice(1)}`, 
          extension.id, event.detail);
      });
    }
  }

  /**
   * Emit an extension-related event
   */
  private _emitExtensionEvent(eventType: string, extensionId: string, data?: any): void {
    const event = new CustomEvent(eventType, { 
      detail: { extensionId, ...data } 
    }) as any;
    
    event.extensionId = extensionId;
    this.dispatchEvent(event);
  }

  /**
   * Add event listener for extension events
   */
  onExtensionEvent(eventType: string, callback: (event: ExtensionEvent) => void): void {
    this.addEventListener(eventType, (event: any) => {
      callback({
        type: eventType,
        extensionId: event.extensionId,
        data: event.detail
      });
    });
  }
}