/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import type { DeviceFrameContext } from '../api/XRSession';

export enum StereoRenderingMode {
  MultiPass = 0,
  SinglePass = 1,
  SinglePassInstanced = 2,
  SinglePassMultiview = 3,
  Unkown = -1,
}

export function stereoRenderingModeToString(mode: StereoRenderingMode): string {
  switch (mode) {
    case StereoRenderingMode.MultiPass:
      return 'multi-pass';
    case StereoRenderingMode.SinglePass:
      return 'single-pass';
    case StereoRenderingMode.SinglePassInstanced:
      return 'single-pass-instanced';
    case StereoRenderingMode.SinglePassMultiview:
      return 'single-pass-multiview';
    case StereoRenderingMode.Unkown:
    default:
      return 'unknown';
  }
}

export default class XRDevice extends EventTarget {
  environmentBlendMode: 'opaque' | 'additive' | 'alpha-blend';
  protected enabled: boolean;
  protected stereoRenderingMode: StereoRenderingMode = -1;

  /**
   * Takes a VRDisplay object from the WebVR 1.1 spec.
   */
  constructor() {
    super();

    // Value is used for `XRSession.prototype.environmentBlendMode`
    // and should be one of XREnvironmentBlendMode types: 'opaque', 'additive',
    // or 'alpha-blend'.
    this.environmentBlendMode = 'opaque';
  }

  /**
   * When a `XRDevice` is created, it takes some time to initialize and be ready, the caller needs to wait for this to happen.
   */
  waitForReady(): Promise<void> {
    throw new Error('Method(device.waitForReady) not implemented');
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
    if (this.stereoRenderingMode === StereoRenderingMode.Unkown) {
      throw new Error('XRDevice.stereoRenderingMode is unknown or not set. Please call XRDevice.waitForReady() first.');
    }
    return this.stereoRenderingMode === StereoRenderingMode.MultiPass;
  }

  getActiveEye(): XREye {
    throw new Error('Method(device.getActiveEye) not implemented');
  }

  /**
   * Called when a XRSession has a `baseLayer` property set.
   *
   * @param {number} sessionId
   * @param {XRWebGLLayer} layer
   */
  onBaseLayerSet(_sessionId: number, _layer: XRWebGLLayer) {
    throw new Error('Method(device.onBaseLayerSet) not implemented');
  }

  /**
   * returns if the session mode is supported.
   */
  isSessionSupported(_mode: XRSessionMode): boolean {
    throw new Error('Method(device.isSessionSupported) not implemented');
  }

  /**
   * @param {string} featureDescriptor
   * @return {boolean}
   */
  isFeatureSupported(_featureDescriptor: string): boolean {
    throw new Error('Method(device.isFeatureSupported) not implemented');
  }

  /**
   * Returns a promise if creating a session is successful.
   * Usually used to set up presentation in the device.
   *
   * @param {XRSessionMode} mode
   * @param {Set<string>} enabledFeatures
   * @return {Promise<number>}
   */
  async requestSession(_mode: XRSessionMode, _enabledFeatures: Set<string>, _sessionId: number): Promise<number> {
    throw new Error('Method(device.requestSession) not implemented');
  }

  /**
   * @return {Function}
   */
  requestAnimationFrame(_callback: Transmute.FrameRequestCallback): number {
    throw new Error('Method(device.requestAnimationFrame) not implemented');
  }

  /**
   * @param {number} sessionId
   */
  onFrameStart(_sessionId: number, _frameContext: DeviceFrameContext, _renderState: XRRenderState) {
    throw new Error('Method(device.onFrameStart) not implemented');
  }

  /**
   * @param {number} sessionId
   */
  onFrameEnd(_sessionId: number, _frameContext: DeviceFrameContext) {
    throw new Error('Method(device.onFrameEnd) not implemented');
  }

  /**
   * @param {number} sessionId
   * @param {XRReferenceSpaceType} type
   * @return {boolean}
   */
  doesSessionSupportReferenceSpace(_sessionId: number, _type: XRReferenceSpaceType): boolean {
    throw new Error('Method(device.doesSessionSupportReferenceSpace) not implemented');
  }

  /**
   * @return {Object?}
   */
  requestStageBounds(): object {
    throw new Error('Method(device.requestStageBounds) not implemented');
  }

  /**
   * Returns a promise resolving to a transform if XRDevice
   * can support frame of reference and provides its own values.
   * Can resolve to `undefined` if the polyfilled API can provide
   * a default. Rejects if this XRDevice cannot
   * support the frame of reference.
   */
  async requestFrameOfReferenceTransform(_sessionId: number, _type: XRReferenceSpaceType, _options?): Promise<Float32Array> {
    throw new Error('Method(device.requestFrameOfReferenceTransform) not implemented');
  }

  /**
   * @param {number} handle
   */
  cancelAnimationFrame(_handle: number) {
    throw new Error('Method(device.cancelAnimationFrame) not implemented');
  }

  /**
   * @param {number} sessionId
   */
  endSession(_sessionId: number) {
    throw new Error('Method(device.endSession) not implemented');
  }

  /**
   * Allows the XRDevice to override the XRSession's view spaces.
   *
   * @param {XRSessionMode} mode
   * @return {Array<XRSpace> | undefined}
   */
  getViewSpaces(_mode: XRSessionMode): Array<XRSpace> | undefined {
    throw new Error('Method(device.getViewSpaces) not implemented');
  }

  /**
   * Takes a XREye and a target to apply properties of
   * `x`, `y`, `width` and `height` on. Returns a boolean
   * indicating if it successfully was able to populate
   * target's values.
   */
  getViewport(
    _sessionId: number,
    _eye: XREye,
    _layer: XRWebGLLayer,
    _target: object,
    _viewIndex: number
  ): boolean {
    throw new Error('Method(device.getViewport) not implemented');
  }

  /**
   * @param {XREye} eye
   * @param {number} viewIndex
   * @return {Float32Array}
   */
  getProjectionMatrix(_eye: XREye, _viewIndex: number): Float32Array {
    throw new Error('Method(device.getProjectionMatrix) not implemented');
  }

  /**
   * Get model matrix unaffected by frame of reference.
   *
   * @return {Float32Array}
   */
  getBasePoseMatrix(): Float32Array {
    throw new Error('Method(device.getBasePoseMatrix) not implemented');
  }

  /**
   * Get view matrix unaffected by frame of reference.
   */
  getBaseViewMatrix(_eye: XREye): Float32Array {
    throw new Error('Method(device.getBaseViewMatrix) not implemented');
  }

  /**
   * Get a list of input sources.
   *
   * @return {Array<XRInputSource>}
   */
  getInputSources(): Array<XRInputSource> {
    throw new Error('Method(device.getInputSources) not implemented');
  }

  /**
   * Get the current pose of an input source.
   *
   * @param {XRInputSource} inputSource
   * @param {XRCoordinateSystem} coordinateSystem
   * @param {String} poseType
   * @return {XRPose}
   */
  getInputPose(_inputSource, _coordinateSystem, _poseType): XRPose {
    throw new Error('Method(device.getInputPose) not implemented');
  }

  /**
   * Called on window resize.
   */
  onWindowResize() {
    // Bound by XRDevice and called on resize, but
    // this will call child class onWindowResize (or, if not defined,
    // call an infinite loop I guess)
    this.onWindowResize();
  }
}
