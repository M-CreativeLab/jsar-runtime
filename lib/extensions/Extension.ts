import { promises as fs } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { 
  ExtensionManifest, 
  ExtensionContext, 
  ExtensionState, 
  ExtensionAPI,
  ExtensionEvent 
} from './types';

/**
 * Extension class representing an individual extension
 * Inspired by Chrome Extension architecture
 */
export class Extension extends EventTarget implements ExtensionAPI {
  private _context: ExtensionContext;
  private _scriptContexts: Array<{ script: string; context: any }> = [];

  constructor(id: string, manifest: ExtensionManifest, basePath: string) {
    super();
    
    this._context = {
      id,
      manifest,
      basePath: resolve(basePath),
      state: ExtensionState.LOADING
    };
  }

  get id(): string {
    return this._context.id;
  }

  get manifest(): ExtensionManifest {
    return this._context.manifest;
  }

  get state(): ExtensionState {
    return this._context.state;
  }

  get context(): ExtensionContext {
    return { ...this._context };
  }

  /**
   * Load the extension and execute background scripts
   */
  async load(): Promise<void> {
    try {
      this._setState(ExtensionState.LOADING);
      
      // Execute background scripts if defined
      if (this.manifest.background?.scripts) {
        for (const scriptPath of this.manifest.background.scripts) {
          await this._executeBackgroundScript(scriptPath);
        }
      }
      
      this._setState(ExtensionState.LOADED);
      this.emit('loaded');
      
    } catch (error) {
      this._setState(ExtensionState.ERROR);
      this.emit('error', error);
      throw error;
    }
  }

  /**
   * Enable the extension (start execution)
   */
  async enable(): Promise<void> {
    if (this.state !== ExtensionState.LOADED && this.state !== ExtensionState.DISABLED) {
      throw new Error(`Cannot enable extension in state: ${this.state}`);
    }
    
    try {
      this._setState(ExtensionState.RUNNING);
      this.emit('enabled');
    } catch (error) {
      this._setState(ExtensionState.ERROR);
      this.emit('error', error);
      throw error;
    }
  }

  /**
   * Disable the extension
   */
  async disable(): Promise<void> {
    if (this.state !== ExtensionState.RUNNING) {
      throw new Error(`Cannot disable extension in state: ${this.state}`);
    }
    
    try {
      this._setState(ExtensionState.DISABLED);
      this.emit('disabled');
    } catch (error) {
      this._setState(ExtensionState.ERROR);
      this.emit('error', error);
      throw error;
    }
  }

  /**
   * Unload the extension completely
   */
  async unload(): Promise<void> {
    try {
      // Clean up script contexts
      this._scriptContexts.length = 0;
      
      this._setState(ExtensionState.UNLOADED);
      this.emit('unloaded');
      
    } catch (error) {
      this._setState(ExtensionState.ERROR);
      this.emit('error', error);
      throw error;
    }
  }

  /**
   * Emit an event from the extension
   */
  emit(eventType: string, data?: any): void {
    const event = new CustomEvent(eventType, { detail: data }) as any;
    event.extensionId = this.id;
    this.dispatchEvent(event);
  }

  /**
   * Listen for events
   */
  on(eventType: string, callback: (data?: any) => void): void {
    this.addEventListener(eventType, (event: any) => {
      callback(event.detail);
    });
  }

  /**
   * Remove event listener
   */
  off(eventType: string, callback: (data?: any) => void): void {
    this.removeEventListener(eventType, callback as EventListener);
  }

  /**
   * Load extension manifest from directory
   */
  static async loadFromDirectory(extensionPath: string): Promise<Extension> {
    const manifestPath = join(extensionPath, 'manifest.json');
    
    try {
      const manifestContent = await fs.readFile(manifestPath, 'utf-8');
      const manifest: ExtensionManifest = JSON.parse(manifestContent);
      
      // Validate required fields
      if (!manifest.name || !manifest.version || !manifest.manifest_version) {
        throw new Error('Invalid manifest: missing required fields (name, version, manifest_version)');
      }
      
      // Generate extension ID from path or name (simplified approach)
      const extensionId = manifest.name.toLowerCase().replace(/[^a-z0-9]/g, '-');
      
      return new Extension(extensionId, manifest, extensionPath);
      
    } catch (error) {
      throw new Error(`Failed to load extension from ${extensionPath}: ${error.message}`);
    }
  }

  /**
   * Execute a background script
   */
  private async _executeBackgroundScript(scriptPath: string): Promise<void> {
    const fullScriptPath = join(this._context.basePath, scriptPath);
    
    try {
      // Read the script content
      const scriptContent = await fs.readFile(fullScriptPath, 'utf-8');
      
      // Create a basic execution context for the script
      const extensionAPI = {
        extension: {
          id: this.id,
          manifest: this.manifest,
          state: this.state
        },
        runtime: {
          // Placeholder for future runtime APIs
          getURL: (path: string) => pathToFileURL(join(this._context.basePath, path)).href,
          onStartup: { addListener: (callback: () => void) => callback() },
          onInstalled: { addListener: (callback: () => void) => callback() }
        },
        console: console // Provide console access to extensions
      };
      
      // Simple script execution (in production, this should be more sandboxed)
      const scriptFunction = new Function('chrome', 'browser', scriptContent);
      const scriptContext = { script: scriptPath, context: extensionAPI };
      
      // Execute the script with extension API context
      scriptFunction(extensionAPI, extensionAPI);
      
      this._scriptContexts.push(scriptContext);
      
      console.info(`[Extension:${this.id}] Executed background script: ${scriptPath}`);
      
    } catch (error) {
      console.error(`[Extension:${this.id}] Failed to execute background script ${scriptPath}:`, error);
      throw error;
    }
  }

  /**
   * Set the extension state and emit state change event
   */
  private _setState(newState: ExtensionState): void {
    const oldState = this._context.state;
    this._context.state = newState;
    
    if (oldState !== newState) {
      this.emit('stateChanged', { oldState, newState });
    }
  }
}