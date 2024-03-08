import XRSystem from './api/XRSystem';
import XRSession from './api/XRSession';
import XRWebGLLayer from './api/XRWebGLLayer';
import XRFrame from './api/XRFrame';
import { type XRDevice, XRNativeDevice } from './devices';

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
  return new XRSystem(xrDevice, sessionId);
}

export {
  XRSystem,
  XRSession,
  XRWebGLLayer,
  XRFrame,
}
