import WebGLRenderingContextImpl from './WebGLRenderingContext';

export function createWebGLContext(
  _width: number,
  _height: number,
  options?: WebGLContextAttributes
): WebGLRenderingContext | null {
  return new WebGLRenderingContextImpl(null, options);
}
