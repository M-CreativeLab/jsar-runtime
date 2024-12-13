const webxrNative = process._linkedBinding('transmute:webxr');
let globalXRSystem: XRSystem = null;

/**
 * @returns The `XRSystem` object.
 */
export function getXRSystem(): XRSystem {
  if (globalXRSystem == null) {
    globalXRSystem = webxrNative.getXRSystem();
  }
  return globalXRSystem;
}
