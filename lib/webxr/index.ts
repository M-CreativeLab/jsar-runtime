const webxrNative = process._linkedBinding('transmute:webxr');
const {
  XRRigidTransform: XRRigidTransformImpl,
} = webxrNative;

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

export {
  XRRigidTransformImpl,
}
