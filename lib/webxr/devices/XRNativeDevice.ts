
import * as logger from '../../bindings/logger';
import { makeRpcCall } from '../../bindings/messaging';
import * as renderer from '../../bindings/renderer';

import XRDevice from './XRDevice';
import XRWebGLLayer from '../api/XRWebGLLayer';

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

  getInputSources(): XRInputSource[] {
    return [];
  }

  getInputPose(_inputSource: any, _coordinateSystem: any, _poseType: any): void {
    return null;
  }
}
