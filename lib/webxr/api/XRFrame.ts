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

import * as logger from '../../bindings/logger';
import type XRDevice from '../devices/XRDevice';
import XRSessionImpl, { PRIVATE as SESSION_PRIVATE, type DeviceFrameContext } from './XRSession';
import XRPoseImpl from './XRPose';
import XRViewerPoseImpl from './XRViewerPose';
import XRViewImpl from './XRView';
import XRReferenceSpaceImpl from './XRReferenceSpace';

export const PRIVATE = Symbol('@@webxr-polyfill/XRFrame');
const NON_ACTIVE_MSG = 'XRFrame access outside the callback that produced it is invalid.';
const NON_ANIMFRAME_MSG = 'getViewerPose can only be called on XRFrame objects passed to XRSession.requestAnimationFrame callbacks.';

let NEXT_FRAME_ID = 0;

export default class XRFrameImpl implements XRFrame {
  predictedDisplayTime?: number;
  trackedAnchors?: XRAnchorSet;
  worldInformation?: XRWorldInformation;
  detectedPlanes?: XRPlaneSet;
  detectedMeshes?: XRMeshSet;
  featurePointCloud?: number[];

  [PRIVATE]: {
    id: number,
    active: boolean,
    animationFrame: boolean,
    device: XRDevice,
    session: XRSessionImpl,
    sessionId: number,
    timestamp: number,
    frameContext: DeviceFrameContext
  };

  /**
   * @param {XRDevice} device
   * @param {XRSession} session
   * @param {number} sessionId
   */
  constructor(
    device: XRDevice,
    session: XRSessionImpl,
    timestamp: number,
    frameContext: DeviceFrameContext
  ) {
    this[PRIVATE] = {
      id: ++NEXT_FRAME_ID,
      active: false,
      animationFrame: false,
      device,
      session,
      sessionId: frameContext.sessionId,
      timestamp,
      frameContext
    };
    // NOTE: DEBUG
    // logger.info('XRFrame created', this[PRIVATE].id, timestamp, frameContext.activeEyeId, frameContext);
  }

  createAnchor?: (pose: XRRigidTransform, space: XRSpace) => Promise<XRAnchor>;

  getHitTestResults(hitTestSource: XRHitTestSource): XRHitTestResult[] {
    throw new Error('Method not implemented.');
  }

  getHitTestResultsForTransientInput(hitTestSource: XRTransientInputHitTestSource): XRTransientInputHitTestResult[] {
    throw new Error('Method not implemented.');
  }

  fillPoses?(spaces: XRSpace[], baseSpace: XRSpace, transforms: Float32Array): boolean {
    throw new Error('Method not implemented.');
  }

  getJointPose?(joint: XRJointSpace, baseSpace: XRSpace): XRJointPose {
    throw new Error('Method not implemented.');
  }

  fillJointRadii?(jointSpaces: XRJointSpace[], radii: Float32Array): boolean {
    throw new Error('Method not implemented.');
  }

  getImageTrackingResults?(): XRImageTrackingResult[] {
    throw new Error('Method not implemented.');
  }

  getLightEstimate(xrLightProbe: XRLightProbe): XRLightEstimate {
    throw new Error('Method not implemented.');
  }

  getDepthInformation(view: XRView): XRCPUDepthInformation {
    throw new Error('Method not implemented.');
  }

  /**
   * @return {XRSession} session
   */
  get session(): XRSession {
    return this[PRIVATE].session;
  }

  getViewerPose(referenceSpace: XRReferenceSpace) {
    if (!this[PRIVATE].animationFrame) {
      throw new TypeError(NON_ANIMFRAME_MSG);
    }
    if (!this[PRIVATE].active) {
      throw new TypeError(NON_ACTIVE_MSG);
    }

    const device = this[PRIVATE].device;
    const session = this[PRIVATE].session;
    const referenceSpaceImpl = <XRReferenceSpaceImpl>referenceSpace;

    const viewerSpaceImpl = session[SESSION_PRIVATE].viewerSpace;
    viewerSpaceImpl._ensurePoseUpdated(device, this[PRIVATE].id, this[PRIVATE].frameContext);
    referenceSpaceImpl._ensurePoseUpdated(device, this[PRIVATE].id, this[PRIVATE].frameContext);

    const activeEye = this.getActiveEye();
    const viewerTransform = referenceSpaceImpl._getSpaceRelativeTransform(viewerSpaceImpl);
    // logger.info('viewer space matrix =>', viewerSpaceImpl._baseMatrix);
    // logger.info('local space matrix =>', referenceSpaceImpl._baseMatrix);
    // logger.info('viewer transform =>', viewerTransform.matrix);

    const views: XRViewImpl[] = [];
    for (const viewSpace of session[SESSION_PRIVATE].viewSpaces) {
      // When rendering in multi-pass mode, only the view for the active eye is used.
      if (device.isRenderingInMultiPass() && viewSpace.eye !== activeEye) {
        continue;
      }
      viewSpace._ensurePoseUpdated(device, this[PRIVATE].id, this[PRIVATE].frameContext);
      const viewTransform = referenceSpaceImpl._getSpaceRelativeTransform(viewSpace);
      const viewIndex = session[SESSION_PRIVATE].viewSpaces.indexOf(viewSpace);
      views.push(
        new XRViewImpl(device, viewTransform, viewSpace.eye, this[PRIVATE].sessionId, viewIndex, this[PRIVATE].frameContext)
      );
    }
    const viewerPose = new XRViewerPoseImpl(viewerTransform, views, false /* TODO: emulatedPosition */);
    return viewerPose;
  }

  /**
   * returns the relative position and orientation—the pose—of one XRSpace to that of another space.
   */
  getPose(space: XRSpace, baseSpace: XRSpace) {
    if (!this[PRIVATE].active) {
      throw new TypeError(NON_ACTIVE_MSG);
    }

    const device = this[PRIVATE].device;
    const spaceImpl = <XRReferenceSpaceImpl>space;
    const baseSpaceImpl = <XRReferenceSpaceImpl>baseSpace;

    if (spaceImpl._specialType === 'target-ray' || spaceImpl._specialType === 'grip') {
      // TODO: Stop special-casing input.
      return device.getInputPose(
        spaceImpl._inputSource, baseSpace, spaceImpl._specialType);
    } else {
      spaceImpl._ensurePoseUpdated(device, this[PRIVATE].id, this[PRIVATE].frameContext);
      baseSpaceImpl._ensurePoseUpdated(device, this[PRIVATE].id, this[PRIVATE].frameContext);
      let transform = baseSpaceImpl._getSpaceRelativeTransform(spaceImpl);
      if (!transform) {
        return null;
      }
      return new XRPoseImpl(transform, false /* TODO: emulatedPosition */);
    }
  }

  private getActiveEye(): XREye {
    const id = this[PRIVATE].frameContext.activeEyeId;
    return id === 0 ? 'left' : 'right';
  }
}
