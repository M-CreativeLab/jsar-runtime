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
import * as logger from '../../bindings/logger';
export const PRIVATE = Symbol('@@webxr-polyfill/XRRigidTransform');

type Quaternion = {
  x: number,
  y: number,
  z: number,
  w: number,
};

export default class XRRigidTransform {
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

  get matrix() {
    return this[PRIVATE].matrix;
  }

  get position() {
    return this[PRIVATE].position;
  }

  get orientation() {
    return this[PRIVATE].orientation;
  }

  get inverse() {
    if (this[PRIVATE].inverse === null) {
      let invMatrix = <Float32Array>mat4.identity(new Float32Array(16));
      mat4.invert(invMatrix, this[PRIVATE].matrix);
      this[PRIVATE].inverse = new XRRigidTransform(invMatrix);
      this[PRIVATE].inverse[PRIVATE].inverse = this;
    }
    return this[PRIVATE].inverse;
  }
}
