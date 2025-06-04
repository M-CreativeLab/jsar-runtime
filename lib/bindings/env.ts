import { isMainThread } from 'node:worker_threads';
import { getHostWebGLContext } from '../webgl';

let nativeContext: Transmute.TrClientContext = null;
try {
  // Preload the followings because this module "transmute:env" depends on them.
  process._linkedBinding('transmute:browser');
  process._linkedBinding('transmute:canvas');
  process._linkedBinding('transmute:dom');
  process._linkedBinding('transmute:messaging');
  process._linkedBinding('transmute:renderer');
  process._linkedBinding('transmute:webgl');
  process._linkedBinding('transmute:webxr');

  // Load the native module "transmute:env" and create the `ClientContext` instance after dependencies are loaded.
  const binding = process._linkedBinding('transmute:env');
  nativeContext = new binding.ClientContext({
    isWorker: !isMainThread,
  });
} catch (err) {
  console.error('failed to initialize "transmute:env" module.', err);
}

export function getClientContext(): Transmute.TrClientContext {
  return nativeContext;
}

export function getPerformanceNow(): number {
  return nativeContext.fastPerformanceNow();
}

/**
 * It returns the host created WebGL or WebGL2 rendering context, this rendering context is used to draw stuffs on the host scene. And
 * at the client-side, there is no right to control the context attributes, thus there is no `contextAttributes` parameter in this method.
 * 
 * @returns an instance of WebGLRenderingContext or WebGL2RenderingContext depends on the host created.
 */
export function getHostWebGLRenderingContext(): WebGLRenderingContext | WebGL2RenderingContext {
  return getHostWebGLContext(nativeContext.webglVersion === 2, nativeContext.gl);
}

/**
 * Check if WebXR is supported in the current environment.
 * 
 * @returns true if WebXR is supported in the current environment.
 */
export function isWebXRSupported(): boolean {
  return nativeContext.xrDevice?.enabled === true;
}

/**
 * The flag indicates if this application is running in debug mode.
 */
export const isDebugging = process.env.JSAR_DEBUG_ENABLED === 'yes';

/**
 * Check if this application should disable the resources caching forcibly.
 * 
 * NOTE: This only works in debug mode.
 */
export function isResourcesCachingDisabled(): boolean {
  if (!isDebugging) {
    return false;
  }
  return process.env.JSAR_RESOURCES_CACHING === 'no';
}

const defaultExpirationTime = 24 * 60 * 60 * 1000; // 24 hours by default

/**
 * Get the expiration time of the resource cache.
 * 
 * By default, the expiration time is 1 day, or from the environment variable `JSAR_RESOURCES_CACHE_EXPIRATION_TIME`, that is a number
 * in milliseconds.
 * 
 * @returns the expiration time of the resource cache in milliseconds.
 */
export function getResourceCacheExpirationTime(): number {
  const expirationTime = parseInt(process.env.JSAR_RESOURCES_CACHE_EXPIRATION_TIME);
  if (typeof expirationTime !== 'number' || isNaN(expirationTime) || expirationTime < 0) {
    return defaultExpirationTime;
  } else {
    return expirationTime;
  }
}

export function printSummary() {
  console.info('Application Environment Summary');
  console.info('- Debugging:', isDebugging ? 'Yes' : 'No');
  console.info('- Client Context', nativeContext);
  if (isDebugging) {
    console.info('- Environment Variables:', process.env);
  }
}

/**
 * Create an interval timer with 500ms to send keep alive update. It will be hangup when Script is busy.
 */
setInterval(() => nativeContext.keepAlive(), 500);

/**
 * 
 * @returns LLM API key
 */
export const getThreepioApiKey = (): string => {
  return process.env.JSAR_THREEPIO_API_KEY as string;
};

/**
 * 
 * @returns LLM API provider name, such as 'doubao', 'qwen', etc.
 */
export const getThreepioApiProvider = (): string => {
  return process.env.JSAR_THREEPIO_API_PROVIDER as string;
};

/**
 * 
 * @returns LLM API model ID, such as 'gpt-4', 'qwen-7b', etc.
 */
export const getThreepioApiModelId = (): string => {
  return process.env.JSAR_THREEPIO_API_MODELID as string;
};

/**
 * 
 * @returns LLM API endpoint, such as 'https://api.doubao.com/v1/chat/completions', etc.
 */
export const getThreepioApiEndpoint = (): string => {
  return process.env.JSAR_THREEPIO_API_ENDPOINT as string;
};
