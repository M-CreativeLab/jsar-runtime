import { WebGLShaderPrecisionFormatImpl } from './WebGLShaderPrecisionFormat';
import { getExtension } from './extensions';
import {
  type NativeCallOptions,
  makeNativeCall,
  isTypedArray,
  unpackTypedArray
} from './utils';

const glNative = process._linkedBinding('transmute:webgl');

export default class WebGLRenderingContextImpl extends glNative.WebGLRenderingContext implements WebGLRenderingContext {
  canvas: HTMLCanvasElement | OffscreenCanvas;
  drawingBufferColorSpace: PredefinedColorSpace;

  constructor(_canvas: HTMLCanvasElement | OffscreenCanvas, options?: WebGLContextAttributes) {
    super(options);
  }

  private nativeCall(name: string, args: any[] = [], options: NativeCallOptions = {}) {
    return makeNativeCall.call(this, <Function>super[name], name, args, options);
  }

  bindFramebuffer(target: number, framebuffer: WebGLFramebuffer): void {
    if (framebuffer == null || framebuffer === undefined) {
      framebuffer = 0;
    }
    return this.nativeCall('bindFramebuffer', [target, framebuffer], {
      debug: {
        argTypes: ['constant', 'default'],
      },
    });
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
  getExtension(extensionName: 'OCULUS_multiview'): OCULUS_multiview;
  getExtension(extensionName: string): any {
    let ext: any = null;
    const supportedExtensions = this.getSupportedExtensions();
    if (supportedExtensions.includes(extensionName)) {
      ext = getExtension(this, extensionName);
    }
    if (ext == null) {
      console.warn(`The extension(${extensionName}) not found.`);
    }
    return ext;
  }
  getShaderPrecisionFormat(shadertype: number, precisiontype: number): WebGLShaderPrecisionFormat {
    const { rangeMin, rangeMax, precision } = super.getShaderPrecisionFormat(shadertype, precisiontype);
    return new WebGLShaderPrecisionFormatImpl(rangeMin, rangeMax, precision);
  }
  isContextLost(): boolean {
    /** TODO: how to implement context lost in JSAR? */
    return false;
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
  bufferData(target: number, size: number, usage: number): void;
  bufferData(target: number, data: BufferSource, usage: number): void;
  bufferData(target: number, data: number | BufferSource, usage: number): void {
    if (typeof data === 'number') {
      throw new Error('bufferData() with size not implemented.');
    } else {
      let dataBuffer: Uint8Array;
      if (data instanceof DataView || isTypedArray(data)) {
        dataBuffer = unpackTypedArray(data);
      } else if (data instanceof ArrayBuffer) {
        dataBuffer = new Uint8Array(data);
      } else {
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
