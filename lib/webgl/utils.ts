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

// eslint-disable-next-line @typescript-eslint/no-explicit-any
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

export function makeNativeCall(
  this: WebGLRenderingContext | WebGL2RenderingContext,
  // eslint-disable-next-line @typescript-eslint/no-unsafe-function-type
  fn: Function,
  name: string,
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  args: any[] = [],
  _options: NativeCallOptions = {}
) {
  if (typeof fn !== 'function') {
    throw new TypeError(`The method(${name}) in native WebGLRenderingContext is not supported`);
  }

  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  let r: any;
  try {
    r = fn.apply(this, args);
  } catch (err) {
    console.error(`Failed to make native call => gl.${name}(${args.join(', ')})`, err);
    throw err;
  }
  return r;
}
