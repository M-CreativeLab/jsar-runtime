import * as logger from '../bindings/logger';
import { ImageBitmapImpl, OffscreenCanvasImpl, kReadPixels } from '../polyfills/offscreencanvas';

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

let isEnableDebugging: boolean = false;
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

export function unpackTypedArray(array: DataView | ArrayBufferView) {
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
    logger.error(`Failed to make native call => gl.${name}(${args.join(', ')})`, err);
    throw err;
  }

  if (isEnableDebugging) {
    const { argTypes, argSep = ', ' } = options.debug || {};
    let argsStr: string;
    if (argTypes) {
      argsStr = args
        .filter((_, i) => argTypes[i] !== 'ignore')
        .map((arg, i) => {
          if (argTypes[i] === 'constant') {
            return glConstantNamesMap.has(arg) ? `${glConstantNamesMap.get(arg)}(${arg})` : `${arg}`;
          } else {
            if (isTypedArray(arg) || arg instanceof ArrayBuffer || arg instanceof DataView) {
              return `${arg.constructor.name}(${arg.byteLength})`;
            }
            return arg;
          }
        })
        .join(argSep);
    } else {
      argsStr = args.join(argSep);
    }

    let returnStr = '';
    if (Array.isArray(r)) {
      returnStr = `=> (${r.length}) { ${r.slice(0, 3).join(', ')} }`;
    } else if (typeof r === 'number' || typeof r === 'boolean') {
      returnStr = `=> ${r}`;
    } else if (typeof r !== 'undefined') {
      returnStr = `=> <${typeof r}>`;
    }
    logger.info(`WebGL::${name}(${argsStr}) ${returnStr}`);
  }
  return r;
}

export function isTexImageSource(imageSource: unknown): imageSource is TexImageSource {
  return (
    imageSource instanceof ImageBitmapImpl ||
    imageSource instanceof OffscreenCanvasImpl
  );
}

export function getPixelsFromTexImageSource(
  colorType: 'rgb8' | 'rgba8',
  imageSource: TexImageSource
): {
  width: number;
  height: number;
  data: ArrayBufferView;
} {
  let pixels: ArrayBufferView;
  if (
    imageSource instanceof ImageBitmapImpl ||
    imageSource instanceof OffscreenCanvasImpl
  ) {
    pixels = imageSource[kReadPixels]({
      colorType,
    });
  } else {
    throw new Error(`Unsupported image source type`);
  }
  return {
    width: imageSource.width,
    height: imageSource.height,
    data: pixels,
  };
}

export function getTextureParametersFromImageSource(
  this: WebGLRenderingContext | WebGL2RenderingContext,
  ...args: any[]
): {
  format: number;
  type: number;
  width: number;
  height: number;
  pixels: ArrayBufferView;
} {
  const format = <number>(args[3]);
  const type = <number>(args[4]);
  const imageSource = <TexImageSource>(args[5]);

  let pixels: ArrayBufferView;
  if (
    imageSource instanceof ImageBitmapImpl ||
    imageSource instanceof OffscreenCanvasImpl
  ) {
    pixels = imageSource[kReadPixels]({
      colorType: format === this.RGB ? 'rgb8' : 'rgba8',
    });
  } else {
    logger.error(`Unsupported image source type for texImage2D():`, imageSource);
    throw new Error(`Unsupported image source type for texImage2D()`);
  }

  // Convert ArrayBuffer to TypedArray
  if (pixels instanceof ArrayBuffer) {
    pixels = new Uint8Array(pixels);
  }

  return {
    format,
    type,
    width: imageSource.width,
    height: imageSource.height,
    pixels,
  };
}
