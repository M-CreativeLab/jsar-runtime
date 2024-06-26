
import { getClientContext } from '@transmute/env';

const {
  XRDeviceNative,
} = process._linkedBinding('transmute:webxr');

export enum StereoRenderingMode {
  MultiPass = 'multipass',
  SinglePass = 'singlepass',
  SinglePassInstanced = 'singlepassinstanced',
  SinglePassMultiview = 'singlepassmultiview',
  Unknown = 'unknown',
}

export class XRDevice {
  enabled: boolean;
  stereoRenderingMode: StereoRenderingMode = StereoRenderingMode.Unknown;
  // Value is used for `XRSession.prototype.environmentBlendMode`
  // and should be one of XREnvironmentBlendMode types: 'opaque', 'additive',
  // or 'alpha-blend'.
  environmentBlendMode: 'opaque' | 'additive' | 'alpha-blend' = 'additive';
  #handle: Transmute.XRDeviceNative;

  constructor() {
    this.#handle = new XRDeviceNative();
    const clientCtx = getClientContext();
    if (clientCtx.xrDevice?.enabled) {
      this.enabled = true;
      this.stereoRenderingMode = <StereoRenderingMode>clientCtx.xrDevice.stereoRenderingMode;
    } else {
      this.enabled = false;
    }
    console.info(`[XRDevice] enabled: ${this.enabled ? 'YES' : 'NO'}`);
    console.info(`[XRDevice] active: ${clientCtx.xrDevice?.active ? 'YES' : 'NO'}`);
    console.info(`[XRDevice] Stereo Rendering Mode: ${this.stereoRenderingMode}`);
  }

  get handle(): Transmute.XRDeviceNative {
    return this.#handle;
  }

  /**
   * returns if the session mode is supported.
   */
  isSessionSupported(mode: XRSessionMode): Promise<boolean> {
    return this.#handle.isSessionSupported(mode);
  }

  /**
   * @param {string} featureDescriptor
   * @return {boolean}
   */
  isFeatureSupported(_featureDescriptor: string): boolean {
    return true;
  }

  /**
   * Returns a promise if creating a session is successful.
   * Usually used to set up presentation in the device.
   *
   * @param {XRSessionMode} mode
   * @param {Set<string>} enabledFeatures
   * @return {Promise<number>}
   */
  async requestSession(mode: XRSessionMode, _enabledFeatures: Set<string>): Promise<number> {
    const sessionId = await this.#handle.requestSession(mode);
    if (sessionId <= 0) {
      throw new Error('Failed to request session');
    }
    return sessionId;
  }

  /**
   * Returns if the device is rendering in multi-pass mode.
   * 
   * In multi-pass, there are 2 framebuffers for left and right eyes, it means the pose.views only have 1 element, but
   * the frame callback will be called twice, once for each eye.
   * 
   * Otherwise, the device is rendering in single-pass mode, and the pose.views will have 2 elements, one for each eye,
   * and the frame callback will be called once.
   */
  isRenderingInMultiPass(): boolean {
    if (this.stereoRenderingMode === StereoRenderingMode.Unknown) {
      throw new Error('XRDevice.stereoRenderingMode is unknown or not set. Please call XRDevice.waitForReady() first.');
    }
    return this.stereoRenderingMode === StereoRenderingMode.MultiPass;
  }
}
