import * as logger from './logger';

let nativeEnv: Transmute.TransmuteEnvironment = null;
let nativeContext: Transmute.TrClientContext = null;
try {
  const binding = process._linkedBinding('transmute:env');
  nativeEnv = new binding.Environment();
  nativeContext = new binding.ClientContext();
} catch (err) {
  logger.error('failed to initialize "transmute:env" module.', err);
}

/**
 * Create an interval timer with 500ms to send keep alive update. It will be hangup when Script is busy.
 */
setInterval(() => nativeContext.keepAlive(), 500);

class Env {
  runtimeInit: any;
  #runtimeInitStr: string;
  get runtimeInitStr() {
    return this.#runtimeInitStr;
  }
  constructor(argJson: string = '{}') {
    this.#runtimeInitStr = argJson;
    try {
      this.runtimeInit = JSON.parse(argJson);
    } catch (err) {
      logger.warn('failed to parse runtimeInit JSON:', argJson, 'switch to default values.');
      this.runtimeInit = {
        // default values?
      };
    }
  }
}

let envInstance: Env;
export function createEnv() {
  if (envInstance instanceof Env) {
    return;
  }
  if (typeof nativeEnv?.getRuntimeInit === 'function') {
    envInstance = new Env(nativeEnv.getRuntimeInit());
  } else {
    envInstance = new Env();
  }
}

export function getRuntimeInit(): any {
  return envInstance.runtimeInit;
}

export function markRuntimeAvailable(runtimeVersions: string) {
  if (nativeEnv?.markRuntimeAvailable) {
    nativeEnv?.markRuntimeAvailable(runtimeVersions);
  }
}

export function getClientContext(): Transmute.TrClientContext {
  return nativeContext;
}

/**
 * Check if WebXR is supported in the current environment.
 * 
 * @returns true if WebXR is supported in the current environment.
 */
export function isWebXRSupported(): boolean {
  return nativeContext.xrDevice?.enabled === true;
}

// by default, it creates env instance.
if (process.env.TRANSMUTE_AUTO_CREATE_ENV === 'yes') {
  createEnv();
}
