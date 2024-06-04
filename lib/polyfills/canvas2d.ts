const canvasbinding = process._linkedBinding('transmute:canvas');

export const ImageData = canvasbinding.ImageData;
export const ImageBitmap = canvasbinding.ImageBitmap;
export const createImageBitmap = canvasbinding.createImageBitmap;
// export const OffscreenCanvas = canvasbinding.OffscreenCanvas;
