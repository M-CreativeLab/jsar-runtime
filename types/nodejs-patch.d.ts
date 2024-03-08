declare namespace Transmute {
  class TransmuteEnvironment {
    constructor();
    getRuntimeInit(): string;
    markRuntimeAvailable(versions: string): void;
  }

  class XRDeviceNative {
    isSessionSupported(mode: string): boolean;
    requestSession(sessionId: number): boolean;
    requestFrameOfReferenceTransform(sessionId: number, type: XRReferenceSpaceType, options?): Float32Array;
    getViewerTransform(): Float32Array;
    /**
     * Get the view matrix for the specified eye.
     * @param eyeId 0 for left eye, 1 for right eye
     */
    getViewerStereoViewMatrix(eyeId: number): Float32Array;
    /**
     * Get the projection matrix for the specified eye.
     * @param eyeId 0 for left eye, 1 for right eye
     */
    getViewerStereoProjectionMatrix(eyeId: number): Float32Array;
  }

  type NativeEventListener = (id: number, type: number, data: string) => void;
  /**
   * The native event target is a class to handle the native event from the native side:
   * 
   * - Sending the event to the native side such as Unity, UE4, etc.
   * - Receiving the event from the native side.
   */
  class NativeEventTarget {
    constructor();
    setNativeEventListener(listener: NativeEventListener): void;
    dispatchEvent(id: number, type: number, data: string): void;
    dispose(): void;
  }
}

declare namespace NodeJS {
  interface Process {
    _linkedBinding(module: 'transmute:logger'): {
      /**
       * Write the message to the log stream.
       * @param level the level could be 0, 1, 2. the 1 means error, 2 means warn, 0 means info.
       * @param message a string message
       * @returns nothing
       */
      log: (level: number, message: string) => void;
    };
    _linkedBinding(module: 'transmute:env'): {
      Environment: typeof Transmute.TransmuteEnvironment;
    };
    _linkedBinding(module: 'transmute:messaging'): {
      NativeEventTarget: typeof Transmute.NativeEventTarget;
    };
    _linkedBinding(module: 'transmute:webgl'): {
      WebGLRenderingContext: typeof WebGLRenderingContext;
    };
    _linkedBinding(module: 'transmute:webxr'): {
      XRDeviceNative: typeof Transmute.XRDeviceNative;
    };
    _linkedBinding(module: string): any;
  }
}
