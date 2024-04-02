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

import { XRDevice } from '../devices';
import { XRViewSpace } from './XRSpace';
import XRFrameImpl, { PRIVATE as XRFRAME_PRIVATE } from './XRFrame';
import XRReferenceSpaceImpl, { XRReferenceSpaceTypes } from './XRReferenceSpace';
import XRRenderStateImpl, { type XRRenderStateInit } from './XRRenderState';
import type XRWebGLLayerImpl from './XRWebGLLayer';
// import XRInputSourceEvent from './XRInputSourceEvent';
import XRSessionEvent from './XRSessionEvent';
// import XRInputSourcesChangeEvent from './XRInputSourcesChangeEvent';
import * as logger from '../../bindings/logger';

export const PRIVATE = Symbol('@@webxr-polyfill/XRSession');

type XRFrameCallbackDescriptor = {
  handle: number;
  cancelled: boolean;
  callback: (time: number, frame: XRFrame) => void;
};

/**
 * This type is to represent the context of a device frame.
 * 
 * _What's a device frame?_
 * 
 * It depends on the device and corresponding native XR implementation, such as Unity XR SDK, OpenXR, etc. For example, in
 * Unity XR with multipass rendering, a device frame is a frame that renders a single eye's view, and the complete views are
 * merged from 2 device frames, one for left eye and one for right.
 * 
 * The context of the device frame is the context information at the time of the device frame, such as the viewer and local
 * matrices, the active eye id, etc.
 * 
 * _Where to use this type?_
 * 
 * Assume that there are 2 frames to render, when the first frame is being rendered, the context will be collected and passed
 * to the JavaScript asynchronously, and then the JavaScript will render the next frame based on the context.
 * 
 * The reason to achieve the asynchronous rendering is to remove the performance bottleneck of the JavaScript thread, thus the
 * native XR implementation can render current frame ASAP, and the JavaScript can render a frame with the context, and then dispatch
 * the frame to the next available device frame.
 */
export type DeviceFrameContext = {
  /**
   * It stands for the Unity's StereoRenderingMode, such as SinglePass, MultiPass, etc.
   * 
   * The WebXR implementation should be able to handle different rendering modes, especially for the "multipass" rendering, which
   * the implementation will create only 1 view in every frame callback.
   */
  type: string;
  /**
   * The stereo id is used to identify the stereo rendering frame, in multipass rendering, a stereo id contains 2 frames, one for
   * left eye and one for right eye.
   */
  stereoId: number;
  /**
   * The session id is used to identify the session that the frame belongs to. Especially at the JSAR implementation, a session
   * stands for an application, the implementation will dispatch to the corresponding session's frame callback by this id.
   */
  sessionId: number;
  /**
   * The "local" is a reference space type in WebXR, in every device frame, the JavaScript implementation could access to this
   * matrix to get the transform of the "local" space.
   */
  localTransform: Float32Array;
  /**
   * See the "localTransform" field, the only difference is that the matrix is for the "viewer" space.
   */
  viewerTransform: Float32Array;
  /**
   * The following fields are only available for "multipass" rendering.
   */
  /**
   * In the "multipass" rendering, every device frame will render a single eye's view, thus the active eye id is used to identify
   * which eye's view is being rendered.
   */
  activeEyeId?: number;
  /**
   * The viewer's view matrix, it works with the `activeEyeId` because the view matrix is represent the left eye or right eye's
   * view.
   */
  viewerViewMatrix?: Float32Array;
  /**
   * See the "viewerViewMatrix" field, the only difference is that the matrix is for the projection.
   */
  viewerProjectionMatrix?: Float32Array;
};

class SessionInternalEvent extends Event {
  sessionId: number;
  inputSource: any;
}

export default class XRSessionImpl extends EventTarget implements XRSession {
  visibilityState: XRVisibilityState;
  domOverlayState?: XRDOMOverlayState;
  frameRate?: number;
  supportedFrameRates?: Float32Array;
  preferredReflectionFormat?: XRReflectionFormat;
  depthUsage: XRDepthUsage;
  depthDataFormat: XRDepthDataFormat;
  enabledFeatures: string[];

  onend: XRSessionEventHandler;
  oninputsourceschange: XRInputSourceChangeEventHandler;
  onselect: XRInputSourceEventHandler;
  onselectstart: XRInputSourceEventHandler;
  onselectend: XRInputSourceEventHandler;
  onsqueeze: XRInputSourceEventHandler;
  onsqueezestart: XRInputSourceEventHandler;
  onsqueezeend: XRInputSourceEventHandler;
  onvisibilitychange: XRSessionEventHandler;
  onframeratechange: XRSessionEventHandler;

  [PRIVATE]: {
    id: number;
    device: XRDevice;
    mode: XRSessionMode;
    immersive: boolean;
    ended: boolean;
    suspended: boolean;
    frameCallbacks: Array<XRFrameCallbackDescriptor>;
    currentFrameCallbacks: null | Array<XRFrameCallbackDescriptor>;
    frameHandle: number;
    deviceFrameHandle: null | number;
    activeRenderState: XRRenderStateImpl;
    pendingRenderState: null | XRRenderStateInit;
    localSpace: XRReferenceSpaceImpl;
    viewerSpace: XRReferenceSpaceImpl;
    viewSpaces: Array<XRViewSpace>;
    currentInputSources: Array<any>;

    startDeviceFrameLoop?: () => void;
    stopDeviceFrameLoop?: () => void;
    dispatchInputSourceEvent?: (type: string, inputSource: any) => void;

    onDeviceFrame?: (time: number, data: DeviceFrameContext) => void;
    onPresentationEnd?: (event: SessionInternalEvent) => void;
    onPresentationStart?: (event: SessionInternalEvent) => void;
    onSelectStart?: (event: SessionInternalEvent) => void;
    onSelectEnd?: (event: SessionInternalEvent) => void;
    onSqueezeStart?: (event: SessionInternalEvent) => void;
    onSqueezeEnd?: (event: SessionInternalEvent) => void;
  };

  /**
   * @param {XRDevice} device
   * @param {XRSessionMode} mode
   * @param {number} id
   */
  constructor(device: XRDevice, mode: XRSessionMode, id: number) {
    super();

    let immersive = mode != 'inline';

    // inlineVerticalFieldOfView must initialize to PI/2 for inline sessions.
    let initialRenderState = new XRRenderStateImpl({
      inlineVerticalFieldOfView: immersive ? null : Math.PI * 0.5
    });

    const defaultViewSpaces = immersive ?
      [new XRViewSpace('left'), new XRViewSpace('right')] :
      [new XRViewSpace('none')];
    Object.freeze(defaultViewSpaces);

    this[PRIVATE] = {
      device,
      mode,
      immersive,
      ended: false,
      suspended: false,
      frameCallbacks: [],
      currentFrameCallbacks: null,
      frameHandle: 0,
      deviceFrameHandle: null,
      id,
      activeRenderState: initialRenderState,
      pendingRenderState: null,
      localSpace: new XRReferenceSpaceImpl('local'),
      viewerSpace: new XRReferenceSpaceImpl('viewer'),
      get viewSpaces() { return <XRViewSpace[]>device.getViewSpaces(mode) || defaultViewSpaces; },
      currentInputSources: []
    };

    // Single handler for animation frames from the device. The spec says this must
    // run on every candidate frame even if there are no callbacks queued up.
    this[PRIVATE].onDeviceFrame = (time, context) => {
      // Queue next frame first to ensure that the frame loop continues.
      if (!this.queueNextFrame()) {
        logger.warn(`[XRSession] onDeviceFrame: queueNextFrame failed because session is ended or suspended.`);
        return;
      }

      // - If session’s pending render state is not null, apply the pending render state.
      if (this[PRIVATE].pendingRenderState !== null) {
        // Apply pending render state.
        this[PRIVATE].activeRenderState = new XRRenderStateImpl(this[PRIVATE].pendingRenderState);
        this[PRIVATE].pendingRenderState = null;

        // Report to the device since it'll need to handle the layer for rendering.
        if (this[PRIVATE].activeRenderState.baseLayer) {
          this[PRIVATE].device.onBaseLayerSet(
            this[PRIVATE].id,
            <XRWebGLLayerImpl>this[PRIVATE].activeRenderState.baseLayer);
        }
      }

      // - If session’s renderState's baseLayer is null, abort these steps.
      if (this[PRIVATE].activeRenderState?.baseLayer === null) {
        logger.warn(`[XRSession] onDeviceFrame: baseLayer is null`);
        return;
      }

      // - If session’s mode is "inline" and session’s renderState's output canvas is null,
      //   abort these steps.
      // ???

      const frame = new XRFrameImpl(device, this, time, context);

      // - Let callbacks be a list of the entries in session’s list of animation frame
      //   callback, in the order in which they were added to the list.
      const callbacks = this[PRIVATE].currentFrameCallbacks = this[PRIVATE].frameCallbacks;

      // - Set session’s list of animation frame callbacks to the empty list.
      this[PRIVATE].frameCallbacks = [];

      // - Set frame’s active boolean to true.
      frame[XRFRAME_PRIVATE].active = true;

      // - Set frame’s animationFrame boolean to true.
      frame[XRFRAME_PRIVATE].animationFrame = true;

      this[PRIVATE].device.onFrameStart(this[PRIVATE].id, context, this[PRIVATE].activeRenderState);
      // inputSources can be populated in .onFrameStart()
      // so check the change and fire inputsourceschange event if needed
      this._checkInputSourcesChange();

      // - For each entry in callbacks, in order:
      //   - If the entry’s cancelled boolean is true, continue to the next entry.
      //   - Invoke the Web IDL callback function, passing now and frame as the arguments
      //   - If an exception is thrown, report the exception.
      const rightNow = performance.now(); //should we get this from arguments?
      for (let i = 0; i < callbacks.length; i++) {
        try {
          if (!callbacks[i].cancelled && typeof callbacks[i].callback === 'function') {
            callbacks[i].callback(rightNow, frame);
          }
        } catch (err) {
          console.error(err);
        }
      }
      this[PRIVATE].currentFrameCallbacks = null;

      // - Set frame’s active boolean to false.
      frame[XRFRAME_PRIVATE].active = false;
      this[PRIVATE].device.onFrameEnd(this[PRIVATE].id, context);
    };

    this[PRIVATE].startDeviceFrameLoop = () => {
      if (this[PRIVATE].deviceFrameHandle === null) {
        this[PRIVATE].deviceFrameHandle = this[PRIVATE].device.requestAnimationFrame((time, data) => {
          if (!data || typeof data !== 'object') {
            logger.warn(`[XRSession] onDeviceFrame skipped, reason is: invalid animation frame, no data found.`, {
              time, data
            });
            this.queueNextFrame();
            return;
          }
          const nativeFrameCtx = <Transmute.NativeFrameContext>data;
          if (!Array.isArray(nativeFrameCtx.sessions) || nativeFrameCtx.sessions.length === 0) {
            logger.warn(`[XRSession] onDeviceFrame skipped, reason is: invalid animation frame, no sessions found or sessions are empty.`, {
              time, data
            });
            this.queueNextFrame();
            return;
          }
          const session = nativeFrameCtx.sessions.find(s => s.sessionId === this[PRIVATE].id);
          if (!session) {
            logger.warn(`[XRSession] onDeviceFrame skipped, reason is: invalid animation frame, no session found.`, {
              time,
              data,
              '[this]': this[PRIVATE],
            });
            this.queueNextFrame();
            return;
          }

          this[PRIVATE].onDeviceFrame(time, {
            type: nativeFrameCtx.type,
            stereoId: nativeFrameCtx.stereoId,
            sessionId: session.sessionId,
            localTransform: session.localTransform,
            viewerTransform: nativeFrameCtx.viewerTransform,
            activeEyeId: nativeFrameCtx.activeEyeId,
            viewerViewMatrix: nativeFrameCtx.viewerViewMatrix,
            viewerProjectionMatrix: nativeFrameCtx.viewerProjectionMatrix,
          });
        });
      }
    };

    this[PRIVATE].stopDeviceFrameLoop = () => {
      const handle = this[PRIVATE].deviceFrameHandle;
      if (handle !== null) {
        this[PRIVATE].device.cancelAnimationFrame(handle);
        this[PRIVATE].deviceFrameHandle = null;
      }
    };

    // Hook into the XRDisplay's `vr-present-end` event so we can
    // wrap up things here if we're cut off from the underlying
    // polyfilled device or explicitly ended via `session.end()` for this
    // session.
    this[PRIVATE].onPresentationEnd = ({ sessionId }) => {
      // If this session was suspended, resume it now that an immersive
      // session has ended.
      if (sessionId !== this[PRIVATE].id) {
        this[PRIVATE].suspended = false;
        this[PRIVATE].startDeviceFrameLoop();
        // this.dispatchEvent('focus', { session: this });
        return;
      }

      // Otherwise, this is the immersive session that has ended.
      // Set `ended` to true so we can disable all functionality
      // in this XRSession
      this[PRIVATE].ended = true;
      this[PRIVATE].stopDeviceFrameLoop();
      device.removeEventListener('@@webxr-polyfill/vr-present-end', this[PRIVATE].onPresentationEnd);
      device.removeEventListener('@@webxr-polyfill/vr-present-start', this[PRIVATE].onPresentationStart);
      device.removeEventListener('@@webxr-polyfill/input-select-start', this[PRIVATE].onSelectStart);
      device.removeEventListener('@@webxr-polyfill/input-select-end', this[PRIVATE].onSelectEnd);
      // this.dispatchEvent('end', new XRSessionEvent('end', { session: this }));
    };
    device.addEventListener('@@webxr-polyfill/vr-present-end', this[PRIVATE].onPresentationEnd);

    // Hook into the XRDisplay's `vr-present-start` event so we can
    // suspend if another session has begun immersive presentation.
    this[PRIVATE].onPresentationStart = ({ sessionId }) => {
      // Ignore if this is the session that has begun immersive presenting
      if (sessionId === this[PRIVATE].id) {
        return;
      }

      this[PRIVATE].suspended = true;
      this[PRIVATE].stopDeviceFrameLoop();
      // this.dispatchEvent('blur', { session: this });
    };
    device.addEventListener('@@webxr-polyfill/vr-present-start', this[PRIVATE].onPresentationStart);

    this[PRIVATE].onSelectStart = evt => {
      // Ignore if this event is not for this session.
      if (evt.sessionId !== this[PRIVATE].id) {
        return;
      }

      this[PRIVATE].dispatchInputSourceEvent('selectstart', evt.inputSource);
    };
    device.addEventListener('@@webxr-polyfill/input-select-start', this[PRIVATE].onSelectStart);

    this[PRIVATE].onSelectEnd = evt => {
      // Ignore if this event is not for this session.
      if (evt.sessionId !== this[PRIVATE].id) {
        return;
      }

      // Sadly, there's no way to make this a user gesture.
      this[PRIVATE].dispatchInputSourceEvent('select', evt.inputSource);
      this[PRIVATE].dispatchInputSourceEvent('selectend', evt.inputSource);
    };
    device.addEventListener('@@webxr-polyfill/input-select-end', this[PRIVATE].onSelectEnd);

    this[PRIVATE].onSqueezeStart = evt => {
      // Ignore if this event is not for this session.
      if (evt.sessionId !== this[PRIVATE].id) {
        return;
      }

      this[PRIVATE].dispatchInputSourceEvent('squeezestart', evt.inputSource);
    };
    device.addEventListener('@@webxr-polyfill/input-squeeze-start', this[PRIVATE].onSqueezeStart);

    this[PRIVATE].onSqueezeEnd = evt => {
      // Ignore if this event is not for this session.
      if (evt.sessionId !== this[PRIVATE].id) {
        return;
      }

      this[PRIVATE].dispatchInputSourceEvent('squeezeend', evt.inputSource);

      // Following the same way as select event
      this[PRIVATE].dispatchInputSourceEvent('squeeze', evt.inputSource);
    };
    device.addEventListener('@@webxr-polyfill/input-squeeze-end', this[PRIVATE].onSqueezeEnd);

    this[PRIVATE].dispatchInputSourceEvent = (type, inputSource) => {
      // TODO
      // const frame = new XRFrame(device, this, this[PRIVATE].id);
      // const event = new XRInputSourceEvent(type, { frame, inputSource });
      // frame[XRFRAME_PRIVATE].active = true;
      // this.dispatchEvent(type, event);
      // frame[XRFRAME_PRIVATE].active = false;
    };

    // Start the frame loop
    this[PRIVATE].startDeviceFrameLoop();

    // TODO
    // this.onblur = undefined;
    // this.onfocus = undefined;
    // this.onresetpose = undefined;
    // this.onend = undefined;
    // this.onselect = undefined;
    // this.onselectstart = undefined;
    // this.onselectend = undefined;
  }
  updateTargetFrameRate(_rate: number): Promise<void> {
    throw new Error('Method not implemented.');
  }

  requestHitTestSource?: (options: XRHitTestOptionsInit) => Promise<XRHitTestSource>;
  requestHitTestSourceForTransientInput?: (options: XRTransientInputHitTestOptionsInit) => Promise<XRTransientInputHitTestSource>;
  requestHitTest?: (ray: XRRay, referenceSpace: XRReferenceSpace) => Promise<XRHitResult[]>;
  updateWorldTrackingState?: (options: { planeDetectionState?: { enabled: boolean; }; }) => void;
  initiateRoomCapture?(): Promise<void> {
    throw new Error('Method not implemented.');
  }
  requestLightProbe(options?: XRLightProbeInit): Promise<XRLightProbe> {
    throw new Error('Method not implemented.');
  }
  getTrackedImageScores?(): Promise<XRImageTrackingScore[]> {
    throw new Error('Method not implemented.');
  }
  trySetFeaturePointCloudEnabled(enabled: boolean): boolean {
    throw new Error('Method not implemented.');
  }
  trySetPreferredPlaneDetectorOptions(preferredOptions: XRGeometryDetectorOptions): boolean {
    throw new Error('Method not implemented.');
  }
  trySetMeshDetectorEnabled(enabled: boolean): boolean {
    throw new Error('Method not implemented.');
  }
  trySetPreferredMeshDetectorOptions(preferredOptions: XRGeometryDetectorOptions): boolean {
    throw new Error('Method not implemented.');
  }

  get ended(): boolean {
    return this[PRIVATE].ended;
  }

  get renderState(): XRRenderState {
    return this[PRIVATE].activeRenderState;
  }

  get environmentBlendMode() {
    return this[PRIVATE].device.environmentBlendMode || 'opaque';
  }

  /**
   * It queues the next frame to be called on the next animation frame, and returns true if it was queued, false if it was
   * not (because the session is ended or suspended).
   * @returns a boolean indicating whether the frame was queued
   */
  private queueNextFrame(): boolean {
    if (this[PRIVATE].ended || this[PRIVATE].suspended) {
      return false;
    }
    this[PRIVATE].deviceFrameHandle = null;
    this[PRIVATE].startDeviceFrameLoop();
    return true;
  }

  /**
   * @param {string} type
   * @return {XRReferenceSpace}
   */
  async requestReferenceSpace(type: XRReferenceSpaceType): Promise<XRReferenceSpace> {
    if (this[PRIVATE].ended) {
      return;
    }

    if (!XRReferenceSpaceTypes.includes(type)) {
      throw new TypeError(`XRReferenceSpaceType must be one of ${XRReferenceSpaceTypes}`);
    }

    if (!this[PRIVATE].device.doesSessionSupportReferenceSpace(this[PRIVATE].id, type)) {
      throw new DOMException(`The ${type} reference space is not supported by this session.`, 'NotSupportedError');
    }

    if (type === 'viewer') {
      return this[PRIVATE].viewerSpace;
    }
    /**
     * In JSAR's WebXR implementation, we treat the "local" space as the dynamic space just like the "viewer" space, and
     * the "local" space is updated in every device frame, so we return a new XRReferenceSpace with the "local" type.
     * 
     * Note that this differs from the browser's behavior, the browser's "local" space is static. The reason why JSAR's
     * "local" space is dynamic is that JSAR's implementation is to allow the objects is able to move from the host such
     * as the Unity.
     */
    if (type === 'local') {
      return this[PRIVATE].localSpace;
    }

    // Request a transform from the device given the values. If returning a
    // transform (probably "local-floor" or "bounded-floor"), use it, and if
    // undefined, XRReferenceSpace will use a default transform. This call can
    // throw, rejecting the promise, indicating the device does not support that
    // frame of reference.
    let transform = await this[PRIVATE].device.requestFrameOfReferenceTransform(this[PRIVATE].id, type);

    // TODO: 'bounded-floor' is only blocked because we currently don't report
    // the bounds geometry correctly.
    if (type === 'bounded-floor') {
      if (!transform) {
        // 'bounded-floor' spaces must have a transform supplied by the device.
        throw new DOMException(`${type} XRReferenceSpace not supported by this device.`, 'NotSupportedError');
      }

      let bounds = this[PRIVATE].device.requestStageBounds();
      if (!bounds) {
        // 'bounded-floor' spaces must have bounds geometry.
        throw new DOMException(`${type} XRReferenceSpace not supported by this device.`, 'NotSupportedError');
      }
      // TODO: Create an XRBoundedReferenceSpace with the correct boundaries.
      throw new DOMException(`The WebXR polyfill does not support the ${type} reference space yet.`, 'NotSupportedError');
    }
    return new XRReferenceSpaceImpl(type, transform);
  }

  /**
   * @param {Function} callback
   * @return {number}
   */
  requestAnimationFrame(callback: (time: number, frame: XRFrame) => void) {
    if (this[PRIVATE].ended) {
      logger.warn(`Can't call requestAnimationFrame on an XRSession that has already ended.`);
      return;
    }

    // Add callback to the queue and return its handle
    const handle = ++this[PRIVATE].frameHandle;
    this[PRIVATE].frameCallbacks.push({
      handle,
      callback,
      cancelled: false
    });
    return handle;
  }

  cancelAnimationFrame(handle: number) {
    // Remove the callback with that handle from the queue
    let callbacks = this[PRIVATE].frameCallbacks;
    let index = callbacks.findIndex(d => d && d.handle === handle);
    if (index > -1) {
      callbacks[index].cancelled = true;
      callbacks.splice(index, 1);
    }
    // If cancelAnimationFrame is called from within a frame callback, also check
    // the remaining callbacks for the current frame:
    callbacks = this[PRIVATE].currentFrameCallbacks;
    if (callbacks) {
      index = callbacks.findIndex(d => d && d.handle === handle);
      if (index > -1) {
        callbacks[index].cancelled = true;
        // Rely on cancelled flag only; don't mutate this array while it's being iterated
      }
    }
  }

  /**
   * @return {Array<XRInputSource>} input sources
   */
  get inputSources() {
    return this[PRIVATE].device.getInputSources();
  }

  /**
   * @return {Promise<void>}
   */
  async end() {
    if (this[PRIVATE].ended) {
      return;
    }

    // If this is an immersive session, trigger the platform to end, which
    // will call the `onPresentationEnd` handler, wrapping this up.
    if (this[PRIVATE].immersive) {
      this[PRIVATE].ended = true;
      this[PRIVATE].device.removeEventListener('@@webxr-polyfill/vr-present-start',
        this[PRIVATE].onPresentationStart);
      this[PRIVATE].device.removeEventListener('@@webxr-polyfill/vr-present-end',
        this[PRIVATE].onPresentationEnd);
      this[PRIVATE].device.removeEventListener('@@webxr-polyfill/input-select-start',
        this[PRIVATE].onSelectStart);
      this[PRIVATE].device.removeEventListener('@@webxr-polyfill/input-select-end',
        this[PRIVATE].onSelectEnd);

      this.dispatchEvent(new XRSessionEvent('end', { session: this }));
    }

    this[PRIVATE].stopDeviceFrameLoop();
    return this[PRIVATE].device.endSession(this[PRIVATE].id);
  }

  /**
   * Queues an update to the active render state to be applied on the next
   * frame. Unset fields of newState will not be changed.
   */
  async updateRenderState(newState: XRRenderStateInit): Promise<void> {
    if (this[PRIVATE].ended) {
      const message = "Can't call updateRenderState on an XRSession " +
        "that has already ended.";
      throw new Error(message);
    }

    const baseLayerImpl = <XRWebGLLayerImpl>newState.baseLayer;
    if (baseLayerImpl && (baseLayerImpl._session !== this)) {
      const message = "Called updateRenderState with a base layer that was " +
        "created by a different session.";
      throw new Error(message);
    }

    const fovSet = (newState.inlineVerticalFieldOfView !== null) &&
      (newState.inlineVerticalFieldOfView !== undefined);

    if (fovSet) {
      if (this[PRIVATE].immersive) {
        const message = "inlineVerticalFieldOfView must not be set for an " +
          "XRRenderState passed to updateRenderState for an " +
          "immersive session.";
        throw new Error(message);
      } else {
        // Clamp the inline FoV to a sane range.
        newState.inlineVerticalFieldOfView = Math.min(
          3.13, Math.max(0.01, newState.inlineVerticalFieldOfView));
      }
    }

    if (this[PRIVATE].pendingRenderState === null) {
      const activeRenderState = this[PRIVATE].activeRenderState;
      this[PRIVATE].pendingRenderState = {
        depthNear: activeRenderState.depthNear,
        depthFar: activeRenderState.depthFar,
        inlineVerticalFieldOfView: activeRenderState.inlineVerticalFieldOfView,
        baseLayer: activeRenderState.baseLayer
      };
    }
    Object.assign(this[PRIVATE].pendingRenderState, newState);
  }

  /**
   * Compares the inputSources with the ones in the previous frame.
   * Fires imputsourceschange event if any added or removed
   * inputSource is found.
   */
  _checkInputSourcesChange() {
    const added = [];
    const removed = [];
    const newInputSources = this.inputSources;
    const oldInputSources = this[PRIVATE].currentInputSources;

    // for (const newInputSource of newInputSources) {
    //   if (!oldInputSources.includes(newInputSource)) {
    //     added.push(newInputSource);
    //   }
    // }

    // for (const oldInputSource of oldInputSources) {
    //   if (!newInputSources.includes(oldInputSource)) {
    //     removed.push(oldInputSource);
    //   }
    // }

    if (added.length > 0 || removed.length > 0) {
      // TODO
      // this.dispatchEvent('inputsourceschange', new XRInputSourcesChangeEvent('inputsourceschange', {
      //   session: this,
      //   added: added,
      //   removed: removed
      // }));
    }

    this[PRIVATE].currentInputSources.length = 0;
    // for (const newInputSource of newInputSources) {
    //   this[PRIVATE].currentInputSources.push(newInputSource);
    // }
  }
}
