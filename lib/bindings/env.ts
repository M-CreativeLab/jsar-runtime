let nativeContext: Transmute.TrClientContext = null;
try {
  const binding = process._linkedBinding('transmute:env');
  nativeContext = new binding.ClientContext();
} catch (err) {
  console.error('failed to initialize "transmute:env" module.', err);
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

/**
 * Create an interval timer with 500ms to send keep alive update. It will be hangup when Script is busy.
 */
setInterval(() => nativeContext.keepAlive(), 500);
