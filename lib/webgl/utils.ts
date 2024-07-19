type ArgType = 'default' | 'ignore' | 'constant';
type TypedArray =
  Uint8Array |
  Uint8ClampedArray |
  Int8Array |
  Uint16Array |
  Int16Array |
  Uint32Array |
  Int32Array |
  Float32Array |
  Float64Array;

export type NativeCallOptions = Partial<{
  debug: {
    argTypes?: ArgType[];
    argSep?: string;
  };
}>;

const glConstantNamesMap: Map<number, string> = new Map();

export function isTypedArray(data: any): data is TypedArray {
  return data instanceof Uint8Array ||
    data instanceof Uint8ClampedArray ||
    data instanceof Int8Array ||
    data instanceof Uint16Array ||
    data instanceof Int16Array ||
    data instanceof Uint32Array ||
    data instanceof Int32Array ||
    data instanceof Float32Array ||
    data instanceof Float64Array;
}

export function unpackTypedArray(array: DataView | ArrayBufferView): Uint8Array {
  return (new Uint8Array(array.buffer)).subarray(
    array.byteOffset,
    array.byteLength + array.byteOffset);
}

/**
 * It fetches all the constants from the native implementation and stores them in a map `#constantNamesMap`, which is used to 
 * get the name of the constant from its value in debugging purposes.
 */
export function setupConstantNamesMap(instance: any, nativeContextType: typeof WebGLRenderingContext | typeof WebGL2RenderingContext) {
  if (instance == null || !instance) {
    throw new TypeError('The instance of WebGLRenderingContext or WebGL2RenderingContext is not valid');
  }
  const allConstantNames = Object.getOwnPropertyNames(nativeContextType.prototype)
    .filter(name => typeof instance[name] === 'number');
  for (const name of allConstantNames) {
    const value = instance[name];
    glConstantNamesMap.set(value, name);
  }
}

export function makeNativeCall(
  this: WebGLRenderingContext | WebGL2RenderingContext,
  fn: Function,
  name: string,
  args: any[] = [],
  options: NativeCallOptions = {}
) {
  if (typeof fn !== 'function') {
    throw new TypeError(`The method(${name}) in native WebGLRenderingContext is not supported`);
  }

  let r: any;
  try {
    r = fn.apply(this, args);
  } catch (err) {
    console.error(`Failed to make native call => gl.${name}(${args.join(', ')})`, err);
    throw err;
  }
  return r;
}
