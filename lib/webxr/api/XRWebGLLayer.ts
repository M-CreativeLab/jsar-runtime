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

import type XRView from './XRView';
import XRSession, { PRIVATE as SESSION_PRIVATE } from './XRSession';

export const PRIVATE = Symbol('@@webxr-polyfill/XRWebGLLayer');

export default class XRWebGLLayer {
  [PRIVATE]: {
    context: WebGLRenderingContext | WebGL2RenderingContext,
    session: XRSession,
    config: XRWebGLLayerInit,
  };

  constructor(session: XRSession, context: WebGLRenderingContext | WebGL2RenderingContext, init?: XRWebGLLayerInit) {
    const config = Object.assign({
      antialias: true,
      depth: true,
      stencil: false,
      alpha: true,
      multiview: false,
      ignoreDepthValues: false,
      framebufferScaleFactor: 1.0,
    }, init || {});

    if (!(session instanceof XRSession)) {
      throw new Error('session must be a XRSession');
    }

    if (session.ended) {
      throw new Error(`InvalidStateError`);
    }

    this[PRIVATE] = {
      context,
      config,
      session,
    };
  }

  get context() {
    return this[PRIVATE].context;
  }

  get antialias() {
    return this[PRIVATE].config.antialias;
  }

  /**
   * The polyfill will always ignore depth values.
   */
  get ignoreDepthValues() {
    return true;
  }

  get framebuffer() {
    // Use the default framebuffer
    return null;
  }

  /**
   * @return {number}
   */
  get framebufferWidth() {
    return this[PRIVATE].context.drawingBufferWidth;
  }

  /**
   * @return {number}
   */
  get framebufferHeight() {
    return this[PRIVATE].context.drawingBufferHeight;
  }

  /**
   * @return {XRSession}
   */
  get _session() {
    return this[PRIVATE].session;
  }

  /**
   * @TODO No mention in spec on not reusing the XRViewport on every frame.
   * 
   * @TODO In the future maybe all this logic should be handled here instead of
   * delegated to the XRView?
   */
  getViewport(view: XRView) {
    return view._getViewport(this);
  }

  /**
   * Gets the scale factor to be requested if you want to match the device
   * resolution at the center of the user's vision. The polyfill will always
   * report 1.0.
   */
  static getNativeFramebufferScaleFactor(session: XRSession) {
    if (!session) {
      throw new TypeError('getNativeFramebufferScaleFactor must be passed a session.')
    }
    if (session[SESSION_PRIVATE].ended) {
      return 0.0;
    } else {
      return 1.0;
    }
  }
}
