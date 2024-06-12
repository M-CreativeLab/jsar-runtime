
import * as logger from '@transmute/logger';
import { getClientContext } from '@transmute/env';
import * as renderer from '@transmute/renderer';

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
  }

  get handle(): Transmute.XRDeviceNative {
    return this.#handle;
  }

  /**
   * When a `XRDevice` is created, it takes some time to initialize and be ready, the caller needs to wait for this to happen.
   */
  async waitForReady(): Promise<boolean> {
    const clientCtx = getClientContext();
    if (clientCtx.xrDevice?.enabled) {
      this.enabled = true;
      this.stereoRenderingMode = <StereoRenderingMode>clientCtx.xrDevice.stereoRenderingMode;
    } else {
      this.enabled = false;
    }
    logger.info(`[XRDevice] enabled: ${this.enabled ? 'YES' : 'NO'}`);
    logger.info(`[XRDevice] active: ${clientCtx.xrDevice?.active ? 'YES' : 'NO'}`);
    logger.info(`[XRDevice] Stereo Rendering Mode: ${this.stereoRenderingMode}`);
    return this.enabled;
  }

  getActiveEye(): XREye {
    if (!this.isRenderingInMultiPass()) {
      return 'none';
    }
    const eyeId = this.#handle.getActiveEyeId();
    return eyeId === 0 ? 'left' : 'right';
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

  doesSessionSupportReferenceSpace(_sessionId: number, referenceSpaceType: XRReferenceSpaceType): boolean {
    if (referenceSpaceType === 'local' || referenceSpaceType === 'viewer') {
      return true;
    } else {
      return false;
    }
  }

  requestStageBounds(): object {
    return null;
  }

  async requestFrameOfReferenceTransform(sessionId: number, type: XRReferenceSpaceType, options): Promise<Float32Array> {
    return this.#handle.requestFrameOfReferenceTransform(sessionId, type, options);
  }

  requestAnimationFrame(callback: Transmute.FrameRequestCallback): number {
    return renderer.requestAnimationFrame(callback);
  }

  cancelAnimationFrame(_handle: number): void {
    renderer.cancelAnimationFrame(_handle);
  }

  /**
   * Called when a XRSession has a `baseLayer` property set.
   *
   * @param {number} sessionId
   * @param {XRWebGLLayer} layer
   */
  onBaseLayerSet(_sessionId: number, _layer: XRWebGLLayer): void {
    // Nothing to do here
  }

  getViewport(sessionId: number, eye: XREye, _layer: XRWebGLLayer, target: object, viewIndex: number): boolean {
    const viewport = this.#handle.getViewport(sessionId, eye, viewIndex);
    Object.assign(target, viewport);
    return true;
  }

  getViewSpaces(_mode: XRSessionMode): XRSpace[] {
    return null;
  }

  getInputSources(): XRInputSource[] {
    const r = this.#handle.getGazeInputSource();
    // logger.info('gaze input sources =>', r);
    return [];
  }

  getInputPose(_inputSource: any, _coordinateSystem: any, _poseType: any): XRPose {
    return null;
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
