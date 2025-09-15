const webxrNative = process._linkedBinding('transmute:webxr');
let globalXRSystem = null;

/**
 * @returns The `XRSystem` object.
 */
export function getXRSystem() {
  if (globalXRSystem == null) {
    globalXRSystem = webxrNative.getXRSystem();
  }
  return globalXRSystem;
}
