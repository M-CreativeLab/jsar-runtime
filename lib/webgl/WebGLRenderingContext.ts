import { getExtension } from './extensions';

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export function wrapWebGLRenderingContext(impl: any): WebGLRenderingContext {
  return Object.assign(impl, {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    getExtension(this: WebGLRenderingContext, extensionName: string): any {
      // eslint-disable-next-line @typescript-eslint/no-explicit-any
      let ext: any = null;
      const supportedExtensions = this.getSupportedExtensions();
      if (supportedExtensions.includes(extensionName)) {
        ext = getExtension(this, extensionName);
      }
      if (ext == null) {
        console.warn(`The extension(${extensionName}) not found.`);
      }
      return ext;
    },
  });
}
