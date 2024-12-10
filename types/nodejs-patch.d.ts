/// <reference path="transmute-private.d.ts" />

// Custom the Node.js `process._linkedBinding` method to support the JSAR's native modules.
declare namespace NodeJS {
  interface Process {
    /**
     * It loads the transmute's native browser module.
     */
    _linkedBinding(module: 'transmute:browser'): {
      // The browser APIs
    };
    /**
     * It loads the transmute's native DOM module, and returns some internal classes for host.
     */
    _linkedBinding(module: 'transmute:dom'): {
      Audio: typeof Audio;
      BrowsingContext: typeof Transmute.BrowsingContext;
      WorkerContext: typeof Transmute.WorkerContext;
    };
    /**
     * It returns the transmute env module, which provides the access to the client context.
     */
    _linkedBinding(module: 'transmute:env'): {
      ClientContext: typeof Transmute.TrClientContext;
    };
    /**
     * It returns the transmute messaging module, which provides the access to the classes for messaging, such as `NativeEventTarget`.
     */
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
    /**
     * It loads the transmute's native canvas module, which contains the implementation of WebCanvas APIs.
     */
    _linkedBinding(module: 'transmute:canvas'): {
      ImageData: typeof ImageData;
      ImageBitmap: typeof ImageBitmap;
      OffscreenCanvas: typeof OffscreenCanvas;
      createImageBitmap: (image: ImageData | ImageBitmap) => Promise<ImageBitmap>;
    };
    /**
     * It loads the transmute's native webgl module, which contains the implementation of WebGL APIs.
     */
    _linkedBinding(module: 'transmute:webgl'): {
      WebGLRenderingContext: typeof Transmute.WebGLRenderingContextOnDevice;
      WebGL2RenderingContext: typeof Transmute.WebGL2RenderingContextOnDevice;
      placeholders: Transmute.WebGLNativePlaceholders;
    };
    /**
     * It loads the transmute's native webxr module, which contains the implementation of WebXR APIs.
     */
    _linkedBinding(module: 'transmute:webxr'): {
      /**
       * It returns the `XRSystem` object, which is the entry point to the WebXR API.
       * 
       * @returns The `XRSystem` object.
       */
      getXRSystem(): XRSystem;
      XRSession: any;
      XRFrame: typeof XRFrame;
      XRRigidTransform: typeof XRRigidTransform;
    };
    _linkedBinding(module: string): any;
  }
}
