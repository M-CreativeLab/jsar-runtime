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

import { type XRDevice } from '../devices';
import { type DeviceFrameContext } from './XRSession';
import XRRigidTransformImpl, {
  XRMatrixPlaceholderType,
  XRMatrixPlaceholder
} from './XRRigidTransform';
import XRViewportImpl from './XRViewport';

const XREyes = ['left', 'right', 'none'];
export const PRIVATE = Symbol('@@webxr-polyfill/XRView');

export default class XRViewImpl implements XRView {
  recommendedViewportScale?: number;
  camera: XRCamera;

  [PRIVATE]: {
    device: XRDevice,
    transform: XRRigidTransformImpl,
    eye: XREye,
    viewport: XRViewportImpl,
    temp: Object,
    sessionId: number,
    viewIndex: number,
    frameContext: DeviceFrameContext
  };

  constructor(
    device: XRDevice,
    transform: XRRigidTransform,
    eye: XREye,
    sessionId: number,
    viewIndex: number,
    frameContext: DeviceFrameContext
  ) {
    if (!XREyes.includes(eye)) {
      throw new Error(`XREye must be one of: ${XREyes}`);
    }

    // Create a shared object that can be updated by other code
    // that can update XRViewport values to adhere to API.
    // Ugly but it works.
    const temp = Object.create(null);
    const viewport = new XRViewportImpl(temp);

    this[PRIVATE] = {
      device,
      eye,
      viewport,
      temp,
      sessionId,
      transform: <XRRigidTransformImpl>transform,
      viewIndex,
      frameContext
    };
  }

  /**
   * @return {XREye}
   */
  get eye() {
    return this[PRIVATE].eye;
  }

  /**
   * @return {Float32Array}
   */
  get projectionMatrix() {
    const { frameContext } = this[PRIVATE];
    return new XRMatrixPlaceholder(
      frameContext.viewerProjectionMatrix,
      XRMatrixPlaceholderType.PROJECTION_MATRIX
    );
  }

  /**
   * @return {XRRigidTransform}
   */
  get transform() {
    return this[PRIVATE].transform;
  }

  /**
   * Request viewport scale.
   */
  requestViewportScale(scale: number): void {
    throw new TypeError(`The method requestViewportScale(${scale}) not implemented.`);
  }

  /**
   * NON-STANDARD
   *
   * `getViewport` is now exposed via XRWebGLLayer instead of XRView.
   * XRWebGLLayer delegates all the actual work to this function.
   *
   * @param {XRWebGLLayer} layer
   * @return {XRViewport?}
   */
  _getViewport(layer: XRWebGLLayer) {
    if (
      this[PRIVATE].device.getViewport(
        this[PRIVATE].sessionId,
        this.eye,
        layer,
        this[PRIVATE].temp,
        this[PRIVATE].viewIndex)
    ) {
      return this[PRIVATE].viewport;
    } else {
      return undefined;
    }
  }
}