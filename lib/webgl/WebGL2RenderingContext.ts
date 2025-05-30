import { wrapWebGLRenderingContext } from './WebGLRenderingContext';

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export function wrapWebGL2RenderingContext(impl: any): WebGL2RenderingContext {
  return Object.assign(
    wrapWebGLRenderingContext(impl) as WebGL2RenderingContext,  // wrap for WebGL1 first
    {
      // Nothing to add here
    });
}
