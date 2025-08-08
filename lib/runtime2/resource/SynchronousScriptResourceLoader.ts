import { ResourceLoaderOnTransmute } from '../jsardom/ResourceLoader';

/**
 * Extended ResourceLoader that handles script files synchronously to fix
 * the issue where external scripts load asynchronously causing subsequent
 * inline scripts to fail when accessing functions from external scripts.
 */
export class SynchronousScriptResourceLoader extends ResourceLoaderOnTransmute {
  private pendingScripts: Map<string, Promise<any>> = new Map();
  private scriptExecutionQueue: Promise<void> = Promise.resolve();

  async fetch<AsType extends keyof any>(
    url: string,
    options: any,
    returnsAs?: AsType
  ): Promise<any> {
    // Check if this is a JavaScript file
    const isScript = this.isScriptUrl(url);
    
    if (isScript) {
      // For script files, ensure they execute in order
      return this.fetchScriptSynchronously(url, options, returnsAs);
    }

    // For non-script files, use the original implementation
    return super.fetch(url, options, returnsAs);
  }

  private isScriptUrl(url: string): boolean {
    try {
      const urlObj = new URL(url, 'file://');
      const pathname = urlObj.pathname.toLowerCase();
      return pathname.endsWith('.js') || pathname.endsWith('.mjs') || pathname.endsWith('.ts');
    } catch {
      // If URL parsing fails, assume it's not a script
      return false;
    }
  }

  private async fetchScriptSynchronously<AsType extends keyof any>(
    url: string,
    options: any,
    returnsAs?: AsType
  ): Promise<any> {
    // Check if we already have a pending request for this script
    if (this.pendingScripts.has(url)) {
      return this.pendingScripts.get(url);
    }

    // Create a promise for this script
    const scriptPromise = this.scriptExecutionQueue.then(async () => {
      console.info(`Loading script synchronously: ${url}`);
      
      // Fetch the script content
      const scriptContent = await super.fetch(url, options, returnsAs || 'string' as AsType);
      
      // If this is a string request (script content), execute it immediately
      if (!returnsAs || returnsAs === 'string') {
        try {
          // Execute the script in the global context
          eval(scriptContent as string);
          console.info(`Script executed synchronously: ${url}`);
        } catch (error) {
          console.error(`Error executing script ${url}:`, error);
          throw error;
        }
      }
      
      return scriptContent;
    });

    // Store the promise to avoid duplicate requests
    this.pendingScripts.set(url, scriptPromise);

    // Chain this script to the execution queue
    this.scriptExecutionQueue = scriptPromise.then(() => {}).catch(() => {});

    return scriptPromise;
  }
}