import * as logger from '../bindings/logger';
import { WebGLShaderPrecisionFormatImpl } from './WebGLShaderPrecisionFormat';

const glNative = process._linkedBinding('transmute:webgl');
const isEnableDebugging = true;

type ArgType = 'default' | 'ignore' | 'constant';
type NativeCallOptions = Partial<{
  debug: {
    argTypes?: ArgType[];
    argSep?: string;
  };
}>;

export default class WebGLRenderingContextImpl extends glNative.WebGLRenderingContext implements WebGLRenderingContext {
  canvas: HTMLCanvasElement | OffscreenCanvas;
  drawingBufferColorSpace: PredefinedColorSpace;
  #constantNamesMap: Map<number, string> = new Map();

  get drawingBufferHeight(): number {
    return super.drawingBufferHeight;
  }
  get drawingBufferWidth(): number {
    return super.drawingBufferWidth;
  }

  constructor(_canvas: HTMLCanvasElement | OffscreenCanvas, options: WebGLContextAttributes) {
    super();
    this.#setupConstantNamesMap();
  }

  /**
   * It fetches all the constants from the native implementation and stores them in a map `#constantNamesMap`, which is used to 
   * get the name of the constant from its value in debugging purposes.
   */
  #setupConstantNamesMap() {
    const allConstantNames = Object.getOwnPropertyNames(glNative.WebGLRenderingContext.prototype)
      .filter(name => typeof this[name] === 'number');
    for (const name of allConstantNames) {
      const value = this[name];
      this.#constantNamesMap.set(value, name);
    }
  }

  private nativeCall(name: string, args: any[] = [], options: NativeCallOptions = {}) {
    const fn = <Function>super[name];
    if (typeof fn !== 'function') {
      throw new TypeError(`The method(${name}) in native WebGLRenderingContext is not supported`);
    }
    const r = fn.apply(this, args);
    if (isEnableDebugging) {
      const { argTypes, argSep = ', ' } = options.debug || {};
      let argsStr: string;
      if (argTypes) {
        argsStr = args
          .filter((_, i) => argTypes[i] !== 'ignore')
          .map((arg, i) => {
            if (argTypes[i] === 'constant') {
              return this.#constantNamesMap.has(arg) ? `${this.#constantNamesMap.get(arg)}(${arg})` : `${arg}`;
            } else {
              return arg;
            }
          })
          .join(argSep);
      } else {
        argsStr = args.join(argSep);
      }

      let returnStr = '';
      if (typeof r !== 'undefined') {
        returnStr = `=> ${r}`;
      }
      logger.info(`WebGL::${name}(${argsStr}) ${returnStr}`);
    }
    return r;
  }

  activeTexture(texture: number): void {
    return this.nativeCall('activeTexture', [texture]);
  }
  attachShader(program: WebGLProgram, shader: WebGLShader): void {
    return this.nativeCall('attachShader', [program, shader]);
  }
  bindAttribLocation(program: WebGLProgram, index: number, name: string): void {
    return this.nativeCall('bindAttribLocation', [program, index, name]);
  }
  bindBuffer(target: number, buffer: WebGLBuffer): void {
    return this.nativeCall('bindBuffer', [target, buffer], {
      debug: {
        argTypes: ['constant',],
      },
    });
  }
  bindFramebuffer(target: number, framebuffer: WebGLFramebuffer): void {
    return this.nativeCall('bindFramebuffer', [target, framebuffer], {
      debug: {
        argTypes: ['constant',],
      },
    });
  }
  bindRenderbuffer(target: number, renderbuffer: WebGLRenderbuffer): void {
    return this.nativeCall('bindRenderbuffer', [target, renderbuffer], {
      debug: {
        argTypes: ['constant',],
      },
    });
  }
  bindTexture(target: number, texture: WebGLTexture): void {
    return this.nativeCall('bindTexture', [target, texture], {
      debug: {
        argTypes: ['constant',],
      },
    });
  }
  blendColor(red: number, green: number, blue: number, alpha: number): void {
    return this.nativeCall('blendColor', [red, green, blue, alpha]);
  }
  blendEquation(mode: number): void {
    return this.nativeCall('blendEquation', [mode]);
  }
  blendEquationSeparate(modeRGB: number, modeAlpha: number): void {
    return this.nativeCall('blendEquationSeparate', [modeRGB, modeAlpha]);
  }
  blendFunc(sfactor: number, dfactor: number): void {
    return this.nativeCall('blendFunc', [sfactor, dfactor], {
      debug: {
        argTypes: ['constant', 'constant'],
      }
    });
  }
  blendFuncSeparate(srcRGB: number, dstRGB: number, srcAlpha: number, dstAlpha: number): void {
    return this.nativeCall('blendFuncSeparate', [srcRGB, dstRGB, srcAlpha, dstAlpha], {
      debug: {
        argTypes: ['constant', 'constant', 'constant', 'constant'],
      }
    });
  }
  checkFramebufferStatus(target: number): number {
    return this.nativeCall('checkFramebufferStatus', [target]);
  }
  clear(mask: number): void {
    // super.clear(mask);
  }
  clearColor(red: number, green: number, blue: number, alpha: number): void {
    // super.clearColor(red, green, blue, alpha);
  }
  clearDepth(depth: number): void {
    // super.clearDepth(depth);
  }
  clearStencil(s: number): void {
    // super.clearStencil(s);
  }
  colorMask(red: boolean, green: boolean, blue: boolean, alpha: boolean): void {
    return this.nativeCall('colorMask', [red, green, blue, alpha]);
  }
  compileShader(shader: WebGLShader): void {
    return this.nativeCall('compileShader', [shader]);
  }
  copyTexImage2D(
    target: number,
    level: number,
    internalformat: number,
    x: number,
    y: number,
    width: number,
    height: number,
    border: number
  ): void {
    return this.nativeCall('copyTexImage2D', [target, level, internalformat, x, y, width, height, border]);
  }
  copyTexSubImage2D(
    target: number,
    level: number,
    xoffset: number,
    yoffset: number,
    x: number,
    y: number,
    width: number,
    height: number
  ): void {
    return this.nativeCall('copyTexSubImage2D', [target, level, xoffset, yoffset, x, y, width, height]);
  }
  createBuffer(): WebGLBuffer {
    return this.nativeCall('createBuffer');
  }
  createFramebuffer(): WebGLFramebuffer {
    return this.nativeCall('createFramebuffer');
  }
  createProgram(): WebGLProgram {
    return this.nativeCall('createProgram');
  }
  createRenderbuffer(): WebGLRenderbuffer {
    return this.nativeCall('createRenderbuffer');
  }
  createShader(type: number): WebGLShader {
    return this.nativeCall('createShader', [type], {
      debug: {
        argTypes: ['constant'],
      }
    });
  }
  createTexture(): WebGLTexture {
    return this.nativeCall('createTexture');
  }
  cullFace(mode: number): void {
    return this.nativeCall('cullFace', [mode], {
      debug: {
        argTypes: ['constant'],
      }
    });
  }
  deleteBuffer(buffer: WebGLBuffer): void {
    return this.nativeCall('deleteBuffer', [buffer]);
  }
  deleteFramebuffer(framebuffer: WebGLFramebuffer): void {
    return this.nativeCall('deleteFramebuffer', [framebuffer]);
  }
  deleteProgram(program: WebGLProgram): void {
    return this.nativeCall('deleteProgram', [program]);
  }
  deleteRenderbuffer(renderbuffer: WebGLRenderbuffer): void {
    return this.nativeCall('deleteRenderbuffer', [renderbuffer]);
  }
  deleteShader(shader: WebGLShader): void {
    return this.nativeCall('deleteShader', [shader]);
  }
  deleteTexture(texture: WebGLTexture): void {
    return this.nativeCall('deleteTexture', [texture]);
  }
  depthFunc(func: number): void {
    return this.nativeCall('depthFunc', [func], {
      debug: {
        argTypes: ['constant'],
      }
    });
  }
  depthMask(flag: boolean): void {
    return this.nativeCall('depthMask', [flag]);
  }
  depthRange(zNear: number, zFar: number): void {
    return this.nativeCall('depthRange', [zNear, zFar]);
  }
  detachShader(program: WebGLProgram, shader: WebGLShader): void {
    return this.nativeCall('detachShader', [program, shader]);
  }
  disable(cap: number): void {
    return this.nativeCall('disable', [cap], {
      debug: {
        argTypes: ['constant'],
      }
    });
  }
  disableVertexAttribArray(index: number): void {
    return this.nativeCall('disableVertexAttribArray', [index]);
  }
  drawArrays(mode: number, first: number, count: number): void {
    return this.nativeCall('drawArrays', [mode, first, count], {
      debug: {
        argTypes: ['constant', 'default', 'default'],
      }
    });
  }
  drawElements(mode: number, count: number, type: number, offset: number): void {
    return this.nativeCall('drawElements', [mode, count, type, offset], {
      debug: {
        argTypes: ['constant', 'default', 'constant', 'default'],
      }
    });
  }
  enable(cap: number): void {
    return this.nativeCall('enable', [cap], {
      debug: {
        argTypes: ['constant'],
      }
    });
  }
  enableVertexAttribArray(index: number): void {
    return this.nativeCall('enableVertexAttribArray', [index]);
  }
  finish(): void {
    return this.nativeCall('finish');
  }
  flush(): void {
    return this.nativeCall('flush');
  }
  framebufferRenderbuffer(
    target: number,
    attachment: number,
    renderbuffertarget: number,
    renderbuffer: WebGLRenderbuffer
  ): void {
    return this.nativeCall('framebufferRenderbuffer', [target, attachment, renderbuffertarget, renderbuffer]);
  }
  framebufferTexture2D(target: number, attachment: number, textarget: number, texture: WebGLTexture, level: number): void {
    return this.nativeCall('framebufferTexture2D', [target, attachment, textarget, texture, level]);
  }
  frontFace(mode: number): void {
    return this.nativeCall('frontFace', [mode], {
      debug: {
        argTypes: ['constant'],
      }
    });
  }
  generateMipmap(target: number): void {
    return this.nativeCall('generateMipmap', [target]);
  }
  getActiveAttrib(program: WebGLProgram, index: number): WebGLActiveInfo {
    return this.nativeCall('getActiveAttrib', [program, index]);
  }
  getActiveUniform(program: WebGLProgram, index: number): WebGLActiveInfo {
    return this.nativeCall('getActiveUniform', [program, index]);
  }
  getAttachedShaders(program: WebGLProgram): WebGLShader[] {
    return this.nativeCall('getAttachedShaders', [program]);
  }
  getAttribLocation(program: WebGLProgram, name: string): number {
    return this.nativeCall('getAttribLocation', [program, name]);
  }
  getBufferParameter(target: number, pname: number) {
    return this.nativeCall('getBufferParameter', [target, pname]);
  }
  getContextAttributes(): WebGLContextAttributes {
    return {
      alpha: true,
      antialias: true,
      depth: true,
      failIfMajorPerformanceCaveat: false,
      powerPreference: 'default',
      premultipliedAlpha: false,
      preserveDrawingBuffer: false,
      stencil: false,
      xrCompatible: false,
    };
  }
  getError(): number {
    return this.nativeCall('getError');
  }
  getExtension(extensionName: 'ANGLE_instanced_arrays'): ANGLE_instanced_arrays;
  getExtension(extensionName: 'EXT_blend_minmax'): EXT_blend_minmax;
  getExtension(extensionName: 'EXT_color_buffer_float'): EXT_color_buffer_float;
  getExtension(extensionName: 'EXT_color_buffer_half_float'): EXT_color_buffer_half_float;
  getExtension(extensionName: 'EXT_float_blend'): EXT_float_blend;
  getExtension(extensionName: 'EXT_frag_depth'): EXT_frag_depth;
  getExtension(extensionName: 'EXT_sRGB'): EXT_sRGB;
  getExtension(extensionName: 'EXT_shader_texture_lod'): EXT_shader_texture_lod;
  getExtension(extensionName: 'EXT_texture_compression_bptc'): EXT_texture_compression_bptc;
  getExtension(extensionName: 'EXT_texture_compression_rgtc'): EXT_texture_compression_rgtc;
  getExtension(extensionName: 'EXT_texture_filter_anisotropic'): EXT_texture_filter_anisotropic;
  getExtension(extensionName: 'KHR_parallel_shader_compile'): KHR_parallel_shader_compile;
  getExtension(extensionName: 'OES_element_index_uint'): OES_element_index_uint;
  getExtension(extensionName: 'OES_fbo_render_mipmap'): OES_fbo_render_mipmap;
  getExtension(extensionName: 'OES_standard_derivatives'): OES_standard_derivatives;
  getExtension(extensionName: 'OES_texture_float'): OES_texture_float;
  getExtension(extensionName: 'OES_texture_float_linear'): OES_texture_float_linear;
  getExtension(extensionName: 'OES_texture_half_float'): OES_texture_half_float;
  getExtension(extensionName: 'OES_texture_half_float_linear'): OES_texture_half_float_linear;
  getExtension(extensionName: 'OES_vertex_array_object'): OES_vertex_array_object;
  getExtension(extensionName: 'OVR_multiview2'): OVR_multiview2;
  getExtension(extensionName: 'WEBGL_color_buffer_float'): WEBGL_color_buffer_float;
  getExtension(extensionName: 'WEBGL_compressed_texture_astc'): WEBGL_compressed_texture_astc;
  getExtension(extensionName: 'WEBGL_compressed_texture_etc'): WEBGL_compressed_texture_etc;
  getExtension(extensionName: 'WEBGL_compressed_texture_etc1'): WEBGL_compressed_texture_etc1;
  getExtension(extensionName: 'WEBGL_compressed_texture_pvrtc'): WEBGL_compressed_texture_pvrtc;
  getExtension(extensionName: 'WEBGL_compressed_texture_s3tc'): WEBGL_compressed_texture_s3tc;
  getExtension(extensionName: 'WEBGL_compressed_texture_s3tc_srgb'): WEBGL_compressed_texture_s3tc_srgb;
  getExtension(extensionName: 'WEBGL_debug_renderer_info'): WEBGL_debug_renderer_info;
  getExtension(extensionName: 'WEBGL_debug_shaders'): WEBGL_debug_shaders;
  getExtension(extensionName: 'WEBGL_depth_texture'): WEBGL_depth_texture;
  getExtension(extensionName: 'WEBGL_draw_buffers'): WEBGL_draw_buffers;
  getExtension(extensionName: 'WEBGL_lose_context'): WEBGL_lose_context;
  getExtension(extensionName: 'WEBGL_multi_draw'): WEBGL_multi_draw;
  getExtension(name: string);
  getExtension(extensionName: 'OCULUS_multiview'): OCULUS_multiview;
  getExtension(extensionName: unknown): any {
    logger.warn(`The extension(${extensionName}) is not supported`);
    return null;
  }
  getFramebufferAttachmentParameter(target: number, attachment: number, pname: number) {
    return this.nativeCall('getFramebufferAttachmentParameter', [target, attachment, pname]);
  }
  getParameter(pname: number) {
    try {
      const r = super.getParameter(pname);
      const pnameStr = this.#constantNamesMap.has(pname) ? `"${this.#constantNamesMap.get(pname)}(${pname})"` : `${pname}`;
      logger.warn(`Parameter(${pnameStr}) = ${r}`);
      return r;
    } catch (e) {
      throw new TypeError(`The parameter(${pname}) is not supported`);
    }
  }
  getProgramInfoLog(program: WebGLProgram): string {
    return this.nativeCall('getProgramInfoLog', [program]);
  }
  getProgramParameter(program: WebGLProgram, pname: number) {
    return this.nativeCall('getProgramParameter', [program, pname]);
  }
  getRenderbufferParameter(target: number, pname: number) {
    return this.nativeCall('getRenderbufferParameter', [target, pname]);
  }
  getShaderInfoLog(shader: WebGLShader): string {
    return this.nativeCall('getShaderInfoLog', [shader]);
  }
  getShaderParameter(shader: WebGLShader, pname: number) {
    return this.nativeCall('getShaderParameter', [shader, pname]);
  }
  getShaderPrecisionFormat(shadertype: number, precisiontype: number): WebGLShaderPrecisionFormat {
    const { rangeMin, rangeMax, precision } = super.getShaderPrecisionFormat(shadertype, precisiontype);
    return new WebGLShaderPrecisionFormatImpl(rangeMin, rangeMax, precision);
  }
  getShaderSource(shader: WebGLShader): string {
    return this.nativeCall('getShaderSource', [shader]);
  }
  getSupportedExtensions(): string[] {
    return this.nativeCall('getSupportedExtensions');
  }
  getTexParameter(target: number, pname: number) {
    return this.nativeCall('getTexParameter', [target, pname]);
  }
  getUniform(program: WebGLProgram, location: WebGLUniformLocation) {
    return this.nativeCall('getUniform', [program, location]);
  }
  getUniformLocation(program: WebGLProgram, name: string): WebGLUniformLocation {
    return this.nativeCall('getUniformLocation', [program, name]);
  }
  getVertexAttrib(index: number, pname: number) {
    return this.nativeCall('getVertexAttrib', [index, pname]);
  }
  getVertexAttribOffset(index: number, pname: number): number {
    return this.nativeCall('getVertexAttribOffset', [index, pname]);
  }
  hint(target: number, mode: number): void {
    return this.nativeCall('hint', [target, mode]);
  }
  isBuffer(buffer: WebGLBuffer): boolean {
    return this.nativeCall('isBuffer', [buffer]);
  }
  isContextLost(): boolean {
    return super.isContextLost();
  }
  isEnabled(cap: number): boolean {
    return this.nativeCall('isEnabled', [cap]);
  }
  isFramebuffer(framebuffer: WebGLFramebuffer): boolean {
    return this.nativeCall('isFramebuffer', [framebuffer]);
  }
  isProgram(program: WebGLProgram): boolean {
    return this.nativeCall('isProgram', [program]);
  }
  isRenderbuffer(renderbuffer: WebGLRenderbuffer): boolean {
    return this.nativeCall('isRenderbuffer', [renderbuffer]);
  }
  isShader(shader: WebGLShader): boolean {
    return this.nativeCall('isShader', [shader]);
  }
  isTexture(texture: WebGLTexture): boolean {
    return this.nativeCall('isTexture', [texture]);
  }
  lineWidth(width: number): void {
    return this.nativeCall('lineWidth', [width]);
  }
  linkProgram(program: WebGLProgram): void {
    return this.nativeCall('linkProgram', [program]);
  }
  pixelStorei(pname: number, param: number | boolean): void {
    return this.nativeCall('pixelStorei', [pname, param], {
      debug: {
        argTypes: ['constant', 'default'],
      }
    });
  }
  polygonOffset(factor: number, units: number): void {
    return this.nativeCall('polygonOffset', [factor, units]);
  }
  renderbufferStorage(target: number, internalformat: number, width: number, height: number): void {
    return this.nativeCall('renderbufferStorage', [target, internalformat, width, height]);
  }
  sampleCoverage(value: number, invert: boolean): void {
    return this.nativeCall('sampleCoverage', [value, invert]);
  }
  scissor(x: number, y: number, width: number, height: number): void {
    return this.nativeCall('scissor', [x, y, width, height]);
  }
  shaderSource(shader: WebGLShader, source: string): void {
    return this.nativeCall('shaderSource', [shader, source], {
      debug: {
        argTypes: [, 'ignore'],
      },
    });
  }
  stencilFunc(func: number, ref: number, mask: number): void {
    return this.nativeCall('stencilFunc', [func, ref, mask], {
      debug: {
        argTypes: ['constant'],
      },
    });
  }
  stencilFuncSeparate(face: number, func: number, ref: number, mask: number): void {
    return this.nativeCall('stencilFuncSeparate', [face, func, ref, mask]);
  }
  stencilMask(mask: number): void {
    return this.nativeCall('stencilMask', [mask]);
  }
  stencilMaskSeparate(face: number, mask: number): void {
    return this.nativeCall('stencilMaskSeparate', [face, mask]);
  }
  stencilOp(fail: number, zfail: number, zpass: number): void {
    return this.nativeCall('stencilOp', [fail, zfail, zpass]);
  }
  stencilOpSeparate(face: number, fail: number, zfail: number, zpass: number): void {
    return this.nativeCall('stencilOpSeparate', [face, fail, zfail, zpass]);
  }
  texParameterf(target: number, pname: number, param: number): void {
    return this.nativeCall('texParameterf', [target, pname, param]);
  }
  texParameteri(target: number, pname: number, param: number): void {
    return this.nativeCall('texParameteri', [target, pname, param]);
  }
  uniform1f(location: WebGLUniformLocation, x: number): void {
    return this.nativeCall('uniform1f', [location, x]);
  }
  uniform1i(location: WebGLUniformLocation, x: number): void {
    return this.nativeCall('uniform1i', [location, x]);
  }
  uniform2f(location: WebGLUniformLocation, x: number, y: number): void {
    return this.nativeCall('uniform2f', [location, x, y]);
  }
  uniform2i(location: WebGLUniformLocation, x: number, y: number): void {
    return this.nativeCall('uniform2i', [location, x, y]);
  }
  uniform3f(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    return this.nativeCall('uniform3f', [location, x, y, z]);
  }
  uniform3i(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    return this.nativeCall('uniform3i', [location, x, y, z]);
  }
  uniform4f(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    return this.nativeCall('uniform4f', [location, x, y, z, w]);
  }
  uniform4i(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    return this.nativeCall('uniform4i', [location, x, y, z, w]);
  }
  useProgram(program: WebGLProgram): void {
    return this.nativeCall('useProgram', [program]);
  }
  validateProgram(program: WebGLProgram): void {
    return this.nativeCall('validateProgram', [program]);
  }
  vertexAttrib1f(index: number, x: number): void {
    return this.nativeCall('vertexAttrib1f', [index, x]);
  }
  vertexAttrib1fv(index: number, values: Float32List): void;
  vertexAttrib1fv(index: number, values: Iterable<number>): void;
  vertexAttrib1fv(index: number, values: Float32List | Iterable<number>): void {
    return this.nativeCall('vertexAttrib1fv', [index, values]);
  }
  vertexAttrib2f(index: number, x: number, y: number): void {
    return this.nativeCall('vertexAttrib2f', [index, x, y]);
  }
  vertexAttrib2fv(index: number, values: Float32List): void;
  vertexAttrib2fv(index: number, values: Iterable<number>): void;
  vertexAttrib2fv(index: number, values: Float32List | Iterable<number>): void {
    return this.nativeCall('vertexAttrib2fv', [index, values]);
  }
  vertexAttrib3f(index: number, x: number, y: number, z: number): void {
    return this.nativeCall('vertexAttrib3f', [index, x, y, z]);
  }
  vertexAttrib3fv(index: number, values: Float32List): void;
  vertexAttrib3fv(index: number, values: Iterable<number>): void;
  vertexAttrib3fv(index: number, values: Float32List | Iterable<number>): void {
    return this.nativeCall('vertexAttrib3fv', [index, values]);
  }
  vertexAttrib4f(index: number, x: number, y: number, z: number, w: number): void {
    return this.nativeCall('vertexAttrib4f', [index, x, y, z, w]);
  }
  vertexAttrib4fv(index: number, values: Float32List): void;
  vertexAttrib4fv(index: number, values: Iterable<number>): void;
  vertexAttrib4fv(index: number, values: Float32List | Iterable<number>): void {
    return this.nativeCall('vertexAttrib4fv', [index, values]);
  }
  vertexAttribPointer(index: number, size: number, type: number, normalized: boolean, stride: number, offset: number): void {
    return this.nativeCall('vertexAttribPointer', [index, size, type, normalized, stride, offset], {
      debug: {
        argTypes: ['default', 'default', 'constant', 'default', 'default', 'default'],
      }
    });
  }
  viewport(x: number, y: number, width: number, height: number): void {
    return this.nativeCall('viewport', [x, y, width, height]);
  }
  makeXRCompatible(): Promise<void> {
    return super.makeXRCompatible();
  }
  bufferData(target: number, size: number, usage: number): void;
  bufferData(target: number, data: BufferSource, usage: number): void;
  bufferData(target: number, data: number | BufferSource, usage: number): void {
    if (typeof data === 'number') {
      throw new Error('bufferData() with size not implemented.');
    } else {
      let dataBuffer: ArrayBuffer;
      if (!(data instanceof ArrayBuffer)) {
        dataBuffer = data.buffer;
      } else {
        dataBuffer = data;
      }
      return this.nativeCall('bufferData', [target, dataBuffer, usage], {
        debug: {
          argTypes: ['constant', 'ignore', 'constant'],
        }
      });
    }
  }
  bufferSubData(target: number, offset: number, data: BufferSource): void {
    let dataBuffer: ArrayBuffer;
    if (!(data instanceof ArrayBuffer)) {
      dataBuffer = data.buffer;
    } else {
      dataBuffer = data;
    }
    return this.nativeCall('bufferSubData', [target, offset, dataBuffer], {
      debug: {
        argTypes: ['constant', 'default', 'ignore'],
      }
    });
  }
  compressedTexImage2D(
    target: number,
    level: number,
    internalformat: number,
    width: number,
    height: number,
    border: number,
    data: ArrayBufferView
  ): void {
    return this.nativeCall('compressedTexImage2D', [target, level, internalformat, width, height, border, data]);
  }
  compressedTexSubImage2D(
    target: number,
    level: number,
    xoffset: number,
    yoffset: number,
    width: number,
    height: number,
    format: number,
    data: ArrayBufferView
  ): void {
    return this.nativeCall('compressedTexSubImage2D', [
      target,
      level,
      xoffset,
      yoffset,
      width,
      height,
      format,
      data]);
  }
  readPixels(
    x: number,
    y: number,
    width: number,
    height: number,
    format: number,
    type: number,
    pixels: ArrayBufferView
  ): void {
    return this.nativeCall('readPixels', [x, y, width, height, format, type, pixels]);
  }
  texImage2D(
    target: number,
    level: number,
    internalformat: number,
    width: number,
    height: number,
    border: number,
    format: number,
    type: number,
    pixels: ArrayBufferView
  ): void;
  texImage2D(
    target: number,
    level: number,
    internalformat: number,
    format: number,
    type: number,
    source: TexImageSource
  ): void;
  texImage2D(
    target: number,
    level: number,
    internalformat: number,
    width: unknown,
    height: unknown,
    border: unknown,
    format?: number,
    type?: number,
    pixels?: ArrayBufferView
  ): void {
    if (arguments.length === 9) {
      return this.nativeCall('texImage2D', [
        target,
        level,
        internalformat,
        width as number,
        height as number,
        border as number,
        format,
        type,
        pixels
      ]);
    } else if (arguments.length === 6) {
      throw new Error('texImage2D() with TexImageSource not implemented.');
    } else {
      throw new Error('Invalid number of arguments for texImage2D()');
    }
  }
  texSubImage2D(
    target: number,
    level: number,
    xoffset: number,
    yoffset: number,
    width: number,
    height: number,
    format: number,
    type: number,
    pixels: ArrayBufferView
  ): void;
  texSubImage2D(
    target: number,
    level: number,
    xoffset: number,
    yoffset: number,
    format: number,
    type: number,
    source: TexImageSource
  ): void;
  texSubImage2D(
    target: number,
    level: number,
    xoffset: number,
    yoffset: number,
    width: unknown,
    height: unknown,
    format: unknown,
    type?: unknown,
    pixels?: unknown
  ): void {
    throw new Error('Method not implemented.');
  }
  uniform1fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform1fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform1fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform1fv', [location, v]);
  }
  uniform1iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform1iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform1iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform1iv', [location, v]);
  }
  uniform2fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform2fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform2fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform2fv', [location, v]);
  }
  uniform2iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform2iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform2iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform2iv', [location, v]);
  }
  uniform3fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform3fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform3fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform3fv', [location, v]);
  }
  uniform3iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform3iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform3iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform3iv', [location, v]);
  }
  uniform4fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform4fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform4fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform4fv', [location, v]);
  }
  uniform4iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform4iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform4iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    return this.nativeCall('uniform4iv', [location, v]);
  }
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List | Iterable<number>): void {
    return this.nativeCall('uniformMatrix2fv', [location, transpose, value]);
  }
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List | Iterable<number>): void {
    return this.nativeCall('uniformMatrix3fv', [location, transpose, value]);
  }
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List | Iterable<number>): void {
    return this.nativeCall('uniformMatrix4fv', [location, transpose, value]);
  }
}
