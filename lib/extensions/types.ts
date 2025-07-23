/**
 * Extension system types inspired by Chrome Extension architecture
 */

export interface ExtensionManifest {
  /** Extension identifier */
  name: string;
  
  /** Extension version */
  version: string;
  
  /** Extension description */
  description?: string;
  
  /** Background script to execute on load */
  background?: {
    scripts?: string[];
    persistent?: boolean;
  };
  
  /** Extension permissions (for future use) */
  permissions?: string[];
  
  /** Manifest version (for compatibility) */
  manifest_version: number;
}

export interface ExtensionContext {
  /** Extension identifier */
  id: string;
  
  /** Extension manifest */
  manifest: ExtensionManifest;
  
  /** Extension base path */
  basePath: string;
  
  /** Extension runtime state */
  state: ExtensionState;
}

export enum ExtensionState {
  LOADING = 'loading',
  LOADED = 'loaded',
  RUNNING = 'running',
  DISABLED = 'disabled',
  ERROR = 'error',
  UNLOADED = 'unloaded'
}

export interface ExtensionLoadOptions {
  /** Whether to enable the extension immediately after loading */
  enabled?: boolean;
  
  /** Whether to persist the extension across runtime restarts */
  persistent?: boolean;
}

export interface ExtensionEvent {
  type: string;
  extensionId: string;
  data?: any;
}

export interface ExtensionAPI {
  /** Extension identifier */
  readonly id: string;
  
  /** Extension manifest */
  readonly manifest: ExtensionManifest;
  
  /** Extension state */
  readonly state: ExtensionState;
  
  /** Emit an event from the extension */
  emit(eventType: string, data?: any): void;
  
  /** Listen for events */
  on(eventType: string, callback: (data?: any) => void): void;
  
  /** Remove event listener */
  off(eventType: string, callback: (data?: any) => void): void;
}