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
import XRRigidTransformImpl, { XRMatrixPlaceholderType, XRRigidTransformPlaceholder } from './XRRigidTransform';
import XRSpaceImpl, { XRViewSpace } from './XRSpace';

const DEFAULT_EMULATION_HEIGHT = 1.6;
export const PRIVATE = Symbol('@@webxr-polyfill/XRReferenceSpace');
export const XRReferenceSpaceTypes = [
  'viewer',
  'local',
  'local-floor',
  'bounded-floor',
  'unbounded' // TODO: 'unbounded' is not supported by the polyfill.
];

/**
 * @param {string} type 
 * @return {boolean}
 */
function isFloor(type: string) {
  return type === 'bounded-floor' || type === 'local-floor';
}

export default class XRReferenceSpaceImpl extends XRSpaceImpl implements XRReferenceSpace {
  onreset: XRReferenceSpaceEventHandler;

  [PRIVATE]: {
    type: XRReferenceSpaceType,
    transform: Float32Array,
    originOffset: Float32Array,
  };

  /**
   * Optionally takes a `transform` from a device's requestFrameOfReferenceMatrix
   * so device's can provide their own transforms for stage (or if they
   * wanted to override eye-level/head-model).
   *
   * @param {XRReferenceSpaceType} type
   * @param {Float32Array?} transform
   */
  constructor(type: XRReferenceSpaceType, transform: Float32Array = null) {
    if (!XRReferenceSpaceTypes.includes(type)) {
      throw new Error(`XRReferenceSpaceType must be one of ${XRReferenceSpaceTypes}`);
    }

    super(type);

    // If stage emulation is disabled, and this is a stage frame of reference,
    // and the XRDevice did not provide a transform, this is an invalid
    // configuration and we shouldn't emulate here. XRSession.requestFrameOfReference
    // should check this as well.
    if (type === 'bounded-floor' && !transform) {
      throw new Error(`XRReferenceSpace cannot use 'bounded-floor' type if the platform does not provide the floor level`);
    }

    // If we're using floor-level reference and no transform, we're emulating.
    // Set emulated height from option or use the default
    if (isFloor(type) && !transform) {
      // Apply an emulated height to the `y` translation
      transform = <Float32Array>mat4.identity(new Float32Array(16));
      transform[13] = DEFAULT_EMULATION_HEIGHT;
    }

    this._inverseBaseMatrix = transform || <Float32Array>mat4.identity(new Float32Array(16));
    this[PRIVATE] = {
      type,
      transform,
      originOffset: <Float32Array>mat4.identity(new Float32Array(16)),
    };
  }

  /**
   * NON-STANDARD
   * Takes a base pose model matrix and transforms it by the
   * frame of reference.
   */
  _transformBasePoseMatrix(out: Float32Array, pose: Float32Array) {
    mat4.multiply(out, this._inverseBaseMatrix, pose);
  }

  /**
   * NON-STANDARD
   */
  _originOffsetMatrix(): Float32Array {
    return this[PRIVATE].originOffset;
  }

  /**
   * transformMatrix = Inv(OriginOffsetMatrix) * transformMatrix
   * @param {Float32Array} transformMatrix 
   */
  _adjustForOriginOffset(transformMatrix: Float32Array) {
    let inverseOriginOffsetMatrix = new Float32Array(16);
    mat4.invert(inverseOriginOffsetMatrix, this[PRIVATE].originOffset);
    mat4.multiply(transformMatrix, inverseOriginOffsetMatrix, transformMatrix);
  }

  /**
   * Gets the transform of the given space in this space
   *
   * @param {XRSpace} space
   * @return {XRRigidTransform}
   */
  _getSpaceRelativeTransform(space: XRSpace): XRRigidTransformImpl {
    const transform = super._getSpaceRelativeTransform(<XRSpaceImpl>space);
    // TODO: Optimize away double creation of XRRigidTransform
    this._adjustForOriginOffset(transform.matrix);

    if (space instanceof XRViewSpace) {
      const currentType = this[PRIVATE].type;
      if (currentType === 'local') {
        return new XRRigidTransformPlaceholder(
          XRMatrixPlaceholderType.VIEW_MATRIX_RELATIVE_TO_LOCAL, transform.matrix);
      }
      if (currentType === 'local-floor') {
        return new XRRigidTransformPlaceholder(
          XRMatrixPlaceholderType.VIEW_MATRIX_RELATIVE_TO_LOCAL_FLOOR, transform.matrix);
      }
      if (currentType === 'unbounded') {
        return new XRRigidTransformPlaceholder(
          XRMatrixPlaceholderType.VIEW_MATRIX, transform.matrix);
      }
      // Supports more types?
    }
    return new XRRigidTransformImpl(transform.matrix);
  }

  /**
   * Doesn't update the bound geometry for bounded reference spaces.
   */
  getOffsetReferenceSpace(additionalOffset): XRReferenceSpace {
    const newSpace = new XRReferenceSpaceImpl(
      this[PRIVATE].type,
      this[PRIVATE].transform);
    mat4.multiply(newSpace[PRIVATE].originOffset, this[PRIVATE].originOffset, additionalOffset.matrix);
    return newSpace;
  }
}
