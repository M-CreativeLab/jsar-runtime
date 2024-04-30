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

import { XRDevice, XRNativeDevice } from '../devices';
// import XRSessionImpl from './XRSession';

export const PRIVATE = Symbol('@@webxr-polyfill/XR');
export const XRSessionModes = ['inline', 'immersive-vr', 'immersive-ar'];

const { XRSession: XRSessionBinding } = process._linkedBinding('transmute:webxr');

const DEFAULT_SESSION_OPTIONS = {
  'inline': {
    requiredFeatures: ['viewer'],
    optionalFeatures: [],
  },
  'immersive-vr': {
    requiredFeatures: ['viewer', 'local'],
    optionalFeatures: [],
  },
  'immersive-ar': {
    requiredFeatures: ['viewer', 'local'],
    optionalFeatures: [],
  }
};

const POLYFILL_REQUEST_SESSION_ERROR =
  `Polyfill Error: Must call navigator.xr.isSessionSupported() with any XRSessionMode
or navigator.xr.requestSession('inline') prior to requesting an immersive
session. This is a limitation specific to the WebXR Polyfill and does not apply
to native implementations of the API.`

class XRSessionWrapper extends XRSessionBinding {
  #eventTarget: EventTarget;

  onend: XRSessionEventHandler = null;
  oninputsourceschange: XRInputSourceChangeEventHandler = null;
  onselect: XRInputSourceEventHandler = null;
  onselectstart: XRInputSourceEventHandler = null;
  onselectend: XRInputSourceEventHandler = null;
  onsqueeze: XRInputSourceEventHandler = null;
  onsqueezestart: XRInputSourceEventHandler = null;
  onsqueezeend: XRInputSourceEventHandler = null;
  onvisibilitychange: XRSessionEventHandler = null;
  onframeratechange: XRSessionEventHandler = null;

  constructor(device: XRDevice, mode: XRSessionMode, sessionId: number) {
    super((<XRNativeDevice>device).handle, mode, sessionId, (type: string, eventProps: any) => {
      const event = new Event(type);
      if (eventProps) {
        Object.assign(event, eventProps);
      }
      this.#eventTarget.dispatchEvent(event);

      // call the event handler such as `onend`, `onselect`, etc.
      const handler = this[`on${event.type}`];
      if (typeof handler === 'function') {
        handler(event);
      }
    });
    this.#eventTarget = new EventTarget();
  }

  addEventListener(type: string, listener: EventListener | EventListenerObject | null, options?: boolean | AddEventListenerOptions): void {
    this.#eventTarget.addEventListener(type, listener, options);
  }

  removeEventListener(type: string, listener: EventListener | EventListenerObject | null, options?: boolean | EventListenerOptions): void {
    this.#eventTarget.removeEventListener(type, listener, options);
  }

  dispatchEvent(event: Event): boolean {
    return this.#eventTarget.dispatchEvent(event);
  }
}

export default class XRSystemImpl extends EventTarget implements XRSystem {
  ondevicechange: XRSystemDeviceChangeEventHandler;
  onsessiongranted: XRSystemSessionGrantedEventHandler;

  #device: XRDevice;
  #bondSessionId: number;
  #immersiveSession: XRSessionWrapper | null;
  #inlineSessions: Set<XRSessionWrapper>;

  constructor(device: XRDevice, sessionId: number) {
    super();
    this.#device = device;
    this.#bondSessionId = sessionId;
    this.#immersiveSession = null;
    this.#inlineSessions = new Set();
  }

  /**
   * @param {XRSessionMode} mode
   * @return {Promise<boolean>}
   */
  async isSessionSupported(mode: XRSessionMode): Promise<boolean> {
    // 'inline' is always guaranteed to be supported.
    if (mode !== 'inline') {
      return Promise.resolve(this.#device.isSessionSupported(mode));
    }
    return Promise.resolve(true);
  }

  /**
   * @param {XRSessionMode} mode
   * @param {XRSessionInit} options
   * @return {Promise<XRSession>}
   */
  async requestSession(mode: XRSessionMode, options?: XRSessionInit): Promise<XRSession> {
    // If the device hasn't resolved yet, wait for it and try again.
    if (!this.#device) {
      if (mode !== 'inline') {
        // Because requesting immersive modes requires a user gesture, we can't
        // wait for a promise to resolve before making the real session request.
        // For that reason, we'll throw a polyfill-specific error here.
        throw new Error(POLYFILL_REQUEST_SESSION_ERROR);
      }
    }

    if (!XRSessionModes.includes(mode)) {
      throw new TypeError(
        `The provided value '${mode}' is not a valid enum value of type XRSessionMode`);
    }

    // Resolve which of the requested features are supported and reject if a
    // required feature isn't available.
    const defaultOptions = DEFAULT_SESSION_OPTIONS[mode];
    const requiredFeatures = defaultOptions.requiredFeatures.concat(
      options && options.requiredFeatures ? options.requiredFeatures : []);
    const optionalFeatures = defaultOptions.optionalFeatures.concat(
      options && options.optionalFeatures ? options.optionalFeatures : []);
    const enabledFeatures = new Set<string>();

    let requirementsFailed = false;
    for (let feature of requiredFeatures) {
      if (!this.#device.isFeatureSupported(feature)) {
        console.error(`The required feature '${feature}' is not supported`);
        requirementsFailed = true;
      } else {
        enabledFeatures.add(feature);
      }
    }

    if (requirementsFailed) {
      throw new DOMException('Session does not support some required features', 'NotSupportedError');
    }

    for (let feature of optionalFeatures) {
      if (!this.#device.isFeatureSupported(feature)) {
        console.log(`The optional feature '${feature}' is not supported`);
      } else {
        enabledFeatures.add(feature);
      }
    }

    // Call device's requestSession, which does some initialization (1.1 
    // fallback calls `vrDisplay.requestPresent()` for example). Could throw 
    // due to missing user gesture.
    const sessionId = await this.#device.requestSession(mode, enabledFeatures, this.#bondSessionId);
    const session = new XRSessionWrapper(this.#device, mode, sessionId);

    if (mode == 'inline') {
      this.#inlineSessions.add(session);
    } else {
      this.#immersiveSession = session;
    }

    const onSessionEnd = () => {
      if (mode == 'inline') {
        this.#inlineSessions.delete(session);
      } else {
        this.#immersiveSession = null;
      }
      session.removeEventListener('end', onSessionEnd);
    };
    session.addEventListener('end', onSessionEnd);
    return session as any as XRSession;
  }
}
