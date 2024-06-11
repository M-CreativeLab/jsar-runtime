
import * as logger from '@transmute/logger';
import { getClientContext } from '@transmute/env';
import * as renderer from '@transmute/renderer';

import type XRRenderState from '../api/XRRenderState';
import XRDevice, { StereoRenderingMode } from './XRDevice';
import { DeviceFrameContext } from '../api/XRSession';

const { XRDeviceNative } = process._linkedBinding('transmute:webxr');

export default class XRNativeDevice extends XRDevice {
  #handle: Transmute.XRDeviceNative;

  constructor() {
    super();
    this.environmentBlendMode = 'additive';
    this.#handle = new XRDeviceNative();
  }

  get handle(): Transmute.XRDeviceNative {
    return this.#handle;
  }

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

  requestAnimationFrame(callback: Transmute.FrameRequestCallback): number {
    return renderer.requestAnimationFrame(callback);
  }

  cancelAnimationFrame(_handle: number): void {
    renderer.cancelAnimationFrame(_handle);
  }

  onBaseLayerSet(_sessionId: number, _layer: XRWebGLLayer): void {
    // Nothing to do here
  }

  onFrameStart(sessionId: number, frameContext: DeviceFrameContext, _renderState: XRRenderState): void {
    let passIndex = 0;
    if (this.stereoRenderingMode === StereoRenderingMode.MultiPass) {
      passIndex = frameContext.activeEyeId === 0 ? 0 : 1;
    }
    this.#handle.startFrame(sessionId, frameContext.stereoId, passIndex);
  }

  onFrameEnd(sessionId: number, frameContext: DeviceFrameContext): void {
    let passIndex = 0;
    if (this.stereoRenderingMode === StereoRenderingMode.MultiPass) {
      passIndex = frameContext.activeEyeId === 0 ? 0 : 1;
    }
    this.#handle.endFrame(sessionId, frameContext.stereoId, passIndex);
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
}
