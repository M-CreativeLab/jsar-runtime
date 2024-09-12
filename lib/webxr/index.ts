import XRSystemImpl from './api/XRSystem';
import { XRDevice } from './device';

const {
  XRRigidTransform: XRRigidTransformImpl,
  XRWebGLLayer: XRWebGLLayerImpl,
} = process._linkedBinding('transmute:webxr');

let xrDevice: XRDevice = null;

/**
 * Initialize the XRDevice.
 */
export async function initDevice() {
  if (xrDevice != null) {
    throw new TypeError('Only 1 XRDevice in a process.');
  }
  xrDevice = new XRDevice();
}

/**
 * Create a new `XRSystem` object.
 * 
 * @returns created instance.
 */
export function createXRSystem(): XRSystem {
  if (xrDevice == null) {
    initDevice();
  }
  return new XRSystemImpl(xrDevice);
}

export {
  XRSystemImpl,
  XRRigidTransformImpl,
  XRWebGLLayerImpl,
}
