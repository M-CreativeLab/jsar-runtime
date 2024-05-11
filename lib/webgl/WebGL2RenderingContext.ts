import * as logger from '@transmute/logger';
import WebGLRenderingContextImpl from './WebGLRenderingContext';
import { WebGLShaderPrecisionFormatImpl } from './WebGLShaderPrecisionFormat';
import { setupConstantNamesMap, makeNativeCall, type NativeCallOptions, getTextureParametersFromImageSource, isTexImageSource, getPixelsFromTexImageSource, isTypedArray, unpackTypedArray } from './utils';
const glNative = process._linkedBinding('transmute:webgl');

class WebGL2RenderingContextImpl extends glNative.WebGL2RenderingContext implements WebGL2RenderingContext {
  constructor(_canvas: HTMLCanvasElement | OffscreenCanvas, options?: WebGLContextAttributes) {
    super(options);
    setupConstantNamesMap(this, glNative.WebGL2RenderingContext);
  }
  private nativeCall(name: string, args: any[] = [], options: NativeCallOptions = {}) {
    return makeNativeCall.call(this, <Function>super[name], name, args, options);
  }

  beginQuery(target: number, query: WebGLQuery): void {
    this.nativeCall('beginQuery', [target, query]);
  }
  beginTransformFeedback(primitiveMode: number): void {
    this.nativeCall('beginTransformFeedback', [primitiveMode]);
  }
  bindBufferBase(target: number, index: number, buffer: WebGLBuffer): void {
    this.nativeCall('bindBufferBase', [target, index, buffer]);
  }
  bindBufferRange(target: number, index: number, buffer: WebGLBuffer, offset: number, size: number): void {
    this.nativeCall('bindBufferRange', [target, index, buffer, offset, size]);
  }
  bindSampler(unit: number, sampler: WebGLSampler): void {
    this.nativeCall('bindSampler', [unit, sampler]);
  }
  bindTransformFeedback(target: number, tf: WebGLTransformFeedback): void {
    this.nativeCall('bindTransformFeedback', [target, tf]);
  }
  bindVertexArray(array: WebGLVertexArrayObject): void {
    this.nativeCall('bindVertexArray', [array]);
  }
  blitFramebuffer(srcX0: number, srcY0: number, srcX1: number, srcY1: number, dstX0: number, dstY0: number, dstX1: number, dstY1: number, mask: number, filter: number): void {
    this.nativeCall('blitFramebuffer', [srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter]);
  }
  clearBufferfi(buffer: number, drawbuffer: number, depth: number, stencil: number): void {
    this.nativeCall('clearBufferfi', [buffer, drawbuffer, depth, stencil]);
  }
  clearBufferfv(buffer: number, drawbuffer: number, values: Float32List, srcOffset?: number): void;
  clearBufferfv(buffer: number, drawbuffer: number, values: Iterable<number>, srcOffset?: number): void;
  clearBufferfv(buffer: unknown, drawbuffer: unknown, values: unknown, srcOffset?: unknown): void {
    this.nativeCall('clearBufferfv', [buffer, drawbuffer, values, srcOffset]);
  }
  clearBufferiv(buffer: number, drawbuffer: number, values: Int32List, srcOffset?: number): void;
  clearBufferiv(buffer: number, drawbuffer: number, values: Iterable<number>, srcOffset?: number): void;
  clearBufferiv(buffer: unknown, drawbuffer: unknown, values: unknown, srcOffset?: unknown): void {
    this.nativeCall('clearBufferiv', [buffer, drawbuffer, values, srcOffset]);
  }
  clearBufferuiv(buffer: number, drawbuffer: number, values: Uint32List, srcOffset?: number): void;
  clearBufferuiv(buffer: number, drawbuffer: number, values: Iterable<number>, srcOffset?: number): void;
  clearBufferuiv(buffer: unknown, drawbuffer: unknown, values: unknown, srcOffset?: unknown): void {
    this.nativeCall('clearBufferuiv', [buffer, drawbuffer, values, srcOffset]);
  }
  clientWaitSync(sync: WebGLSync, flags: number, timeout: number): number {
    return this.nativeCall('clientWaitSync', [sync, flags, timeout]);
  }
  compressedTexImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, imageSize: number, offset: number): void;
  compressedTexImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, srcData: ArrayBufferView, srcOffset?: number, srcLengthOverride?: number): void;
  compressedTexImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, srcData: unknown, srcOffset?: unknown, srcLengthOverride?: unknown): void {
    this.nativeCall('compressedTexImage3D', [target, level, internalformat, width, height, depth, border, srcData, srcOffset, srcLengthOverride]);
  }
  compressedTexSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, width: number, height: number, depth: number, format: number, imageSize: number, offset: number): void;
  compressedTexSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, width: number, height: number, depth: number, format: number, srcData: ArrayBufferView, srcOffset?: number, srcLengthOverride?: number): void;
  compressedTexSubImage3D(target: unknown, level: unknown, xoffset: unknown, yoffset: unknown, zoffset: unknown, width: unknown, height: unknown, depth: unknown, format: unknown, srcData: unknown, srcOffset?: unknown, srcLengthOverride?: unknown): void {
    this.nativeCall('compressedTexSubImage3D', [target, level, xoffset, yoffset, zoffset, width, height, depth, format, srcData, srcOffset, srcLengthOverride]);
  }
  copyBufferSubData(readTarget: number, writeTarget: number, readOffset: number, writeOffset: number, size: number): void {
    this.nativeCall('copyBufferSubData', [readTarget, writeTarget, readOffset, writeOffset, size]);
  }
  copyTexSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, x: number, y: number, width: number, height: number): void {
    this.nativeCall('copyTexSubImage3D', [target, level, xoffset, yoffset, zoffset, x, y, width, height]);
  }
  createQuery(): WebGLQuery {
    return this.nativeCall('createQuery');
  }
  createSampler(): WebGLSampler {
    return this.nativeCall('createSampler');
  }
  createTransformFeedback(): WebGLTransformFeedback {
    return this.nativeCall('createTransformFeedback');
  }
  createVertexArray(): WebGLVertexArrayObject {
    return this.nativeCall('createVertexArray');
  }
  deleteQuery(query: WebGLQuery): void {
    return this.nativeCall('deleteQuery', [query]);
  }
  deleteSampler(sampler: WebGLSampler): void {
    return this.nativeCall('deleteSampler', [sampler]);
  }
  deleteSync(sync: WebGLSync): void {
    return this.nativeCall('deleteSync', [sync]);
  }
  deleteTransformFeedback(tf: WebGLTransformFeedback): void {
    return this.nativeCall('deleteTransformFeedback', [tf]);
  }
  deleteVertexArray(vertexArray: WebGLVertexArrayObject): void {
    return this.nativeCall('deleteVertexArray', [vertexArray]);
  }
  drawArraysInstanced(mode: number, first: number, count: number, instanceCount: number): void {
    return this.nativeCall('drawArraysInstanced', [mode, first, count, instanceCount]);
  }
  drawBuffers(buffers: number[]): void;
  drawBuffers(buffers: Iterable<number>): void;
  drawBuffers(buffers: unknown): void {
    return this.nativeCall('drawBuffers', [buffers]);
  }
  drawElementsInstanced(mode: number, count: number, type: number, offset: number, instanceCount: number): void {
    return this.nativeCall('drawElementsInstanced', [mode, count, type, offset, instanceCount]);
  }
  drawRangeElements(mode: number, start: number, end: number, count: number, type: number, offset: number): void {
    return this.nativeCall('drawRangeElements', [mode, start, end, count, type, offset]);
  }
  endQuery(target: number): void {
    return this.nativeCall('endQuery', [target]);
  }
  endTransformFeedback(): void {
    return this.nativeCall('endTransformFeedback');
  }
  fenceSync(condition: number, flags: number): WebGLSync {
    return this.nativeCall('fenceSync', [condition, flags]);
  }
  framebufferTextureLayer(target: number, attachment: number, texture: WebGLTexture, level: number, layer: number): void {
    return this.nativeCall('framebufferTextureLayer', [target, attachment, texture, level, layer]);
  }
  getActiveUniformBlockName(program: WebGLProgram, uniformBlockIndex: number): string {
    return this.nativeCall('getActiveUniformBlockName', [program, uniformBlockIndex]);
  }
  getActiveUniformBlockParameter(program: WebGLProgram, uniformBlockIndex: number, pname: number) {
    return this.nativeCall('getActiveUniformBlockParameter', [program, uniformBlockIndex, pname]);
  }
  getActiveUniforms(program: WebGLProgram, uniformIndices: number[], pname: number);
  getActiveUniforms(program: WebGLProgram, uniformIndices: Iterable<number>, pname: number);
  getActiveUniforms(program: unknown, uniformIndices: unknown, pname: unknown): any {
    return this.nativeCall('getActiveUniforms', [program, uniformIndices, pname]);
  }
  getBufferSubData(target: number, srcByteOffset: number, dstBuffer: ArrayBufferView, dstOffset?: number, length?: number): void {
    return this.nativeCall('getBufferSubData', [target, srcByteOffset, dstBuffer, dstOffset, length]);
  }
  getFragDataLocation(program: WebGLProgram, name: string): number {
    return this.nativeCall('getFragDataLocation', [program, name]);
  }
  getIndexedParameter(target: number, index: number) {
    return this.nativeCall('getIndexedParameter', [target, index]);
  }
  getInternalformatParameter(target: number, internalformat: number, pname: number) {
    return this.nativeCall('getInternalformatParameter', [target, internalformat, pname]);
  }
  getQuery(target: number, pname: number): WebGLQuery {
    return this.nativeCall('getQuery', [target, pname]);
  }
  getQueryParameter(query: WebGLQuery, pname: number) {
    return this.nativeCall('getQueryParameter', [query, pname]);
  }
  getSamplerParameter(sampler: WebGLSampler, pname: number) {
    return this.nativeCall('getSamplerParameter', [sampler, pname]);
  }
  getSyncParameter(sync: WebGLSync, pname: number) {
    return this.nativeCall('getSyncParameter', [sync, pname]);
  }
  getTransformFeedbackVarying(program: WebGLProgram, index: number): WebGLActiveInfo {
    return this.nativeCall('getTransformFeedbackVarying', [program, index]);
  }
  getUniformBlockIndex(program: WebGLProgram, uniformBlockName: string): number {
    return this.nativeCall('getUniformBlockIndex', [program, uniformBlockName]);
  }
  getUniformIndices(program: WebGLProgram, uniformNames: string[]): number[];
  getUniformIndices(program: WebGLProgram, uniformNames: Iterable<string>): Iterable<number>;
  getUniformIndices(program: unknown, uniformNames: unknown): Iterable<number> | number[] {
    return this.nativeCall('getUniformIndices', [program, uniformNames]);
  }
  invalidateFramebuffer(target: number, attachments: number[]): void;
  invalidateFramebuffer(target: number, attachments: Iterable<number>): void;
  invalidateFramebuffer(target: unknown, attachments: unknown): void {
    return this.nativeCall('invalidateFramebuffer', [target, attachments]);
  }
  invalidateSubFramebuffer(target: number, attachments: number[], x: number, y: number, width: number, height: number): void;
  invalidateSubFramebuffer(target: number, attachments: Iterable<number>, x: number, y: number, width: number, height: number): void;
  invalidateSubFramebuffer(target: unknown, attachments: unknown, x: unknown, y: unknown, width: unknown, height: unknown): void {
    return this.nativeCall('invalidateSubFramebuffer', [target, attachments, x, y, width, height]);
  }
  isQuery(query: WebGLQuery): boolean {
    return this.nativeCall('isQuery', [query]);
  }
  isSampler(sampler: WebGLSampler): boolean {
    return this.nativeCall('isSampler', [sampler]);
  }
  isSync(sync: WebGLSync): boolean {
    return this.nativeCall('isSync', [sync]);
  }
  isTransformFeedback(tf: WebGLTransformFeedback): boolean {
    return this.nativeCall('isTransformFeedback', [tf]);
  }
  isVertexArray(vertexArray: WebGLVertexArrayObject): boolean {
    return this.nativeCall('isVertexArray', [vertexArray]);
  }
  pauseTransformFeedback(): void {
    return this.nativeCall('pauseTransformFeedback');
  }
  readBuffer(src: number): void {
    return this.nativeCall('readBuffer', [src]);
  }
  renderbufferStorageMultisample(target: number, samples: number, internalformat: number, width: number, height: number): void {
    return this.nativeCall('renderbufferStorageMultisample', [target, samples, internalformat, width, height]);
  }
  resumeTransformFeedback(): void {
    return this.nativeCall('resumeTransformFeedback');
  }
  samplerParameterf(sampler: WebGLSampler, pname: number, param: number): void {
    return this.nativeCall('samplerParameterf', [sampler, pname, param]);
  }
  samplerParameteri(sampler: WebGLSampler, pname: number, param: number): void {
    return this.nativeCall('samplerParameteri', [sampler, pname, param]);
  }
  texImage2D(
    target: number,
    level: number,
    internalformat: number,
    width: unknown,
    height: unknown,
    border: unknown,
    format?: unknown,
    type?: unknown,
    pixels?: unknown,
    srcOffset?: unknown
  ): void {
    const callOptions = <NativeCallOptions>{
      debug: {
        argTypes: ['constant', , 'constant', , , , 'constant', 'constant'],
      }
    };
    if (arguments.length === 9) {
      if (pixels instanceof ArrayBuffer) {
        pixels = new Uint8Array(pixels);
      }
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
      ], callOptions);
    } else if (arguments.length === 6) {
      const params = getTextureParametersFromImageSource.apply(this, arguments);
      return this.nativeCall('texImage2D', [
        target,
        level,
        internalformat,
        params.width,
        params.height,
        0,
        params.format,
        params.type,
        params.pixels,
      ], callOptions);
    } else {
      throw new Error('Invalid number of arguments for texImage2D()');
    }
  }
  texImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, format: number, type: number, pboOffset: number): void;
  texImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, format: number, type: number, source: TexImageSource): void;
  texImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, format: number, type: number, srcData: ArrayBufferView): void;
  texImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, format: number, type: number, srcData: ArrayBufferView, srcOffset: number): void;
  texImage3D(target: number, level: number, internalformat: number, width: number, height: number, depth: number, border: number, format: number, type: number, srcData: unknown, srcOffset?: unknown): void {
    if (isTexImageSource(srcData)) {
      const pixels = getPixelsFromTexImageSource(format === this.RGB ? 'rgb8' : 'rgba8', srcData);
      return this.nativeCall('texImage3D', [
        target,
        level,
        internalformat,
        width,
        height,
        depth,
        border,
        format,
        type,
        pixels.data,
      ]);
    } else {
      return this.nativeCall('texImage3D', [
        target, level, internalformat, width, height, depth, border, format, type, srcData, srcOffset]);
    }
  }
  texStorage2D(target: number, levels: number, internalformat: number, width: number, height: number): void {
    return this.nativeCall('texStorage2D', [target, levels, internalformat, width, height]);
  }
  texStorage3D(target: number, levels: number, internalformat: number, width: number, height: number, depth: number): void {
    return this.nativeCall('texStorage3D', [target, levels, internalformat, width, height, depth]);
  }
  texSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, width: number, height: number, depth: number, format: number, type: number, pboOffset: number): void;
  texSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, width: number, height: number, depth: number, format: number, type: number, source: TexImageSource): void;
  texSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, width: number, height: number, depth: number, format: number, type: number, srcData: ArrayBufferView, srcOffset?: number): void;
  texSubImage3D(target: number, level: number, xoffset: number, yoffset: number, zoffset: number, width: number, height: number, depth: number, format: number, type: number, srcData: unknown, srcOffset?: unknown): void {
    if (isTexImageSource(srcData)) {
      const pixels = getPixelsFromTexImageSource(format === this.RGB ? 'rgb8' : 'rgba8', srcData);
      return this.nativeCall('texSubImage3D', [
        target,
        level,
        xoffset,
        yoffset,
        zoffset,
        width,
        height,
        depth,
        format,
        type,
        pixels.data,
      ]);
    } else {
      return this.nativeCall('texSubImage3D', [
        target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, srcData, srcOffset]);
    }
  }
  transformFeedbackVaryings(program: WebGLProgram, varyings: string[], bufferMode: number): void;
  transformFeedbackVaryings(program: WebGLProgram, varyings: Iterable<string>, bufferMode: number): void;
  transformFeedbackVaryings(program: unknown, varyings: unknown, bufferMode: unknown): void {
    return this.nativeCall('transformFeedbackVaryings', [program, varyings, bufferMode]);
  }
  uniform1ui(location: WebGLUniformLocation, v0: number): void {
    return this.nativeCall('uniform1ui', [location, v0]);
  }
  uniform1uiv(location: WebGLUniformLocation, data: Uint32List, srcOffset?: number, srcLength?: number): void;
  uniform1uiv(location: WebGLUniformLocation, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniform1uiv(location: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniform1uiv', [location, data, srcOffset, srcLength]);
  }
  uniform2ui(location: WebGLUniformLocation, v0: number, v1: number): void {
    return this.nativeCall('uniform2ui', [location, v0, v1]);
  }
  uniform2uiv(location: WebGLUniformLocation, data: Uint32List, srcOffset?: number, srcLength?: number): void;
  uniform2uiv(location: WebGLUniformLocation, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniform2uiv(location: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniform2uiv', [location, data, srcOffset, srcLength]);
  }
  uniform3ui(location: WebGLUniformLocation, v0: number, v1: number, v2: number): void {
    return this.nativeCall('uniform3ui', [location, v0, v1, v2]);
  }
  uniform3uiv(location: WebGLUniformLocation, data: Uint32List, srcOffset?: number, srcLength?: number): void;
  uniform3uiv(location: WebGLUniformLocation, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniform3uiv(location: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniform3uiv', [location, data, srcOffset, srcLength]);
  }
  uniform4ui(location: WebGLUniformLocation, v0: number, v1: number, v2: number, v3: number): void {
    return this.nativeCall('uniform4ui', [location, v0, v1, v2, v3]);
  }
  uniform4uiv(location: WebGLUniformLocation, data: Uint32List, srcOffset?: number, srcLength?: number): void;
  uniform4uiv(location: WebGLUniformLocation, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniform4uiv(location: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniform4uiv', [location, data, srcOffset, srcLength]);
  }
  uniformBlockBinding(program: WebGLProgram, uniformBlockIndex: number, uniformBlockBinding: number): void {
    return this.nativeCall('uniformBlockBinding', [program, uniformBlockIndex, uniformBlockBinding]);
  }
  uniformMatrix2x3fv(location: WebGLUniformLocation, transpose: boolean, data: Float32List, srcOffset?: number, srcLength?: number): void;
  uniformMatrix2x3fv(location: WebGLUniformLocation, transpose: boolean, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniformMatrix2x3fv(location: unknown, transpose: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniformMatrix2x3fv', [location, transpose, data, srcOffset, srcLength]);
  }
  uniformMatrix2x4fv(location: WebGLUniformLocation, transpose: boolean, data: Float32List, srcOffset?: number, srcLength?: number): void;
  uniformMatrix2x4fv(location: WebGLUniformLocation, transpose: boolean, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniformMatrix2x4fv(location: unknown, transpose: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniformMatrix2x4fv', [location, transpose, data, srcOffset, srcLength]);
  }
  uniformMatrix3x2fv(location: WebGLUniformLocation, transpose: boolean, data: Float32List, srcOffset?: number, srcLength?: number): void;
  uniformMatrix3x2fv(location: WebGLUniformLocation, transpose: boolean, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniformMatrix3x2fv(location: unknown, transpose: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniformMatrix3x2fv', [location, transpose, data, srcOffset, srcLength]);
  }
  uniformMatrix3x4fv(location: WebGLUniformLocation, transpose: boolean, data: Float32List, srcOffset?: number, srcLength?: number): void;
  uniformMatrix3x4fv(location: WebGLUniformLocation, transpose: boolean, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniformMatrix3x4fv(location: unknown, transpose: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniformMatrix3x4fv', [location, transpose, data, srcOffset, srcLength]);
  }
  uniformMatrix4x2fv(location: WebGLUniformLocation, transpose: boolean, data: Float32List, srcOffset?: number, srcLength?: number): void;
  uniformMatrix4x2fv(location: WebGLUniformLocation, transpose: boolean, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniformMatrix4x2fv(location: unknown, transpose: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniformMatrix4x2fv', [location, transpose, data, srcOffset, srcLength]);
  }
  uniformMatrix4x3fv(location: WebGLUniformLocation, transpose: boolean, data: Float32List, srcOffset?: number, srcLength?: number): void;
  uniformMatrix4x3fv(location: WebGLUniformLocation, transpose: boolean, data: Iterable<number>, srcOffset?: number, srcLength?: number): void;
  uniformMatrix4x3fv(location: unknown, transpose: unknown, data: unknown, srcOffset?: unknown, srcLength?: unknown): void {
    return this.nativeCall('uniformMatrix4x3fv', [location, transpose, data, srcOffset, srcLength]);
  }
  vertexAttribDivisor(index: number, divisor: number): void {
    return this.nativeCall('vertexAttribDivisor', [index, divisor]);
  }
  vertexAttribI4i(index: number, x: number, y: number, z: number, w: number): void {
    return this.nativeCall('vertexAttribI4i', [index, x, y, z, w]);
  }
  vertexAttribI4iv(index: number, values: Int32List): void;
  vertexAttribI4iv(index: number, values: Iterable<number>): void;
  vertexAttribI4iv(index: unknown, values: unknown): void {
    return this.nativeCall('vertexAttribI4iv', [index, values]);
  }
  vertexAttribI4ui(index: number, x: number, y: number, z: number, w: number): void {
    return this.nativeCall('vertexAttribI4ui', [index, x, y, z, w]);
  }
  vertexAttribI4uiv(index: number, values: Uint32List): void;
  vertexAttribI4uiv(index: number, values: Iterable<number>): void;
  vertexAttribI4uiv(index: unknown, values: unknown): void {
    return this.nativeCall('vertexAttribI4uiv', [index, values]);
  }
  vertexAttribIPointer(index: number, size: number, type: number, stride: number, offset: number): void {
    return this.nativeCall('vertexAttribIPointer', [index, size, type, stride, offset]);
  }
  waitSync(sync: WebGLSync, flags: number, timeout: number): void {
    return this.nativeCall('waitSync', [sync, flags, timeout]);
  }

  getShaderPrecisionFormat(shadertype: number, precisiontype: number): WebGLShaderPrecisionFormat {
    const { rangeMin, rangeMax, precision } = super.getShaderPrecisionFormat(shadertype, precisiontype);
    return new WebGLShaderPrecisionFormatImpl(rangeMin, rangeMax, precision);
  }
  shaderSource(shader: WebGLShader, source: string): void {
    /**
     * Process rules:
     * - The #extension directive must appear before any non-preprocessor tokens in a shader.
     */
    const lines = source.split('\n');
    // find all #extension directives and move them to the top of the shader
    const extensionDecls: string[] = [];
    let fixedShaderSrc: string = '';
    for (const line of lines) {
      if (line.startsWith('#extension ') && line.endsWith(': enable')) {
        extensionDecls.push(line);
      } else {
        fixedShaderSrc += line + '\n';
      }
    }
    if (extensionDecls.length > 0) {
      const extensions = extensionDecls.join('\n');
      fixedShaderSrc = extensions + '\n' + fixedShaderSrc;
    }

    return this.nativeCall('shaderSource', [shader, fixedShaderSrc], {
      debug: {
        argTypes: [, 'ignore'],
      },
    });
  }
  bufferData(target: number, data: number | BufferSource, usage: number, srcOffset?: number, length: number = 0): void {
    if (typeof data === 'number') {
      throw new Error('bufferData() with size not implemented.');
    } else {
      let dataBuffer: Uint8Array;
      if (data instanceof DataView || isTypedArray(data)) {
        dataBuffer = unpackTypedArray(data);
      } else if (data instanceof ArrayBuffer) {
        dataBuffer = new Uint8Array(data);
      } else {
        throw new Error('Invalid data type for bufferData(), expected ArrayBuffer or TypedArray.');
      }
      return this.nativeCall('bufferData', [target, dataBuffer, usage], {
        debug: {
          argTypes: ['constant', 'default', 'constant'],
        }
      });
    }
  }
}

Object.assign(WebGL2RenderingContextImpl.prototype, {
  getExtension: WebGLRenderingContextImpl.prototype.getExtension,
});
export default WebGL2RenderingContextImpl;
