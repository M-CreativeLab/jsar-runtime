let nativeEnv = null;

try {
  const binding = (process as any)._linkedBinding('transmute:env');
  nativeEnv = new binding.Environment();
} catch (err) {
  console.warn('failed to load linked env module.');
}

class Env {
  #readyContextStr: string;
  get readyContextStr() {
    return this.#readyContextStr;
  }
  constructor(contextStr: string) {
    this.#readyContextStr = contextStr;
  }
}

let envInstance: Env;

if (nativeEnv?.getReadyContext) {
  const contextStr = nativeEnv.getReadyContext() || '{}';
  envInstance = new Env(contextStr);
} else {
  envInstance = new Env('{}');
}

export function getReadyContext(): string {
  return envInstance.readyContextStr;
}

export function createEnv(contextStr: string) {
  envInstance = new Env(contextStr);
}

export function markRuntimeAvailable() {
  if (nativeEnv?.markRuntimeAvailable) {
    nativeEnv?.markRuntimeAvailable();
  }
}

export function markGomBufferAsErrored(channelId: string, errorCode: number) {
  if (nativeEnv?.markGomBufferAsErrored) {
    nativeEnv?.markGomBufferAsErrored(channelId, errorCode);
  }
}
