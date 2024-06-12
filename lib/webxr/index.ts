import XRSystemImpl from './api/XRSystem';
import { XRDevice } from './device';

const {
  XRRigidTransform: XRRigidTransformImpl,
  XRWebGLLayer: XRWebGLLayerImpl,
} = process._linkedBinding('transmute:webxr');

let xrDevice: XRDevice = new XRDevice();
export async function prepareXRSystem(): Promise<boolean> {
  return await xrDevice.waitForReady();
}

export function createBondXRSystem(): XRSystem {
  if (xrDevice == null) {
    throw new Error('Failed to create XR system, please call prepareXRSystem() first.');
  }
  return new XRSystemImpl(xrDevice);
}

export {
  XRSystemImpl,
  XRRigidTransformImpl,
  XRWebGLLayerImpl,
}
