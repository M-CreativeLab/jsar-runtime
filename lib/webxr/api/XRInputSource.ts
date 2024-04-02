/*
 * Copyright 2018 Google Inc. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import XRSpace from './XRSpace';

export const PRIVATE = Symbol('@@webxr-polyfill/XRInputSource');

type XRInputSourceInternal = {
  handedness: XRHandedness;
  targetRayMode: XRTargetRayMode;
  profiles: string[];
  gamepad: Gamepad;
};

export default class XRInputSourceImpl implements XRInputSource {
  [PRIVATE]: {
    internal: XRInputSourceInternal;
    gripSpace: XRSpace;
    targetRaySpace: XRSpace;
  }

  constructor(internal: XRInputSourceInternal) {
    this[PRIVATE] = {
      internal,
      gripSpace: new XRSpace('grip', this),
      targetRaySpace: new XRSpace('target-ray', this)
    };
  }

  get handedness() { return this[PRIVATE].internal.handedness; }
  get targetRayMode() { return this[PRIVATE].internal.targetRayMode; }
  get targetRaySpace() { return this[PRIVATE].targetRaySpace; }
  get profiles() { return this[PRIVATE].internal.profiles; }
  get gamepad() { return this[PRIVATE].internal.gamepad; }
  get gripSpace() {
    let mode = this[PRIVATE].internal.targetRayMode;
    if (mode === 'gaze' || mode === 'screen') {
      // grip space must be null for non-trackable input sources
      return null;
    }
    return this[PRIVATE].gripSpace;
  }
}
