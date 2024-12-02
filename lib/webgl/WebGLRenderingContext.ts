import { getExtension } from './extensions';

export function wrapWebGLRenderingContext(impl: any): WebGLRenderingContext {
  return Object.assign(impl, {
    getExtension(this: WebGLRenderingContext, extensionName: string): any {
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
