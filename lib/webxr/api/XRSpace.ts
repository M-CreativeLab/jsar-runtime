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
import { mat4 } from 'gl-matrix';

import { type XRDevice } from '../devices';
import XRRigidTransformImpl from './XRRigidTransform';
import { DeviceFrameContext } from './XRSession';

export const PRIVATE = Symbol('@@webxr-polyfill/XRSpace');

// Not exposed, for reference only
export const XRSpaceSpecialTypes = ['grip', 'target-ray'];

export default class XRSpaceImpl extends EventTarget implements XRSpace {
  [PRIVATE]: {
    specialType: string,
    inputSource: any,
    baseMatrix: Float32Array,
    inverseBaseMatrix: Float32Array,
    lastFrameId: number
  };

  constructor(specialType: string = null, inputSource: any = null) {
    super();

    this[PRIVATE] = {
      specialType,
      inputSource,
      // The transform for the space in the base space, along with it's inverse
      baseMatrix: null,
      inverseBaseMatrix: null,
      lastFrameId: -1
    };
  }

  /**
   * @return {string?}
   */
  get _specialType() {
    return this[PRIVATE].specialType;
  }

  /**
   * @return {XRInputSource?}
   */
  get _inputSource() {
    return this[PRIVATE].inputSource;
  }

  /**
   * NON-STANDARD
   * Trigger an update for this space's base pose if necessary
   */
  _ensurePoseUpdated(device: XRDevice, frameId: number, frameContext: DeviceFrameContext) {
    if (frameId == this[PRIVATE].lastFrameId) {
      return;
    }
    this[PRIVATE].lastFrameId = frameId;
    this._onPoseUpdate(device, frameContext);
  }

  /**
   * NON-STANDARD
   * Called when this space's base pose needs to be updated
   */
  _onPoseUpdate(_device: XRDevice, frameContext: DeviceFrameContext) {
    const { specialType } = this[PRIVATE];
    if (specialType == 'viewer') {
      this._baseMatrix = frameContext.viewerTransform;
    } else if (specialType == 'local') {
      this._baseMatrix = frameContext.localTransform;
    }
    // TODO: support other types?
  }

  set _baseMatrix(matrix: Float32Array) {
    this[PRIVATE].baseMatrix = matrix;
    this[PRIVATE].inverseBaseMatrix = null;
  }
  get _baseMatrix() {
    if (!this[PRIVATE].baseMatrix) {
      if (this[PRIVATE].inverseBaseMatrix) {
        this[PRIVATE].baseMatrix = new Float32Array(16);
        mat4.invert(this[PRIVATE].baseMatrix, this[PRIVATE].inverseBaseMatrix);
      }
    }
    return this[PRIVATE].baseMatrix;
  }

  set _inverseBaseMatrix(matrix: Float32Array) {
    this[PRIVATE].inverseBaseMatrix = matrix;
    this[PRIVATE].baseMatrix = null;
  }
  get _inverseBaseMatrix() {
    if (!this[PRIVATE].inverseBaseMatrix) {
      if (this[PRIVATE].baseMatrix) {
        this[PRIVATE].inverseBaseMatrix = new Float32Array(16);
        mat4.invert(this[PRIVATE].inverseBaseMatrix, this[PRIVATE].baseMatrix);
      }
    }
    return this[PRIVATE].inverseBaseMatrix;
  }

  /**
   * NON-STANDARD
   * Gets the transform of the given space in this space
   */
  _getSpaceRelativeTransform(space: XRSpaceImpl): XRRigidTransformImpl {
    if (!this._inverseBaseMatrix || !space._baseMatrix) {
      return null;
    }
    let out = new Float32Array(16);
    mat4.multiply(out, this._inverseBaseMatrix, space._baseMatrix);
    return new XRRigidTransformImpl(out);
  }
}

// Nonstandard helper class. Not exposed by the API anywhere.
export class XRViewSpace extends XRSpaceImpl {
  get eye(): XREye {
    if (this._specialType !== 'left' && this._specialType !== 'right' && this._specialType !== 'none') {
      throw new Error('XRViewSpace eye property is only available for left and right view spaces');
    }
    return <XREye>this._specialType;
  }

  /**
   * Called when this space's base pose needs to be updated
   */
  _onPoseUpdate(_device: XRDevice, frameContext: DeviceFrameContext): void {
    // this._inverseBaseMatrix = frameContext.viewerViewMatrix;
    this._baseMatrix = frameContext.viewerViewMatrix;
  }
}
