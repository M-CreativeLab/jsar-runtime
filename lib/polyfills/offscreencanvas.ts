import fs from 'fs/promises';
import os from 'os';
import { basename } from 'path';
import CanvasKitInit, {
  EmulatedCanvas2D,
  CanvasKit,
  Font as CanvasKitFont,
  Typeface as CanvasKitTypeface,
  Image as CanvasKitImage,
  type EmulatedCanvas2DContext,
} from 'canvaskit-wasm';
import * as fontkit from 'fontkit';

import getSystemFonts from './get-system-fonts';
import * as logger from '../bindings/logger';
import canvaskitWasmData from './canvaskit-wasm';

type Font = {
  buffer: Buffer;
  descriptors: {
    family: string;
    style?: string;
  };
  typeface: CanvasKitTypeface;
  ascent: number;
  descent: number;
};

const loadedFonts: Font[] = [];
let canvasKit: CanvasKit = null;

const defaultHeight = 16;
const fontStringRegex = new RegExp(
  '(italic|oblique|normal|)\\s*' +              // style
  '(small-caps|normal|)\\s*' +                  // variant
  '(bold|bolder|lighter|[1-9]00|normal|)\\s*' + // weight
  '([\\d\\.]+)' +                               // size
  '(px|pt|pc|in|cm|mm|%|em|ex|ch|rem|q)' +      // unit
  // line-height is ignored here, as per the spec
  '(.+)'                                        // family
);

function toFontStr(input: Buffer | string) {
  let s: string;
  if (Buffer.isBuffer(input)) {
    s = input.toString('utf8');
  } else {
    s = input;
  }
  /**
   * FIXME: This is a hacky way to remove the null character from the fontkit.
   */
  return s.replace(/\u0000/g, '');
}

function parseFontString(fontStr: string) {
  const font = fontStringRegex.exec(fontStr);
  if (!font) {
    return null;
  }

  let size = parseFloat(font[4]);
  let sizePx = defaultHeight;
  let unit = font[5];
  switch (unit) {
    case 'em':
    case 'rem':
      sizePx = size * defaultHeight;
      break;
    case 'pt':
      sizePx = size * 4 / 3;
      break;
    case 'px':
      sizePx = size;
      break;
    case 'pc':
      sizePx = size * defaultHeight;
      break;
    case 'in':
      sizePx = size * 96;
      break;
    case 'cm':
      sizePx = size * 96.0 / 2.54;
      break;
    case 'mm':
      sizePx = size * (96.0 / 25.4);
      break;
    case 'q': // quarter millimeters
      sizePx = size * (96.0 / 25.4 / 4);
      break;
    case '%':
      sizePx = size * (defaultHeight / 75);
      break;
  }
  return {
    'style': font[1],
    'variant': font[2],
    'weight': font[3],
    'sizePx': sizePx,
    'family': font[6].trim()
  };
}

function addFontToCache(font: fontkit.Font, buffer: Buffer) {
  const fontFamily = toFontStr(font.familyName);
  const typeface = canvasKit.Typeface.MakeFreeTypeFaceFromData(buffer);
  loadedFonts.push({
    buffer,
    descriptors: {
      family: fontFamily,
      style: toFontStr(font.subfamilyName),
    },
    ascent: font.ascent,
    descent: font.descent,
    typeface,
  });
}

export const kDisposeCanvas = Symbol('kDisposeCanvas');

export class OffscreenCanvasRenderingContext2DImpl implements OffscreenCanvasRenderingContext2D {
  #nativeCanvas: EmulatedCanvas2D;
  #nativeCanvas2DContext: EmulatedCanvas2DContext;
  #fontToMeasure: CanvasKitFont;
  #isDirty = true;

  /**
   * CanvasKit does not support the text align and baseline, so we need to store them here.
   */
  #textAlign: CanvasTextAlign = 'center';
  #textBaseline: CanvasTextBaseline = 'middle';
  #defaultFontFamily: string;
  #defaultFontSize: number = 12;

  constructor(canvas?: OffscreenCanvasImpl) {
    this.canvas = canvas;
    const nativeCanvas = canvas.nativeCanvas;
    if (!nativeCanvas || nativeCanvas == null) {
      throw new TypeError(`The underlying native canvas is null.`);
    }
    this.#nativeCanvas = nativeCanvas;
    this.#nativeCanvas2DContext = nativeCanvas.getContext('2d');
    this.#fontToMeasure = new canvasKit.Font(null, 10);

    /**
     * Set the default font size and family.
     */
    this.#defaultFontFamily = this.#getDefaultFontFamily();

    /**
     * Set the font via the default value firstly.
     */
    this.#setFont(this.#defaultFontFamily, this.#defaultFontSize);
  }

  #getDefaultFontFamily() {
    switch (os.platform()) {
      case 'android':
        return 'Alibaba PuHuiTi 3.0 55 Regular';
      case 'darwin':
        return 'Noto Sans SC';
      default:
        return 'Arial';
    }
  }

  #setFontByStr(fontStr: string) {
    let { family, sizePx } = parseFontString(fontStr);
    this.#setFont(family, sizePx);
  }

  #setFont(fontFamily: string, fontSize: number) {
    if (!fontFamily || fontFamily === 'sans-serif') {
      fontFamily = this.#defaultFontFamily;
    }
    if (!fontSize) {
      fontSize = this.#defaultFontSize;
    }
    this.#nativeCanvas2DContext.font = `${fontSize}px ${fontFamily}`;
    if (this.#fontToMeasure) {
      const fontObject = loadedFonts.find((font) => font.descriptors.family === fontFamily);
      if (fontObject) {
        this.#fontToMeasure.setSize(fontSize);
        this.#fontToMeasure.setTypeface(fontObject.typeface);
      }
    }
  }

  commit(): void {
    throw new Error('Method(commit) not implemented.');
  }
  reset(): void {
    (this.#nativeCanvas2DContext as any).reset();
    this.#markAsDirty();
  }
  transform(a: number, b: number, c: number, d: number, e: number, f: number): void {
    this.#nativeCanvas2DContext.transform(a, b, c, d, e, f);
    this.#markAsDirty();
  }

  /// Drawing rectangles

  clearRect(x: number, y: number, w: number, h: number): void {
    this.#nativeCanvas2DContext.clearRect(x, y, w, h);
    this.#markAsDirty();
  }
  fillRect(x: number, y: number, w: number, h: number): void {
    this.#nativeCanvas2DContext.fillRect(x, y, w, h);
    this.#markAsDirty();
  }
  strokeRect(x: number, y: number, w: number, h: number): void {
    this.#nativeCanvas2DContext.strokeRect(x, y, w, h);
    this.#markAsDirty();
  }

  /// Drawing text

  fillText(text: string, x: number, y: number, maxWidth?: number): void {
    const { width, actualBoundingBoxAscent, actualBoundingBoxDescent } = this.measureText(text);
    switch (this.#textAlign) {
      case 'center':
        x -= width / 2;
        break;
      case 'right':
        x -= width;
        break;
      default:
        break;
    }

    switch (this.#textBaseline) {
      case 'top':
        y -= actualBoundingBoxAscent;
        break;
      case 'middle':
        y -= (actualBoundingBoxAscent + actualBoundingBoxDescent) / 2;
        break;
      case 'bottom':
        y -= actualBoundingBoxDescent;
        break;
      case 'alphabetic':
      case 'ideographic':
      case 'hanging':
        break;
    }

    this.#nativeCanvas2DContext.fillText(text, x, y, maxWidth);
    this.#markAsDirty();
  }
  drawText(text: string, x: number, y: number, maxWidth?: number): void {
    logger.warn('drawText is deprecated, use fillText instead.');
    this.fillText(text, x, y, maxWidth);
  }
  strokeText(text: string, x: number, y: number, maxWidth?: number): void {
    this.#nativeCanvas2DContext.strokeText(text, x, y, maxWidth);
    this.#markAsDirty();
  }
  measureText(text: string): TextMetrics {
    const { width } = this.#nativeCanvas2DContext.measureText(text);
    const metrics = {
      width,
      actualBoundingBoxAscent: -1,
      actualBoundingBoxDescent: -1,
      actualBoundingBoxLeft: -1,
      actualBoundingBoxRight: -1,
      fontBoundingBoxAscent: -1,
      fontBoundingBoxDescent: -1,
    };

    const { family: fontFamily } = parseFontString(this.font);
    const font = loadedFonts.find((font) => font.descriptors.family === fontFamily);
    if (font) {
      const measureMetrics = this.#fontToMeasure.getMetrics();
      metrics.fontBoundingBoxAscent = measureMetrics.ascent;
      metrics.fontBoundingBoxDescent = measureMetrics.descent;
      metrics.actualBoundingBoxAscent = measureMetrics.ascent;
      metrics.actualBoundingBoxDescent = measureMetrics.descent;
    }
    return metrics;
  }

  /// Line styles

  get lineWidth() {
    return this.#nativeCanvas2DContext.lineWidth;
  }
  set lineWidth(value) {
    this.#nativeCanvas2DContext.lineWidth = value;
  }
  get lineCap() {
    return this.#nativeCanvas2DContext.lineCap;
  }
  set lineCap(value) {
    this.#nativeCanvas2DContext.lineCap = value;
  }
  get lineJoin() {
    return this.#nativeCanvas2DContext.lineJoin;
  }
  set lineJoin(value) {
    this.#nativeCanvas2DContext.lineJoin = value;
  }
  get miterLimit() {
    return this.#nativeCanvas2DContext.miterLimit;
  }
  set miterLimit(value) {
    this.#nativeCanvas2DContext.miterLimit = value;
  }
  get lineDashOffset() {
    return this.#nativeCanvas2DContext.lineDashOffset;
  }
  set lineDashOffset(value) {
    this.#nativeCanvas2DContext.lineDashOffset = value;
  }

  getLineDash(): number[] {
    return this.#nativeCanvas2DContext.getLineDash();
  }
  setLineDash(segments: number[]): void {
    this.#nativeCanvas2DContext.setLineDash(segments);
    this.#markAsDirty();
  }

  /// Text styles

  get font(): string {
    return this.#nativeCanvas2DContext.font;
  }
  set font(value: string) {
    this.#setFontByStr(value);
  }

  get textAlign() {
    return this.#textAlign;
  }
  set textAlign(value) {
    this.#textAlign = value;
  }
  get textBaseline() {
    return this.#textBaseline;
  }
  set textBaseline(value) {
    this.#textBaseline = value;
  }
  get direction() {
    return this.#nativeCanvas2DContext.direction;
  }
  set direction(value) {
    this.#nativeCanvas2DContext.direction = value;
  }
  get fontKerning() {
    return this.#nativeCanvas2DContext.fontKerning;
  }
  set fontKerning(value) {
    this.#nativeCanvas2DContext.fontKerning = value;
  }

  /// Fill and stroke styles

  get fillStyle() {
    return this.#nativeCanvas2DContext.fillStyle;
  }
  set fillStyle(value) {
    this.#nativeCanvas2DContext.fillStyle = value;
  }
  get strokeStyle() {
    return this.#nativeCanvas2DContext.strokeStyle;
  }
  set strokeStyle(value) {
    this.#nativeCanvas2DContext.strokeStyle = value;
  }

  /// Gradients and patterns

  createConicGradient(startAngle: number, x: number, y: number): CanvasGradient {
    return this.#nativeCanvas2DContext.createConicGradient(startAngle, x, y);
  }
  createLinearGradient(x0: number, y0: number, x1: number, y1: number): CanvasGradient {
    return this.#nativeCanvas2DContext.createLinearGradient(x0, y0, x1, y1);
  }
  createRadialGradient(x0: number, y0: number, r0: number, x1: number, y1: number, r1: number): CanvasGradient {
    return this.#nativeCanvas2DContext.createRadialGradient(x0, y0, r0, x1, y1, r1);
  }
  createPattern(image: CanvasImageSource, repetition: string): CanvasPattern {
    return this.#nativeCanvas2DContext.createPattern(image, repetition);
  }

  /// Shadows

  get shadowBlur() {
    return this.#nativeCanvas2DContext.shadowBlur;
  }
  set shadowBlur(value) {
    this.#nativeCanvas2DContext.shadowBlur = value;
  }
  get shadowColor() {
    return this.#nativeCanvas2DContext.shadowColor;
  }
  set shadowColor(value) {
    this.#nativeCanvas2DContext.shadowColor = value;
  }
  get shadowOffsetX() {
    return this.#nativeCanvas2DContext.shadowOffsetX;
  }
  set shadowOffsetX(value) {
    this.#nativeCanvas2DContext.shadowOffsetX = value;
  }
  get shadowOffsetY() {
    return this.#nativeCanvas2DContext.shadowOffsetY;
  }
  set shadowOffsetY(value) {
    this.#nativeCanvas2DContext.shadowOffsetY = value;
  }

  /// Paths

  beginPath(): void {
    this.#nativeCanvas2DContext.beginPath();
    this.#markAsDirty();
  }
  closePath(): void {
    this.#nativeCanvas2DContext.closePath();
    this.#markAsDirty();
  }
  moveTo(x: number, y: number): void {
    this.#nativeCanvas2DContext.moveTo(x, y);
    this.#markAsDirty();
  }
  lineTo(x: number, y: number): void {
    this.#nativeCanvas2DContext.lineTo(x, y);
    this.#markAsDirty();
  }
  bezierCurveTo(cp1x: number, cp1y: number, cp2x: number, cp2y: number, x: number, y: number): void {
    this.#nativeCanvas2DContext.bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
    this.#markAsDirty();
  }
  quadraticCurveTo(cpx: number, cpy: number, x: number, y: number): void {
    this.#nativeCanvas2DContext.quadraticCurveTo(cpx, cpy, x, y);
    this.#markAsDirty();
  }
  arc(x: number, y: number, radius: number, startAngle: number, endAngle: number, anticlockwise?: boolean): void {
    this.#nativeCanvas2DContext.arc(x, y, radius, startAngle, endAngle, anticlockwise);
    this.#markAsDirty();
  }
  arcTo(x1: number, y1: number, x2: number, y2: number, radius: number): void {
    this.#nativeCanvas2DContext.arcTo(x1, y1, x2, y2, radius);
    this.#markAsDirty();
  }
  ellipse(x: number, y: number, radiusX: number, radiusY: number, rotation: number, startAngle: number, endAngle: number, anticlockwise?: boolean): void {
    this.#nativeCanvas2DContext.ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise);
    this.#markAsDirty();
  }
  rect(x: number, y: number, w: number, h: number): void {
    this.#nativeCanvas2DContext.rect(x, y, w, h);
    this.#markAsDirty();
  }
  roundRect(x: number, y: number, w: number, h: number, radii?: number | DOMPointInit | (number | DOMPointInit)[]): void;
  roundRect(x: number, y: number, w: number, h: number, radii?: number | DOMPointInit | Iterable<number | DOMPointInit>): void;
  roundRect(x: unknown, y: unknown, w: unknown, h: unknown, radii?: unknown): void {
    this.#nativeCanvas2DContext.roundRect.apply(this.#nativeCanvas2DContext, arguments);
    this.#markAsDirty();
  }

  /// Drawing paths

  fill(fillRule?: CanvasFillRule): void;
  fill(path: Path2D, fillRule?: CanvasFillRule): void;
  fill(_path?: unknown, _fillRule?: unknown): void {
    this.#nativeCanvas2DContext.fill.apply(this.#nativeCanvas2DContext, arguments);
    this.#markAsDirty();
  }
  stroke(path?: Path2D): void {
    this.#nativeCanvas2DContext.stroke(path);
    this.#markAsDirty();
  }
  drawFocusIfNeeded(element: Element): void;
  drawFocusIfNeeded(path: Path2D, element: Element): void;
  drawFocusIfNeeded(_path: unknown, _element?: unknown): void {
    this.#nativeCanvas2DContext.drawFocusIfNeeded.apply(this.#nativeCanvas2DContext, arguments);
    this.#markAsDirty();
  }
  scrollPathIntoView(): void;
  scrollPathIntoView(path: Path2D): void;
  scrollPathIntoView(_path?: unknown): void {
    throw new TypeError('Method(scrollPathIntoView) not implemented.');
  }
  clip(fillRule?: CanvasFillRule): void;
  clip(path: Path2D, fillRule?: CanvasFillRule): void;
  clip(_path?: unknown, _fillRule?: unknown): void {
    this.#nativeCanvas2DContext.clip.apply(this.#nativeCanvas2DContext, arguments);
    this.#markAsDirty();
  }
  isPointInPath(x: number, y: number, fillRule?: CanvasFillRule): boolean;
  isPointInPath(path: Path2D, x: number, y: number, fillRule?: CanvasFillRule): boolean;
  isPointInPath(_path: unknown, _x: unknown, _y?: unknown, _fillRule?: unknown): boolean {
    return this.#nativeCanvas2DContext.isPointInPath.apply(this.#nativeCanvas2DContext, arguments);
  }
  isPointInStroke(x: number, y: number): boolean;
  isPointInStroke(path: Path2D, x: number, y: number): boolean;
  isPointInStroke(_path: unknown, _x: unknown, _y?: unknown): boolean {
    return this.#nativeCanvas2DContext.isPointInStroke.apply(this.#nativeCanvas2DContext, arguments);
  }

  /// Transformations

  getTransform(): DOMMatrix {
    return this.#nativeCanvas2DContext.getTransform();
  }
  rotate(angle: number): void {
    this.#nativeCanvas2DContext.rotate(angle);
    this.#markAsDirty();
  }
  scale(x: number, y: number): void {
    this.#nativeCanvas2DContext.scale(x, y);
    this.#markAsDirty();
  }
  translate(x: number, y: number): void {
    this.#nativeCanvas2DContext.translate(x, y);
    this.#markAsDirty();
  }
  setTransform(a: number, b: number, c: number, d: number, e: number, f: number): void;
  setTransform(transform?: DOMMatrix2DInit): void;
  setTransform(_a?: unknown, _b?: unknown, _c?: unknown, _d?: unknown, _e?: unknown, _f?: unknown): void {
    this.#nativeCanvas2DContext.setTransform.apply(this.#nativeCanvas2DContext, arguments);
    this.#markAsDirty();
  }
  resetTransform(): void {
    this.#nativeCanvas2DContext.resetTransform();
    this.#markAsDirty();
  }

  /// Compositing

  get globalAlpha() {
    return this.#nativeCanvas2DContext.globalAlpha;
  }
  set globalAlpha(value) {
    this.#nativeCanvas2DContext.globalAlpha = value;
  }
  get globalCompositeOperation() {
    return this.#nativeCanvas2DContext.globalCompositeOperation;
  }
  set globalCompositeOperation(value) {
    this.#nativeCanvas2DContext.globalCompositeOperation = value;
  }

  /// Drawing images

  drawImage(image: CanvasImageSource, dx: number, dy: number): void;
  drawImage(image: CanvasImageSource, dx: number, dy: number, dw: number, dh: number): void;
  drawImage(image: CanvasImageSource, sx: number, sy: number, sw: number, sh: number, dx: number, dy: number, dw: number, dh: number): void;
  drawImage(image: any, sx: unknown, sy: unknown, sw?: unknown, sh?: unknown, dx?: unknown, dy?: unknown, dw?: unknown, dh?: unknown): void {
    if (!(image instanceof ImageBitmapImpl)) {
      throw new TypeError('Only ImageBitmap is supported.');
    }
    const imageWidth = image.width;
    const imageHeight = image.height;
    const skImage = image._getSkImage();
    if (skImage == null) {
      logger.warn('Invalid ImageBitmap instance, skImage is null.');
      return;
    }
    if (skImage.isDeleted() === true) {
      logger.warn('The underlying image is deleted, skipped to draw this image.');
      throw new Error('The underlying image is deleted, skipped to draw this image.');
    }
    this.#nativeCanvas2DContext.drawImage.apply(this.#nativeCanvas2DContext, [
      skImage,
      sx,
      sy,
      sw || imageWidth,
      sh || imageHeight,
      dx || 0,
      dy || 0,
      dw || imageWidth,
      dh || imageHeight,
    ]);
    this.#markAsDirty();
  }

  /// Pixel manipulation

  createImageData(sw: number, sh: number, settings?: ImageDataSettings): ImageData;
  createImageData(imagedata: ImageData): ImageData;
  createImageData(_sw: unknown, _sh?: unknown, _settings?: unknown): ImageData {
    return this.#nativeCanvas2DContext.createImageData.apply(this.#nativeCanvas2DContext, arguments);
  }
  getImageData(sx: number, sy: number, sw: number, sh: number, settings?: ImageDataSettings): ImageData {
    return this.#nativeCanvas2DContext.getImageData(sx, sy, sw, sh, settings);
  }
  putImageData(imagedata: ImageData, dx: number, dy: number): void;
  putImageData(imagedata: ImageData, dx: number, dy: number, dirtyX: number, dirtyY: number, dirtyWidth: number, dirtyHeight: number): void;
  putImageData(_imagedata: unknown, _dx: unknown, _dy: unknown, _dirtyX?: unknown, _dirtyY?: unknown, _dirtyWidth?: unknown, _dirtyHeight?: unknown): void {
    this.#nativeCanvas2DContext.putImageData.apply(this.#nativeCanvas2DContext, arguments);
    this.#markAsDirty();
  }

  /// Image smoothing

  get imageSmoothingEnabled() {
    return this.#nativeCanvas2DContext.imageSmoothingEnabled;
  }
  set imageSmoothingEnabled(value) {
    this.#nativeCanvas2DContext.imageSmoothingEnabled = value;
  }
  get imageSmoothingQuality() {
    return this.#nativeCanvas2DContext.imageSmoothingQuality;
  }
  set imageSmoothingQuality(value) {
    this.#nativeCanvas2DContext.imageSmoothingQuality = value;
  }

  /// The canvas state
  save(): void {
    this.#nativeCanvas2DContext.save();
    this.#markAsDirty();
  }
  restore(): void {
    this.#nativeCanvas2DContext.restore();
    this.#markAsDirty();
  }
  canvas: OffscreenCanvas;
  getContextAttributes(): CanvasRenderingContext2DSettings {
    return this.#nativeCanvas2DContext.getContextAttributes();
  }

  /// Filters
  get filter() {
    return this.#nativeCanvas2DContext.filter;
  }
  set filter(value) {
    this.#nativeCanvas2DContext.filter = value;
  }

  // The followings are JSAR-specfic methods to optimize the transmute messaging.
  #markAsDirty() {
    this.#isDirty = true;
  }
  markAsNotDirty() {
    this.#isDirty = false;
  }
  isDirty() {
    return this.#isDirty;
  }
}

/**
 * OffscreenCanvas
 */
export class OffscreenCanvasImpl extends EventTarget implements OffscreenCanvas {
  width: number = 0;
  height: number = 0;
  #nativeCanvas: EmulatedCanvas2D;
  #context2d: OffscreenCanvasRenderingContext2DImpl;

  oncontextlost: (event: Event) => any;
  oncontextrestored: (event: Event) => any;

  constructor(width: number, height: number) {
    super();
    this.width = width;
    this.height = height;
  }

  get nativeCanvas() {
    return this.#nativeCanvas;
  }

  #loadDefaultFonts() {
    for (let font of loadedFonts) {
      this.#nativeCanvas.loadFont(font.buffer, font.descriptors);
      logger.info(`loaded font: ${font.descriptors.family}`);
    }
  }

  getContext(contextId: '2d', contextAttributes?: any): OffscreenCanvasRenderingContext2D;
  getContext(contextId: string, contextAttributes?: any): any;
  getContext(contextId: string, _contextAttributes?: any): any {
    if (contextId === '2d') {
      if (this.#context2d) {
        return this.#context2d;
      } else {
        this.#nativeCanvas = canvasKit.MakeCanvas(this.width, this.height);
        if (this.#nativeCanvas == null) {
          throw new TypeError(`Failed to create native canvas with size: ${this.width}x${this.height}.`);
        }
        this.#context2d = new OffscreenCanvasRenderingContext2DImpl(this);
        this.#loadDefaultFonts();
        return this.#context2d;
      }
    } else {
      throw new TypeError('Method(getContext) not implemented for non-2d.');
    }
  }

  transferToImageBitmap(): ImageBitmap {
    return null;
  }

  convertToBlob(_options?: any): Promise<Blob> {
    return null;
  }

  toDataURL(mime: string) {
    return this.#nativeCanvas.toDataURL(mime);
  }

  [kDisposeCanvas]() {
    this.#nativeCanvas.dispose();
  }
}

export class ImageBitmapImpl implements ImageBitmap {
  private static _liveCount = 0;

  height: number;
  width: number;
  private skImage: CanvasKitImage;
  constructor(buffer: ArrayBuffer) {
    const image = canvasKit.MakeImageFromEncoded(buffer);
    if (!image) {
      throw new Error(`Failed to create image from buffer: ${buffer}`);
    }
    this.skImage = image;
    this.height = image.height();
    this.width = image.width();
    ImageBitmapImpl._liveCount += 1;
    logger.info(`ImageBitmap(${this.width}x${this.height}) is created.`);
  }

  close(): void {
    try {
      this.skImage.delete();
      ImageBitmapImpl._liveCount -= 1;
    } catch (_) {
      // ts-ignore
    } finally {
      logger.info(`ImageBitmap(${this.width}x${this.height}) is disposed, and ${ImageBitmapImpl._liveCount} left.`);
    }
    return;
  }

  _getSkImage() {
    return this.skImage;
  }
}

export class ImageDataImpl implements ImageData {
  constructor(data: any, sw: unknown, sh?: unknown, settings?: unknown) {
    if (typeof data === 'number') {
      settings = sh as ImageDataSettings;
      sh = sw as number;
      sw = data as number;
      data = null;
    }

    if (data != null) {
      if (data.length % 4) {
        throw new Error(`The data array to create ImageData must be a multiple of 4, but got ${data.length}.`);
      }
      this.data = data as Uint8ClampedArray;
    }
    this.width = sw as number;
    this.height = sh as number || this.width;

    if (settings) {
      this.colorSpace = (settings as ImageDataSettings).colorSpace;
    }
  }

  data: Uint8ClampedArray;
  height: number;
  width: number;
  colorSpace: PredefinedColorSpace = 'srgb';
}

export async function createImageBitmapImpl(source: ImageBitmapSource, _sx?: unknown, _sy?: unknown, _sw?: unknown, _sh?: unknown, _options?: unknown): Promise<ImageBitmap> {
  if (!(source instanceof Blob)) {
    throw new TypeError('createImageBitmap only supports Blob objects as input.');
  }
  if (arguments.length > 1) {
    throw new TypeError('createImageBitmap with cropping options is not supported.');
  }
  return new ImageBitmapImpl(await source.arrayBuffer());
}

export const InitializeOffscreenCanvas = async (options: Partial<{ loadSystemFonts: boolean }> = {}) => {
  if (canvasKit != null) {
    throw new TypeError('OffscreenCanvas already initialized');
  }

  /**
   * FIXME(Yorkie): This is a hacky way to let canvaskit to be load with data URI.
   */
  globalThis.window = {} as any;
  canvasKit = await CanvasKitInit({
    locateFile: (_) => canvaskitWasmData,
  });
  // Clear the window object.
  delete globalThis.window;
  logger.info('CanvasKit is initialized.');

  // Load system fonts
  if (options.loadSystemFonts === true) {
    logger.info('Loading system fonts...');
    const systemFonts = (
      await getSystemFonts({
        extensions: ['ttf'],
      })
    ).filter(fontFilename => {
      const name = basename(fontFilename, '.ttf').toLowerCase();
      return ['arial', 'droidsans'].includes(name) ||
        name.startsWith('NotoSansSC-Regular'.toLowerCase()) ||
        name.startsWith('AlibabaPuhuiti'.toLowerCase());
    });
    logger.info(`Detected ${systemFonts.length} fonts`, systemFonts);

    await Promise.all(
      systemFonts.map(async (fontPath) => {
        try {
          const fontData = await fs.readFile(fontPath);
          const fontOrCollection = fontkit.create(fontData) as any;
          addFontToCache(fontOrCollection, fontData);
        } catch (err) {
          logger.warn(`failed to load font: ${fontPath}, reason:`, err);
        }
      })
    );
  }

  // returns the canvaskit for other modules to use.
  return canvasKit;
};
