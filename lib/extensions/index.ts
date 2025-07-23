/**
 * JSAR Extension System
 * 
 * Provides Chrome Extension-like functionality for JSAR runtime,
 * allowing extensions to be loaded, unloaded, and execute scripts.
 * 
 * @example
 * ```typescript
 * import { ExtensionManager } from './extensions';
 * 
 * const extensionManager = new ExtensionManager();
 * await extensionManager.loadExtension('/path/to/extension');
 * ```
 */

export { Extension } from './Extension';
export { ExtensionManager } from './ExtensionManager';
export * from './types';