import XRSystemImpl from './api/XRSystem';
import XRSessionImpl from './api/XRSession';
// import XRRigidTransformImpl from './api/XRRigidTransform';
// import XRWebGLLayerImpl from './api/XRWebGLLayer';
import XRFrameImpl from './api/XRFrame';
import XRViewImpl from './api/XRView';
import { type XRDevice, XRNativeDevice } from './devices';

const {
  XRRigidTransform: XRRigidTransformImpl,
  XRWebGLLayer: XRWebGLLayerImpl,
} = process._linkedBinding('transmute:webxr');

let xrDevice: XRDevice = null;
export async function prepareXRSystem(): Promise<void> {
  if (xrDevice == null) {
    xrDevice = new XRNativeDevice();
  }
  await xrDevice.waitForReady();
}

export function createBondXRSystem(sessionId: number): XRSystem {
  if (xrDevice == null) {
    throw new Error('Failed to create XR system, please call prepareXRSystem() first.');
  }
  return new XRSystemImpl(xrDevice, sessionId);
}

export {
  XRSystemImpl,
  XRSessionImpl,
  XRRigidTransformImpl,
  XRWebGLLayerImpl,
  XRFrameImpl,
  XRViewImpl,
}
