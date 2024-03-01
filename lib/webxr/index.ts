import XRSystem from './api/XRSystem';
import XRSession from './api/XRSession';
import XRFrame from './api/XRFrame';

let xrSystem: XRSystem = null;
export function createXRSystem(): XRSystem {
  xrSystem = new XRSystem(null);
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
