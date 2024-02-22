import * as logger from '../bindings/logger';
import { WebGLShaderPrecisionFormatImpl } from './WebGLShaderPrecisionFormat';

const glNative = process._linkedBinding('transmute:webgl');

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

  activeTexture(texture: number): void {
    super.activeTexture(texture);
  }
  attachShader(program: WebGLProgram, shader: WebGLShader): void {
    super.attachShader(program, shader);
  }
  bindAttribLocation(program: WebGLProgram, index: number, name: string): void {
    super.bindAttribLocation(program, index, name);
  }
  bindBuffer(target: number, buffer: WebGLBuffer): void {
    super.bindBuffer(target, buffer);
  }
  bindFramebuffer(target: number, framebuffer: WebGLFramebuffer): void {
    super.bindFramebuffer(target, framebuffer);
  }
  bindRenderbuffer(target: number, renderbuffer: WebGLRenderbuffer): void {
    super.bindRenderbuffer(target, renderbuffer);
  }
  bindTexture(target: number, texture: WebGLTexture): void {
    super.bindTexture(target, texture);
  }
  blendColor(red: number, green: number, blue: number, alpha: number): void {
    super.blendColor(red, green, blue, alpha);
  }
  blendEquation(mode: number): void {
    super.blendEquation(mode)
  }
  blendEquationSeparate(modeRGB: number, modeAlpha: number): void {
    super.blendEquationSeparate(modeRGB, modeAlpha)
  }
  blendFunc(sfactor: number, dfactor: number): void {
    super.blendFunc(sfactor, dfactor);
  }
  blendFuncSeparate(srcRGB: number, dstRGB: number, srcAlpha: number, dstAlpha: number): void {
    super.blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha)
  }
  checkFramebufferStatus(target: number): number {
    return super.checkFramebufferStatus(target);
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
    super.colorMask(red, green, blue, alpha);
  }
  compileShader(shader: WebGLShader): void {
    super.compileShader(shader);
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
    super.copyTexImage2D(target, level, internalformat, x, y, width, height, border);
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
    super.copyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
  }
  createBuffer(): WebGLBuffer {
    return super.createBuffer();
  }
  createFramebuffer(): WebGLFramebuffer {
    return super.createFramebuffer();
  }
  createProgram(): WebGLProgram {
    return super.createProgram();
  }
  createRenderbuffer(): WebGLRenderbuffer {
    return super.createRenderbuffer();
  }
  createShader(type: number): WebGLShader {
    return super.createShader(type);
  }
  createTexture(): WebGLTexture {
    return super.createTexture();
  }
  cullFace(mode: number): void {
    super.cullFace(mode);
  }
  deleteBuffer(buffer: WebGLBuffer): void {
    super.deleteBuffer(buffer);
  }
  deleteFramebuffer(framebuffer: WebGLFramebuffer): void {
    super.deleteFramebuffer(framebuffer);
  }
  deleteProgram(program: WebGLProgram): void {
    super.deleteProgram(program);
  }
  deleteRenderbuffer(renderbuffer: WebGLRenderbuffer): void {
    super.deleteRenderbuffer(renderbuffer);
  }
  deleteShader(shader: WebGLShader): void {
    super.deleteShader(shader);
  }
  deleteTexture(texture: WebGLTexture): void {
    super.deleteTexture(texture);
  }
  depthFunc(func: number): void {
    super.depthFunc(func);
  }
  depthMask(flag: boolean): void {
    super.depthMask(flag);
  }
  depthRange(zNear: number, zFar: number): void {
    super.depthRange(zNear, zFar);
  }
  detachShader(program: WebGLProgram, shader: WebGLShader): void {
    super.detachShader(program, shader);
  }
  disable(cap: number): void {
    super.disable(cap);
  }
  disableVertexAttribArray(index: number): void {
    super.disableVertexAttribArray(index);
  }
  drawArrays(mode: number, first: number, count: number): void {
    super.drawArrays(mode, first, count);
  }
  drawElements(mode: number, count: number, type: number, offset: number): void {
    super.drawElements(mode, count, type, offset);
  }
  enable(cap: number): void {
    super.enable(cap);
  }
  enableVertexAttribArray(index: number): void {
    super.enableVertexAttribArray(index);
  }
  finish(): void {
    super.finish();
  }
  flush(): void {
    super.flush();
  }
  framebufferRenderbuffer(
    target: number,
    attachment: number,
    renderbuffertarget: number,
    renderbuffer: WebGLRenderbuffer
  ): void {
    super.framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  }
  framebufferTexture2D(target: number, attachment: number, textarget: number, texture: WebGLTexture, level: number): void {
    super.framebufferTexture2D(target, attachment, textarget, texture, level);
  }
  frontFace(mode: number): void {
    super.frontFace(mode);
  }
  generateMipmap(target: number): void {
    super.generateMipmap(target);
  }
  getActiveAttrib(program: WebGLProgram, index: number): WebGLActiveInfo {
    return super.getActiveAttrib(program, index);
  }
  getActiveUniform(program: WebGLProgram, index: number): WebGLActiveInfo {
    return super.getActiveUniform(program, index);
  }
  getAttachedShaders(program: WebGLProgram): WebGLShader[] {
    return super.getAttachedShaders(program);
  }
  getAttribLocation(program: WebGLProgram, name: string): number {
    return super.getAttribLocation(program, name);
  }
  getBufferParameter(target: number, pname: number) {
    return super.getBufferParameter(target, pname);
  }
  getContextAttributes(): WebGLContextAttributes {
    return {
      alpha: true,
      antialias: true,
      depth: true,
      failIfMajorPerformanceCaveat: false,
      powerPreference: 'default',
      premultipliedAlpha: true,
      preserveDrawingBuffer: false,
      stencil: false,
      xrCompatible: false,
    };
  }
  getError(): number {
    return super.getError();
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
    return super.getFramebufferAttachmentParameter(target, attachment, pname);
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
    return super.getProgramInfoLog(program);
  }
  getProgramParameter(program: WebGLProgram, pname: number) {
    return super.getProgramParameter(program, pname);
  }
  getRenderbufferParameter(target: number, pname: number) {
    return super.getRenderbufferParameter(target, pname);
  }
  getShaderInfoLog(shader: WebGLShader): string {
    return super.getShaderInfoLog(shader);
  }
  getShaderParameter(shader: WebGLShader, pname: number) {
    return super.getShaderParameter(shader, pname);
  }
  getShaderPrecisionFormat(shadertype: number, precisiontype: number): WebGLShaderPrecisionFormat {
    const { rangeMin, rangeMax, precision } = super.getShaderPrecisionFormat(shadertype, precisiontype);
    return new WebGLShaderPrecisionFormatImpl(rangeMin, rangeMax, precision);
  }
  getShaderSource(shader: WebGLShader): string {
    return super.getShaderSource(shader);
  }
  getSupportedExtensions(): string[] {
    return super.getSupportedExtensions();
  }
  getTexParameter(target: number, pname: number) {
    return super.getTexParameter(target, pname);
  }
  getUniform(program: WebGLProgram, location: WebGLUniformLocation) {
    return super.getUniform(program, location);
  }
  getUniformLocation(program: WebGLProgram, name: string): WebGLUniformLocation {
    return super.getUniformLocation(program, name);
  }
  getVertexAttrib(index: number, pname: number) {
    return super.getVertexAttrib(index, pname);
  }
  getVertexAttribOffset(index: number, pname: number): number {
    return super.getVertexAttribOffset(index, pname);
  }
  hint(target: number, mode: number): void {
    super.hint(target, mode);
  }
  isBuffer(buffer: WebGLBuffer): boolean {
    return super.isBuffer(buffer);
  }
  isContextLost(): boolean {
    return super.isContextLost();
  }
  isEnabled(cap: number): boolean {
    return super.isEnabled(cap);
  }
  isFramebuffer(framebuffer: WebGLFramebuffer): boolean {
    return super.isFramebuffer(framebuffer);
  }
  isProgram(program: WebGLProgram): boolean {
    return super.isProgram(program);
  }
  isRenderbuffer(renderbuffer: WebGLRenderbuffer): boolean {
    return super.isRenderbuffer(renderbuffer);
  }
  isShader(shader: WebGLShader): boolean {
    return super.isShader(shader);
  }
  isTexture(texture: WebGLTexture): boolean {
    return super.isTexture(texture);
  }
  lineWidth(width: number): void {
    super.lineWidth(width);
  }
  linkProgram(program: WebGLProgram): void {
    super.linkProgram(program);
  }
  pixelStorei(pname: number, param: number | boolean): void {
    super.pixelStorei(pname, param);
  }
  polygonOffset(factor: number, units: number): void {
    super.polygonOffset(factor, units);
  }
  renderbufferStorage(target: number, internalformat: number, width: number, height: number): void {
    super.renderbufferStorage(target, internalformat, width, height);
  }
  sampleCoverage(value: number, invert: boolean): void {
    super.sampleCoverage(value, invert);
  }
  scissor(x: number, y: number, width: number, height: number): void {
    super.scissor(x, y, width, height);
  }
  shaderSource(shader: WebGLShader, source: string): void {
    super.shaderSource(shader, source);
  }
  stencilFunc(func: number, ref: number, mask: number): void {
    super.stencilFunc(func, ref, mask);
  }
  stencilFuncSeparate(face: number, func: number, ref: number, mask: number): void {
    super.stencilFuncSeparate(face, func, ref, mask);
  }
  stencilMask(mask: number): void {
    super.stencilMask(mask);
  }
  stencilMaskSeparate(face: number, mask: number): void {
    super.stencilMaskSeparate(face, mask);
  }
  stencilOp(fail: number, zfail: number, zpass: number): void {
    super.stencilOp(fail, zfail, zpass);
  }
  stencilOpSeparate(face: number, fail: number, zfail: number, zpass: number): void {
    super.stencilOpSeparate(face, fail, zfail, zpass);
  }
  texParameterf(target: number, pname: number, param: number): void {
    super.texParameterf(target, pname, param);
  }
  texParameteri(target: number, pname: number, param: number): void {
    super.texParameteri(target, pname, param);
  }
  uniform1f(location: WebGLUniformLocation, x: number): void {
    super.uniform1f(location, x);
  }
  uniform1i(location: WebGLUniformLocation, x: number): void {
    super.uniform1i(location, x);
  }
  uniform2f(location: WebGLUniformLocation, x: number, y: number): void {
    super.uniform2f(location, x, y);
  }
  uniform2i(location: WebGLUniformLocation, x: number, y: number): void {
    super.uniform2i(location, x, y);
  }
  uniform3f(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    super.uniform3f(location, x, y, z);
  }
  uniform3i(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    super.uniform3i(location, x, y, z);
  }
  uniform4f(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    super.uniform4f(location, x, y, z, w);
  }
  uniform4i(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    super.uniform4i(location, x, y, z, w);
  }
  useProgram(program: WebGLProgram): void {
    super.useProgram(program);
  }
  validateProgram(program: WebGLProgram): void {
    super.validateProgram(program);
  }
  vertexAttrib1f(index: number, x: number): void {
    super.vertexAttrib1f(index, x);
  }
  vertexAttrib1fv(index: number, values: Float32List): void;
  vertexAttrib1fv(index: number, values: Iterable<number>): void;
  vertexAttrib1fv(index: number, values: Float32List | Iterable<number>): void {
    super.vertexAttrib1fv(index, values);
  }
  vertexAttrib2f(index: number, x: number, y: number): void {
    super.vertexAttrib2f(index, x, y);
  }
  vertexAttrib2fv(index: number, values: Float32List): void;
  vertexAttrib2fv(index: number, values: Iterable<number>): void;
  vertexAttrib2fv(index: number, values: Float32List | Iterable<number>): void {
    super.vertexAttrib2fv(index, values);
  }
  vertexAttrib3f(index: number, x: number, y: number, z: number): void {
    super.vertexAttrib3f(index, x, y, z);
  }
  vertexAttrib3fv(index: number, values: Float32List): void;
  vertexAttrib3fv(index: number, values: Iterable<number>): void;
  vertexAttrib3fv(index: number, values: Float32List | Iterable<number>): void {
    super.vertexAttrib3fv(index, values);
  }
  vertexAttrib4f(index: number, x: number, y: number, z: number, w: number): void {
    super.vertexAttrib4f(index, x, y, z, w);
  }
  vertexAttrib4fv(index: number, values: Float32List): void;
  vertexAttrib4fv(index: number, values: Iterable<number>): void;
  vertexAttrib4fv(index: number, values: Float32List | Iterable<number>): void {
    super.vertexAttrib4fv(index, values);
  }
  vertexAttribPointer(index: number, size: number, type: number, normalized: boolean, stride: number, offset: number): void {
    super.vertexAttribPointer(index, size, type, normalized, stride, offset);
  }
  viewport(x: number, y: number, width: number, height: number): void {
    super.viewport(x, y, width, height);
  }
  makeXRCompatible(): Promise<void> {
    return super.makeXRCompatible();
  }
  bufferData(target: number, size: number, usage: number): void;
  bufferData(target: number, data: BufferSource, usage: number): void;
  bufferData(target: number, data: number | BufferSource, usage: number): void {
    if (typeof data === 'number') {
      throw new Error('BufferData with size not implemented.');
    } else {
      // convert to arraybuffer if not
      if (!(data instanceof ArrayBuffer)) {
        super.bufferData(target, data.buffer, usage);
      } else {
        super.bufferData(target, data, usage);
      }
    }
  }
  bufferSubData(target: number, offset: number, data: BufferSource): void {
    super.bufferSubData(target, offset, data);
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
    super.compressedTexImage2D(target, level, internalformat, width, height, border, data);
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
    super.compressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, data);
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
    return super.readPixels(x, y, width, height, format, type, pixels);
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
      super.texImage2D(
        target,
        level,
        internalformat,
        width as number,
        height as number,
        border as number,
        format,
        type,
        pixels);
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
    super.uniform1fv(location, v);
  }
  uniform1iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform1iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform1iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform1iv(location, v);
  }
  uniform2fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform2fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform2fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform2fv(location, v);
  }
  uniform2iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform2iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform2iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform2iv(location, v);
  }
  uniform3fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform3fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform3fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform3fv(location, v);
  }
  uniform3iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform3iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform3iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform3iv(location, v);
  }
  uniform4fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform4fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform4fv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform4fv(location, v);
  }
  uniform4iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform4iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform4iv(location: WebGLUniformLocation, v: Float32List | Iterable<number>): void {
    super.uniform4iv(location, v);
  }
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List | Iterable<number>): void {
    super.uniformMatrix2fv(location, transpose, value);
  }
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List | Iterable<number>): void {
    super.uniformMatrix3fv(location, transpose, value);
  }
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List | Iterable<number>): void {
    super.uniformMatrix4fv(location, transpose, value);
  }
}
