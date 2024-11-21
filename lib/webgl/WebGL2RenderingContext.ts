import WebGLRenderingContextImpl from './WebGLRenderingContext';
import { WebGLShaderPrecisionFormatImpl } from './WebGLShaderPrecisionFormat';
import {
  type NativeCallOptions,
  makeNativeCall,
  isTypedArray,
  unpackTypedArray
} from './utils';
const glNative = process._linkedBinding('transmute:webgl');

class WebGL2RenderingContextImpl extends glNative.WebGL2RenderingContext implements WebGL2RenderingContext {
  constructor(_canvas: HTMLCanvasElement | OffscreenCanvas, options?: WebGLContextAttributes) {
    super(options);
  }
  private nativeCall(name: string, args: any[] = [], options: NativeCallOptions = {}) {
    return makeNativeCall.call(this, <Function>super[name], name, args, options);
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
      } else if (Array.isArray(data)) {
        dataBuffer = unpackTypedArray(new Float32Array(data));
      } else {
        console.warn('#');
        console.warn('# Invalid data type for bufferData().');
        console.warn('# target:', target);
        console.warn('# data:',
          'is TypedArray?' + isTypedArray(data) ? 'yes' : 'no',
          'is Array?' + Array.isArray(data) ? 'yes' : 'no',
          data);
        console.warn('#');
        throw new Error('Invalid data type for bufferData(), expected ArrayBuffer or TypedArray, but got ' + typeof data);
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
