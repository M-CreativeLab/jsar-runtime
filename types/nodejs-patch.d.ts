/**
 * Add patch for the OffscreenCanvas to support the HTMLRenderingContext.
 */
interface OffscreenCanvas {
  getContext(contextId: 'jsar:htmlrenderer'): Transmute.HTMLRenderingContext;
}

/**
 * New APIs for WebXR Device API.
 */
interface XRSession {
  /**
   * Update the session's collision box, which is used for the collision detection with native ray.
   */
  updateCollisionBox(min: number[], max: number[]): void;
}

interface XRInputSource {
  /**
   * Set the target ray's hit test result, this is used for updating the host ray rendering.
   *
   * @param hit If the target ray hits an object.
   * @param endTransform The ray's end transform if hit or null.
   * @returns null
   */
  setTargetRayHitTestResult(hit: boolean, endTransform?: XRRigidTransform | null): void;
}

/**
 * Transmute: The JSAR internal APIs.
 */
declare namespace Transmute {
  /**
   * Frame request callback.
   * 
   * @param time The current time in milliseconds.
   */
  type FrameRequestCallback = (time: number) => void;

  /**
   * Represents the session context object, basically including the session id and its local transform.
   */
  type NativeSessionContextItem = {
    /**
     * The session id.
     */
    sessionId: number;
    /**
     * The session's local transform.
     */
    localTransform: Float32Array;
  };

  /**
   * The native frame context object.
   */
  type NativeFrameContext = {
    type: string;
    activeEyeId?: number;
    stereoId: number;
    viewerViewMatrix?: Float32Array;
    viewerProjectionMatrix?: Float32Array;
    viewerTransform: Float32Array;
    sessions: NativeSessionContextItem[];
  };

  /**
   * The client context which is returned via the "transmute:env" module.
   * 
   * This object contains the following properties:
   * - id: The client context id.
   * - url: The current URL.
   * - applicationCacheDirectory: The application cache directory.
   * - httpsProxyServer: The HTTPS proxy server.
   * - webglVersion: The WebGL version.
   * - xrDevice: The XR device information.
   */
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
    /**
     * Send a keep alive message.
     */
    keepAlive(): void;
    /**
     * Returns the current time in milliseconds in fast mode.
     */
    fastPerformanceNow(): number;
  }

  /**
   * The render exception callback.
   */
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
  type WebGLPlaceholderId = number;
  type WebGLNativePlaceholders = {
    useRightHandedKey: string;
    inverseMatrixKey: string;
    placeholderIdKey: string;
    placeholderIds: {
      ProjectionMatrix: WebGLPlaceholderId;
      ViewMatrix: WebGLPlaceholderId;
      ViewProjectionMatrix: WebGLPlaceholderId;
      [key: string]: WebGLPlaceholderId;
    };
  };

  class HTMLRenderingContext {
    setHTML(html: string): void;
    dispatchEvent(type: 'mousemove', event: MouseEvent): void;
    dispatchEvent(type: 'mousedown', event: MouseEvent): void;
    dispatchEvent(type: 'mouseup', event: MouseEvent): void;
  }

  type XRNativeSession = {
    id: number;
    recommendedContentSize: number;
  };
  type XRNativeInputSource = {
    id: number;
    handness: string;
    targetRayMode: string;
  };

  class XRDeviceNative {
    isSessionSupported(mode: string): Promise<boolean>;
    requestSession(mode: string): Promise<XRNativeSession>;
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
  }

  type NativeEventListener = (
    eventId: number,    // The id of the incoming event
    eventType: number,  // The type of the incoming event
    peerId: number,     // The id of the peer event, such as a RpcResponse has a peer RpcRequest.
    detailJson: string  // The detail of the incoming event in JSON format
  ) => void;
  /**
   * The native event target is a class to handle the native event from the native side:
   * 
   * - Sending the event to the native side such as Unity, UE4, etc.
   * - Receiving the event from the native side.
   */
  class NativeEventTarget {
    static readonly EventTypes: {
      [key: string]: number;
    };
    static readonly DocumentEventTypes: {
      [key: string]: number;
    };
    constructor(listener: NativeEventListener);
    dispatchEvent(eventInit: { type: number, detail?: string }): number;
    dispose(): void;
  }

  /**
   * The internal rendering context for the document rendering.
   */
  class DocumentRenderingContext {
    constructor();
    /**
     * Start a new document rendering from the source and MIME type.
     * 
     * @param source the document source.
     * @param mimeType the MIME type: 'text/html', 'text/xsml', 'text/svg+xml', etc.
     */
    start(source: string, mimeType: 'text/html'): Document;
  }
}

/**
 * Custom the Node.js `process._linkedBinding` method to support the JSAR's native modules.
 */
declare namespace NodeJS {
  interface Process {
    _linkedBinding(module: 'transmute:dom'): {
      Audio: typeof Audio;
      DocumentRenderingContext: typeof Transmute.DocumentRenderingContext;
    };
    _linkedBinding(module: 'transmute:env'): {
      ClientContext: typeof Transmute.TrClientContext;
    };
    _linkedBinding(module: 'transmute:messaging'): {
      NativeEventTarget: typeof Transmute.NativeEventTarget;
    };
    _linkedBinding(module: 'transmute:math3d'): {
      matrixMultiplyToArray: (a: number[], b: number[], out: number[], offset: number) => void;
    };
    _linkedBinding(module: 'transmute:renderer'): {
      RenderLoop: typeof Transmute.RenderLoop;
      AnimationFrameListener: typeof Transmute.AnimationFrameListener;
    };
    _linkedBinding(module: 'transmute:canvas'): {
      ImageData: typeof ImageData;
      ImageBitmap: typeof ImageBitmap;
      OffscreenCanvas: typeof OffscreenCanvas;
      createImageBitmap: (image: ImageData | ImageBitmap) => Promise<ImageBitmap>;
    };
    _linkedBinding(module: 'transmute:webgl'): {
      WebGLRenderingContext: typeof Transmute.WebGLRenderingContextOnDevice;
      WebGL2RenderingContext: typeof Transmute.WebGL2RenderingContextOnDevice;
      placeholders: Transmute.WebGLNativePlaceholders;
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
