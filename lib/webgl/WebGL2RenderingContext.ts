import { wrapWebGLRenderingContext } from './WebGLRenderingContext';

export function wrapWebGL2RenderingContext(impl: any): WebGL2RenderingContext {
  return Object.assign(
    wrapWebGLRenderingContext(impl) as WebGL2RenderingContext,  // wrap for WebGL1 first
    {
      // Nothing to add here
    });
}
