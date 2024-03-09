
import * as logger from '../../bindings/logger';
import { makeRpcCall } from '../../bindings/messaging';
import * as renderer from '../../bindings/renderer';

import XRDevice, { StereoRenderingMode, stereoRenderingModeToString } from './XRDevice';
import XRWebGLLayer from '../api/XRWebGLLayer';
import XRPose from '../api/XRPose';
import XRSpace from '../api/XRSpace';

const { XRDeviceNative } = process._linkedBinding('transmute:webxr');

type DeviceInitResponse = {
  enabled: boolean;
  isDeviceActive: boolean;
  stereoRenderingMode: StereoRenderingMode;
};

export default class XRNativeDevice extends XRDevice {
  #handle: Transmute.XRDeviceNative;

  constructor() {
    super();
    this.environmentBlendMode = 'additive';
    this.#handle = new XRDeviceNative();
  }

  async waitForReady(): Promise<void> {
    const response = await makeRpcCall('xr.initializeDevice', []) as DeviceInitResponse;
    this.enabled = response.enabled;
    this.stereoRenderingMode = response.stereoRenderingMode || StereoRenderingMode.MultiPass;
    logger.info(`XR: Device enabled: ${this.enabled}`);
    logger.info(`XR: Stereo rendering mode: ${stereoRenderingModeToString(this.stereoRenderingMode)}`);
  }

  getActiveEye(): XREye {
    if (!this.isRenderingInMultiPass()) {
      return 'none';
    }
    const eyeId = this.#handle.getActiveEyeId();
    return eyeId === 0 ? 'left' : 'right';
  }

  isSessionSupported(mode: XRSessionMode): boolean {
    return mode === 'immersive-ar';
  }

  isFeatureSupported(_featureDescriptor: string): boolean {
    return true;
  }

  async requestSession(_mode: XRSessionMode, _enabledFeatures: Set<string>, sessionId: number): Promise<number> {
    this.#handle.requestSession(sessionId);
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

  requestAnimationFrame(callback: any): number {
    return renderer.requestAnimationFrame(callback);
  }

  cancelAnimationFrame(_handle: number): void {
    renderer.cancelAnimationFrame(_handle);
  }

  onBaseLayerSet(_sessionId: number, _layer: XRWebGLLayer): void {
    // Nothing to do here
  }

  onFrameStart(_sessionId: number, _renderState: any): void {
    // Nothing to do here
  }

  onFrameEnd(_sessionId: number): void {
    // Nothing to do here
  }

  getBasePoseMatrix(): Float32Array {
    return this.#handle.getViewerTransform();
  }

  getBaseViewMatrix(eye: XREye): Float32Array {
    if (eye === 'none') {
      throw new TypeError('eye must be "left" or "right"');
    }
    const id = eye === 'left' ? 0 : 1;
    return this.#handle.getViewerStereoViewMatrix(id);
  }

  getProjectionMatrix(eye: XREye, _viewIndex: number): Float32Array {
    // TODO: use viewIndex?
    if (eye === 'none') {
      throw new TypeError('eye must be "left" or "right"');
    }
    const id = eye === 'left' ? 0 : 1;
    return this.#handle.getViewerStereoProjectionMatrix(id);
  }

  getViewSpaces(_mode: XRSessionMode): XRSpace[] {
    return null;
  }

  getInputSources(): XRInputSource[] {
    return [];
  }

  getInputPose(_inputSource: any, _coordinateSystem: any, _poseType: any): XRPose {
    return null;
  }
}
