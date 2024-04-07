import * as logger from '../bindings/logger';
import WebGLRenderingContextImpl from './WebGLRenderingContext';
import WebGL2RenderingContextImpl from './WebGL2RenderingContext';

type RenderingContext = WebGLRenderingContext | WebGL2RenderingContext;

export function getContext(type: 'webgl', options?: WebGLContextAttributes): WebGLRenderingContext | null;
export function getContext(type: 'webgl2', options?: WebGLContextAttributes): WebGL2RenderingContext | null;
export function getContext(type: 'webgl' | 'webgl2', options?: WebGLContextAttributes): RenderingContext | null;
export function getContext(type: 'webgl' | 'webgl2', options?: WebGLContextAttributes): RenderingContext | null {
  logger.info(`[webgl] created a new ${type} context.`);
  return type === 'webgl2' ?
    new WebGL2RenderingContextImpl(null, options) :
    new WebGLRenderingContextImpl(null, options);
}
