/*
 * Copyright 2019 Google Inc. All Rights Reserved.
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
import { mat4, vec3, quat } from 'gl-matrix';
export const PRIVATE = Symbol('@@webxr-polyfill/XRRigidTransform');

type Quaternion = {
  x: number,
  y: number,
  z: number,
  w: number,
};

/**
 * A XR matrix placeholder is to represent a matrix that is not used immediately in JSAR's rendering pipeline.
 * 
 * The projection and view matrix in headset's rendering pipeline is such important that it's related to XR experience. In JSAR
 * WebXR implementation, we have to render Web-provided content in delayed frames without blocking the main rendering thread.
 * Thus when a render command is submitted to the main render queue, it uses the old matrices to render with new frame, it will
 * cause a visual glitch.
 * 
 * To address this issue, we introduce the `XRMatrixPlaceholder` to represent a matrix such as view matrix, when the main renderer
 * is to get a `XRMatrixPlaceholder` matrix, it will know how to read the real-time matrix from main thread, thus it can get the
 * correct matrix to render the frame, and the visual glitch is avoided.
 * 
 * More exactly, the result of rendering JSAR's WebXR frame is a real-time view/projection matrix plus the old content, even if
 * the content is delayed, but we keep the visual consistency, right?
 */
export enum XRMatrixPlaceholderType {
  PROJECTION_MATRIX = 10,
  INVERSE_PROJECTION_MATRIX = 11,
  VIEW_MATRIX = 20,
  INVERSE_VIEW_MATRIX = 21,
  VIEW_MATRIX_RELATIVE_TO_LOCAL = 30,
  INVERSE_VIEW_MATRIX_RELATIVE_TO_LOCAL = 31,
  VIEW_MATRIX_RELATIVE_TO_LOCAL_FLOOR = 40,
  INVERSE_VIEW_MATRIX_RELATIVE_TO_LOCAL_FLOOR = 41,
  VIEW_PROJECTION_MATRIX = 100,
  VIEW_PROJECTION_MATRIX_RELATIVE_TO_LOCAL = 101,
  VIEW_PROJECTION_MATRIX_RELATIVE_TO_LOCAL_FLOOR = 102,
  /**
   * More view matrix types can be added here.
   * 
   * It depends on the reference space type, by usual the view matrix in userland is returned in:
   * 
   * ```
   * const baseSpace = xrSession.requestReferenceSpace('local');  // or 'local-floor' or 'bounded-floor' and others
   * xrSession.requestAnimationFrame((time, frame) => {
   *   const pose = frame.getViewerPose(baseSpace);
   *   const viewMatrix = pose.views[0].transform.matrix; // this matrix is the view matrix relative to the baseSpace.
   * });
   * ```
   */
}

export class XRMatrixPlaceholder extends Float32Array {
  type: XRMatrixPlaceholderType;
  xrSessionId: number;
  _isXRMatrixPlaceholder = true;
  constructor(from: Float32Array, type: XRMatrixPlaceholderType, xrSessionId?: number) {
    super(from);
    this.type = type;
    this.xrSessionId = xrSessionId;
  }
}

export default class XRRigidTransformImpl implements XRRigidTransform {
  [PRIVATE]: {
    matrix: Float32Array,
    position: Quaternion,
    orientation: Quaternion,
    inverse: XRRigidTransform,
  };

  // no arguments: identity transform
  // (Float32Array): transform based on matrix
  // (Quaternion): transform based on position without any rotation
  // (Quaternion, Quaternion): transform based on position and
  // orientation quaternion
  constructor(_positionOrMatrix?: Quaternion | Float32Array, _orientation?: Quaternion) {
    this[PRIVATE] = {
      matrix: null,
      position: null,
      orientation: null,
      inverse: null,
    };

    if (arguments.length === 0) {
      this[PRIVATE].matrix = <Float32Array>mat4.identity(new Float32Array(16));
    } else if (arguments.length === 1) {
      if (arguments[0] instanceof Float32Array) {
        this[PRIVATE].matrix = arguments[0];
      } else {
        this[PRIVATE].position = this._getPoint(arguments[0]);
        this[PRIVATE].orientation = { x: 0, y: 0, z: 0, w: 1 };
      }
    } else if (arguments.length === 2) {
      this[PRIVATE].position = this._getPoint(arguments[0]);
      this[PRIVATE].orientation = this._getPoint(arguments[1]);
    } else {
      throw new TypeError('Too many arguments.');
    }

    if (this[PRIVATE].matrix) {
      // Decompose matrix into position and orientation.
      let position = vec3.create();
      mat4.getTranslation(position, this[PRIVATE].matrix);
      this[PRIVATE].position = {
        x: position[0],
        y: position[1],
        z: position[2],
        w: 1,
      };

      let orientation = quat.create();
      mat4.getRotation(orientation, this[PRIVATE].matrix);
      this[PRIVATE].orientation = {
        x: orientation[0],
        y: orientation[1],
        z: orientation[2],
        w: orientation[3]
      };
    } else {
      // Compose matrix from position and orientation.
      this[PRIVATE].matrix = <Float32Array>mat4.identity(new Float32Array(16));
      mat4.fromRotationTranslation(
        this[PRIVATE].matrix,
        quat.fromValues(
          this[PRIVATE].orientation.x,
          this[PRIVATE].orientation.y,
          this[PRIVATE].orientation.z,
          this[PRIVATE].orientation.w),
        vec3.fromValues(
          this[PRIVATE].position.x,
          this[PRIVATE].position.y,
          this[PRIVATE].position.z)
      );
    }
  }

  /**
   * Try to convert arg to a {x, y, z, w} if it isn't already one.
   */
  private _getPoint(arg: any) {
    return {
      x: arg?.x || 0,
      y: arg?.y || 0,
      z: arg?.z || 0,
      w: arg?.w || 1,
    };
  }

  get matrix(): Float32Array {
    return this[PRIVATE].matrix;
  }

  get position() {
    return <any>this[PRIVATE].position;
  }

  get orientation() {
    return <any>this[PRIVATE].orientation;
  }

  get inverse() {
    if (this[PRIVATE].inverse === null) {
      let invMatrix = <Float32Array>mat4.identity(new Float32Array(16));
      mat4.invert(invMatrix, this[PRIVATE].matrix);
      this[PRIVATE].inverse = new XRRigidTransformImpl(invMatrix);
      this[PRIVATE].inverse[PRIVATE].inverse = this;
    }
    return this[PRIVATE].inverse;
  }
}

export class XRRigidTransformPlaceholder extends XRRigidTransformImpl {
  matrixPlaceholderType: XRMatrixPlaceholderType;
  constructor(type: XRMatrixPlaceholderType, positionOrMatrix?: Quaternion | Float32Array, orientation?: Quaternion) {
    super(positionOrMatrix, orientation);
    this.matrixPlaceholderType = type;
  }

  get matrix() {
    return new XRMatrixPlaceholder(super.matrix, this.matrixPlaceholderType);
  }

  get inverse() {
    let type: XRMatrixPlaceholderType;
    switch (this.matrixPlaceholderType % 10) {
      case 0:
        /** 10, 20, 30... means base matrix, so just +1 to mark it as the inverse matrix */
        type = this.matrixPlaceholderType + 1;
        break;
      case 1:
        /** 11, 21, 31... means inverse matrix, so just -1 to mark it as the base matrix */
        type = this.matrixPlaceholderType - 1;
        break;
      default:
        throw new TypeError('The matrix placeholder type is invalid.');
    }
    return new XRRigidTransformPlaceholder(type, super.inverse.matrix);
  }
}
