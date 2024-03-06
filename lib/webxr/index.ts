import XRSystem from './api/XRSystem';
import XRSession from './api/XRSession';
import XRFrame from './api/XRFrame';
import { type XRDevice, XRNativeDevice } from './devices';

let xrDevice: XRDevice = null;
let xrSystem: XRSystem = null;
export async function createXRSystem(): Promise<XRSystem> {
  if (xrDevice == null) {
    xrDevice = new XRNativeDevice();
  }
  await xrDevice.waitForReady();
  xrSystem = new XRSystem(xrDevice);
  return xrSystem;
}

export function getXRSystem(): XRSystem {
  if (xrSystem == null) {
    throw new Error('Failed to get XR system, please call createXRSystem() first.');
  }
  return xrSystem;
}

export {
  XRSystem,
  XRSession,
  XRFrame,
}
