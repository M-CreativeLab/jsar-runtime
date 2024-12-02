/**
 * Transmute: The JSAR private namespace.
 * 
 * This namespace contains the private global types, classes, and interfaces for the Transmute JSAR internal implementation.
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
   */
  class TrClientContext {
    /**
     * The client context id.
     */
    id: number;
    /**
     * The client context URL.
     */
    url: string;
    applicationCacheDirectory: string;
    httpsProxyServer: string;
    /**
     * The webgl version, 1 for WebGL1, 2 for WebGL2.
     */
    webglVersion: number;
    /**
     * The webgl context to draw to the host scene.
     */
    gl: WebGLRenderingContext | WebGL2RenderingContext;
    /**
     * The WebXR device configuration.
     */
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
      ViewProjectionMatrixForRightEye: WebGLPlaceholderId;
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
   * The resource response types.
   * 
   * - string: The response is a string.
   * - arraybuffer: The response is an ArrayBuffer.
   * - json: The response is a JSON object.
   */
  type ResourceResponseTypesMap = {
    string: string;
    arraybuffer: ArrayBuffer;
    json: object;
  };

  /**
   * The resource fetching options.
   */
  type ResourceFetchingOptions = {
    /** The accept header. */
    accept?: string;
    /** The cookie jar. */
    cookieJar?: any;
    /** The referrer. */
    referrer?: string;
  };

  /**
   * The resource loader interface.
   */
  interface ResourceLoader {
    /**
     * Fetch a resource from the network or file system.
     * 
     * @param url 
     * @param options 
     * @param returnsAs
     * @returns The corresponding response type.
     */
    fetch<AsType extends keyof ResourceResponseTypesMap>(url: string, options: ResourceFetchingOptions, returnsAs?: AsType): Promise<ResourceResponseTypesMap[AsType]>;

    /**
     * Create a WHATWG fetch implementation.
     * 
     * @param baseURI The base URI to be used with relative URLs and CORS checks.
     * @returns the fetch(input, init?) function.
     */
    createWHATWGFetchImpl(baseURI: string): (input: RequestInfo, init?: RequestInit) => Promise<Response>;
  }

  /**
   * What's a WebRuntimeContext?
   * 
   * The WebRuntimeContext is a context for the web runtime, such as the main document rendering, worker execution, audio worklet, etc.
   */
  class RuntimeContext {
    /**
     * Configure the resource loader for the context.
     * @param loader the resource loader.
     */
    setResourceLoader(loader: ResourceLoader): void;
  }

  /**
   * A browsing context is an environment in which an agent displays a `Document`. The HTML or depreated XSML document depends on this context to bootstrap.
   */
  class BrowsingContext extends RuntimeContext {
    /**
     * Create the browsing context.
     */
    constructor();
    /**
     * Start a new document rendering from the source and MIME type.
     * 
     * @param source the document source.
     * @param mimeType the MIME type: 'text/html', 'text/xsml', 'text/svg+xml', etc.
     */
    start(source: string, mimeType: 'text/html'): Document;
  }

  /**
   * The `RuntimeContext` for the worker execution.
   */
  class WorkerContext extends RuntimeContext {
    /**
     * Create a new v8 context for scripting.
     * @param baseURI The base URI for the worker.
     * @param options The worker options.
     */
    constructor(baseURI: string, options?: WorkerOptions);
    /**
     * Start the worker execution from the URL, internally it will fetch the script and execute it.
     * 
     * @param url The worker script URL or relative path to the base URI.
     */
    start(url: string): void;
    /**
     * Start the worker execution from the source code.
     * 
     * @param source The worker script source code.
     */
    startFromSource(source: string): void;
    /**
     * Dispatch an event to the worker context.
     * 
     * @param event the event object.
     */
    dispatchEvent(event: Event): void;
  }
}
