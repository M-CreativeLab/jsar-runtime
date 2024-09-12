import XRSystemImpl from './api/XRSystem';
import { XRDevice } from './device';

const {
  XRRigidTransform: XRRigidTransformImpl,
  XRWebGLLayer: XRWebGLLayerImpl,
} = process._linkedBinding('transmute:webxr');

let xrDevice: XRDevice = null;
export async function prepareXRSystem(): Promise<void> {
  if (xrDevice != null) {
    throw new TypeError('Only 1 XRDevice in a process.');
  }
  xrDevice = new XRDevice();
}

export function createBondXRSystem(): XRSystem {
  if (xrDevice == null) {
    throw new Error('Failed to create XR system, please call prepareXRSystem() first.');
  }
  return new XRSystemImpl(xrDevice);
}

export function createXRSystem(): XRSystem {
  if (xrDevice == null) {
    throw new Error('Failed to create XR system, please call prepareXRSystem() first.');
  }
  return createBondXRSystem();
}

export {
  XRSystemImpl,
  XRRigidTransformImpl,
  XRWebGLLayerImpl,
}
