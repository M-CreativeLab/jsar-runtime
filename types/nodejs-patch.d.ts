/// <reference path="transmute-private.d.ts" />

// Custom the Node.js `process._linkedBinding` method to support the JSAR's native modules.
declare namespace NodeJS {
  interface Process {
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
    _linkedBinding(module: 'transmute:renderer'): {
      RenderLoop: typeof Transmute.RenderLoop;
      AnimationFrameListener: typeof Transmute.AnimationFrameListener;
    };

    /**
     * Loads a native linked binding.
     * 
     * @param module The name of the module to load.
     */
    _linkedBinding(module: string): any;
  }
}
