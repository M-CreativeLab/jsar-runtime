import { wrapWebGLRenderingContext } from './WebGLRenderingContext';
import { wrapWebGL2RenderingContext } from './WebGL2RenderingContext';

type RenderingContext = WebGLRenderingContext | WebGL2RenderingContext;

const {
  WebGLRenderingContext: WebGLRenderingContextImpl,
  WebGL2RenderingContext: WebGL2RenderingContextImpl,
} = process._linkedBinding('transmute:webgl');

export function getContext(type: 'webgl', options?: WebGLContextAttributes): WebGLRenderingContext | null;
export function getContext(type: 'webgl2', options?: WebGLContextAttributes): WebGL2RenderingContext | null;
export function getContext(type: 'webgl' | 'webgl2', options?: WebGLContextAttributes): RenderingContext | null;
export function getContext(_type: 'webgl' | 'webgl2', _options?: WebGLContextAttributes): RenderingContext | null {
  // TODO
  throw new Error('Not implemented');
}

/**
 * Get the WebGL context of the host environment.
 * 
 * @param isWebGL2 If true, return a WebGL2RenderingContext, otherwise return a WebGLRenderingContext.
 * @param nativeGl The native WebGL context.
 * @returns The wrapped WebGL context.
 */
export function getHostWebGLContext(isWebGL2: boolean, nativeGl: RenderingContext): RenderingContext {
  const wrapped = isWebGL2 ?
    wrapWebGL2RenderingContext(nativeGl as WebGL2RenderingContext) :
    wrapWebGLRenderingContext(nativeGl as WebGLRenderingContext);
  return wrapped;
}

export {
  WebGLRenderingContextImpl,
  WebGL2RenderingContextImpl,
}
