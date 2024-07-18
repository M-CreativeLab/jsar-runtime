import {
  ImageData,
  ImageBitmap,
  createImageBitmap,
  OffscreenCanvas,
} from '../webcanvas/canvas2d';

/**
 * Canvas2D classes
 */
Object.defineProperties(globalThis, {
  'ImageData': {
    value: ImageData,
    writable: false,
    enumerable: true,
    configurable: false,
  },
  'ImageBitmap': {
    value: ImageBitmap,
    writable: false,
    enumerable: true,
    configurable: false,
  },
  'OffscreenCanvas': {
    value: OffscreenCanvas,
    writable: false,
    enumerable: true,
    configurable: false,
  },
});

Object.defineProperties(globalThis, {
  'createImageBitmap': {
    value: createImageBitmap,
    writable: false,
    enumerable: true,
    configurable: false,
  },
});
