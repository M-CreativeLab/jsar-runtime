import XRDevice from './XRDevice';
import * as logger from '../../bindings/logger';
import { makeRpcCall } from '../../bindings/messaging';
const { XRDeviceNative } = process._linkedBinding('transmute:webxr');

export default class XRNativeDevice extends XRDevice {
  #handle: Transmute.XRDeviceNative;
  #enabled: boolean;

  constructor() {
    super();
    this.environmentBlendMode = 'additive';
    this.#handle = new XRDeviceNative();
  }

  async waitForReady(): Promise<void> {
    const response = await makeRpcCall('xr.initializeDevice', []);
    this.#enabled = response.enabled;
  }

  isSessionSupported(mode: XRSessionMode): boolean {
    return mode === 'immersive-ar';
  }

  isFeatureSupported(_featureDescriptor: string): boolean {
    return true;
  }

  requestSession(mode: XRSessionMode, enabledFeatures: Set<string>): Promise<number> {
    return Promise.resolve(0);
  }
}
