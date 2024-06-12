declare namespace Transmute {
  type FrameRequestCallback = (time: number, data: unknown) => void;
  type NativeSessionContextItem = {
    sessionId: number;
    localTransform: Float32Array;
  };
  type NativeFrameContext = {
    type: string;
    activeEyeId?: number;
    stereoId: number;
    viewerViewMatrix?: Float32Array;
    viewerProjectionMatrix?: Float32Array;
    viewerTransform: Float32Array;
    sessions: NativeSessionContextItem[];
  };

  class TransmuteEnvironment {
    constructor();
    getRuntimeInit(): string;
    markRuntimeAvailable(versions: string): void;
  }

  class TrClientContext {
    id: number;
    url: string;
    applicationCacheDirectory: string;
    httpsProxyServer: string;
    webglVersion: number;
    xrDevice: {
      enabled: boolean;
      active: boolean;
      stereoRenderingMode: 'multipass' | 'singlepass' | 'singlepass-instanced' | 'singlepass-multiview' | 'unknown';
    };
  }

  type RenderExceptionCallback = (code: number) => void;
  /**
   * The render loop is the backend implementation for `requestAnimationFrame` and `cancelAnimationFrame`.
   */
  class RenderLoop {
    constructor();
    supportsWebGL2(): boolean;
    setExceptionCallback(callback: RenderExceptionCallback): void;
    setFrameCallback(callback: FrameRequestCallback): void;
  }
  class AnimationFrameListener {
    constructor();
    connect(callback: FrameRequestCallback): number;
    isConnected(): boolean;
  }

  class WebGLRenderingContextOnDevice extends WebGLRenderingContext {
    constructor(contextAttribs?: WebGLContextAttributes);
  }
  class WebGL2RenderingContextOnDevice extends WebGL2RenderingContext {
    constructor(contextAttribs?: WebGLContextAttributes);
  }

  type XRNativeInputSource = {
    id: number;
    handness: string;
    targetRayMode: string;
  };
  class XRDeviceNative {
    isSessionSupported(mode: string): Promise<boolean>;
    requestSession(mode: string): Promise<number>;
    requestFrameOfReferenceTransform(sessionId: number, type: XRReferenceSpaceType, options?): Float32Array;
    getViewport(sessionId: number, eye: XREye, viewIndex: number): {
      x: number;
      y: number;
      width: number;
      height: number;
    };
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
    getActiveEyeId(): number;

    // Input source
    getGazeInputSource(): XRNativeInputSource;
    getHandInputSource(handness: string): XRNativeInputSource;
    getGamepadInputSource(): XRNativeInputSource;
    getScreenInputSource(): XRNativeInputSource;

    // frame
    startFrame(sessionId: number, stereoRenderingId: number, passIndex: number): void;
    endFrame(sessionId: number, stereoRenderingId: number, passIndex: number): void;
  }

  type NativeEventListener = (id: number, type: number, data: string) => void;
  /**
   * The native event target is a class to handle the native event from the native side:
   * 
   * - Sending the event to the native side such as Unity, UE4, etc.
   * - Receiving the event from the native side.
   */
  class NativeEventTarget {
    constructor(listener: NativeEventListener);
    dispatchEvent(eventInit: { type: number, detail?: string }): number;
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
      ClientContext: typeof Transmute.TrClientContext;
    };
    _linkedBinding(module: 'transmute:messaging'): {
      NativeEventTarget: typeof Transmute.NativeEventTarget;
    };
    _linkedBinding(module: 'transmute:renderer'): {
      RenderLoop: typeof Transmute.RenderLoop;
      AnimationFrameListener: typeof Transmute.AnimationFrameListener;
    };
    _linkedBinding(module: 'transmute:canvas'): {
      ImageData: typeof ImageData;
      ImageBitmap: typeof ImageBitmap;
      createImageBitmap: (image: ImageData | ImageBitmap) => Promise<ImageBitmap>;
      OffscreenCanvas: typeof OffscreenCanvas;
    };
    _linkedBinding(module: 'transmute:webgl'): {
      WebGLRenderingContext: typeof Transmute.WebGLRenderingContextOnDevice;
      WebGL2RenderingContext: typeof Transmute.WebGL2RenderingContextOnDevice;
    };
    _linkedBinding(module: 'transmute:webxr'): {
      XRDeviceNative: typeof Transmute.XRDeviceNative;
      XRSession: any;
      XRFrame: typeof XRFrame;
      XRWebGLLayer: typeof XRWebGLLayer;
      XRRigidTransform: typeof XRRigidTransform;
    };
    _linkedBinding(module: string): any;
  }
}
