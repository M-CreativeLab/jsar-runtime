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
    _linkedBinding(module: 'transmute:webgl'): {
      WebGLRenderingContext: typeof WebGLRenderingContext;
    };
    _linkedBinding(module: string): any;
  }
}
