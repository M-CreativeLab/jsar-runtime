import { createXRSession } from './XRSession';

export default class XRSystemImpl extends EventTarget implements XRSystem {
  ondevicechange: XRSystemDeviceChangeEventHandler;
  onsessiongranted: XRSystemSessionGrantedEventHandler;

  async requestSession(_mode: XRSessionMode, options?: XRSessionInit): Promise<XRSession> {
    return createXRSession(options);
  }

  isSessionSupported(mode: XRSessionMode): Promise<boolean> {
    if (mode === 'immersive-ar') {
      return Promise.resolve(true);
    } else {
      return Promise.resolve(false);
    }
  }
}
