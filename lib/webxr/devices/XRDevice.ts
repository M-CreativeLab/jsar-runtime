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

// import XRReferenceSpace from '../api/XRReferenceSpace';

export default class XRDevice extends EventTarget {
  environmentBlendMode: 'opaque' | 'additive' | 'alpha-blend';

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
    throw new Error('Not implemented');
  }

  /**
   * Called when a XRSession has a `baseLayer` property set.
   *
   * @param {number} sessionId
   * @param {XRWebGLLayer} layer
   */
  onBaseLayerSet(_sessionId: number, _layer) {
    throw new Error('Not implemented');
  }

  /**
   * returns if the session mode is supported.
   */
  isSessionSupported(_mode: XRSessionMode): boolean {
    throw new Error('Not implemented');
  }

  /**
   * @param {string} featureDescriptor
   * @return {boolean}
   */
  isFeatureSupported(_featureDescriptor: string): boolean {
    throw new Error('Not implemented');
  }

  /**
   * Returns a promise if creating a session is successful.
   * Usually used to set up presentation in the device.
   *
   * @param {XRSessionMode} mode
   * @param {Set<string>} enabledFeatures
   * @return {Promise<number>}
   */
  async requestSession(_mode: XRSessionMode, _enabledFeatures: Set<string>): Promise<number> {
    throw new Error('Not implemented');
  }

  /**
   * @return {Function}
   */
  requestAnimationFrame(_callback) {
    throw new Error('Not implemented');
  }

  /**
   * @param {number} sessionId
   */
  onFrameStart(_sessionId: number, _renderState) {
    throw new Error('Not implemented');
  }

  /**
   * @param {number} sessionId
   */
  onFrameEnd(_sessionId: number) {
    throw new Error('Not implemented');
  }

  /**
   * @param {number} sessionId
   * @param {XRReferenceSpaceType} type
   * @return {boolean}
   */
  doesSessionSupportReferenceSpace(_sessionId: number, _type: XRReferenceSpaceType) {
    throw new Error('Not implemented');
  }

  /**
   * @return {Object?}
   */
  requestStageBounds() {
    throw new Error('Not implemented');
  }

  /**
   * Returns a promise resolving to a transform if XRDevice
   * can support frame of reference and provides its own values.
   * Can resolve to `undefined` if the polyfilled API can provide
   * a default. Rejects if this XRDevice cannot
   * support the frame of reference.
   *
   * @param {XRFrameOfReferenceType} type
   * @param {XRFrameOfReferenceOptions} options
   * @return {Promise<XRFrameOfReference>}
   */
  async requestFrameOfReferenceTransform(type, options) {
    return undefined;
  }

  /**
   * @param {number} handle
   */
  cancelAnimationFrame(_handle: number) {
    throw new Error('Not implemented');
  }

  /**
   * @param {number} sessionId
   */
  endSession(_sessionId: number) {
    throw new Error('Not implemented');
  }

  /**
   * Allows the XRDevice to override the XRSession's view spaces.
   *
   * @param {XRSessionMode} mode
   * @return {Array<XRSpace> | undefined}
   */
  getViewSpaces(_mode: XRSessionMode) {
    return undefined;
  }

  /**
   * Takes a XREye and a target to apply properties of
   * `x`, `y`, `width` and `height` on. Returns a boolean
   * indicating if it successfully was able to populate
   * target's values.
   *
   * @param {number} sessionId
   * @param {XREye} eye
   * @param {XRWebGLLayer} layer
   * @param {Object?} target
   * @param {number} viewIndex
   * @return {boolean}
   */
  getViewport(_sessionId: number, _eye, _layer, _target, _viewIndex) {
    throw new Error('Not implemented');
  }

  /**
   * @param {XREye} eye
   * @param {number} viewIndex
   * @return {Float32Array}
   */
  getProjectionMatrix(_eye: XREye, _viewIndex: number) {
    throw new Error('Not implemented');
  }

  /**
   * Get model matrix unaffected by frame of reference.
   *
   * @return {Float32Array}
   */
  getBasePoseMatrix() {
    throw new Error('Not implemented');
  }

  /**
   * Get view matrix unaffected by frame of reference.
   *
   * @param {XREye} eye
   * @return {Float32Array}
   */
  getBaseViewMatrix(_eye: XREye) {
    throw new Error('Not implemented');
  }

  /**
   * Get a list of input sources.
   *
   * @return {Array<XRInputSource>}
   */
  getInputSources() {
    throw new Error('Not implemented');
  }

  /**
   * Get the current pose of an input source.
   *
   * @param {XRInputSource} inputSource
   * @param {XRCoordinateSystem} coordinateSystem
   * @param {String} poseType
   * @return {XRPose}
   */
  getInputPose(inputSource, coordinateSystem, poseType) {
    throw new Error('Not implemented');
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
