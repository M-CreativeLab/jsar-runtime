const glNative = process._linkedBinding('transmute:webgl');

export default class WebGLRenderingContextImpl extends glNative.WebGLRenderingContext implements WebGLRenderingContext {
  // DEPTH_BUFFER_BIT: 256;
  // STENCIL_BUFFER_BIT: 1024;
  // COLOR_BUFFER_BIT: 16384;
  POINTS: 0;
  LINES: 1;
  LINE_LOOP: 2;
  LINE_STRIP: 3;
  TRIANGLES: 4;
  TRIANGLE_STRIP: 5;
  TRIANGLE_FAN: 6;
  ZERO: 0;
  ONE: 1;
  SRC_COLOR: 768;
  ONE_MINUS_SRC_COLOR: 769;
  SRC_ALPHA: 770;
  ONE_MINUS_SRC_ALPHA: 771;
  DST_ALPHA: 772;
  ONE_MINUS_DST_ALPHA: 773;
  DST_COLOR: 774;
  ONE_MINUS_DST_COLOR: 775;
  SRC_ALPHA_SATURATE: 776;
  FUNC_ADD: 32774;
  BLEND_EQUATION: 32777;
  BLEND_EQUATION_RGB: 32777;
  BLEND_EQUATION_ALPHA: 34877;
  FUNC_SUBTRACT: 32778;
  FUNC_REVERSE_SUBTRACT: 32779;
  BLEND_DST_RGB: 32968;
  BLEND_SRC_RGB: 32969;
  BLEND_DST_ALPHA: 32970;
  BLEND_SRC_ALPHA: 32971;
  CONSTANT_COLOR: 32769;
  ONE_MINUS_CONSTANT_COLOR: 32770;
  CONSTANT_ALPHA: 32771;
  ONE_MINUS_CONSTANT_ALPHA: 32772;
  BLEND_COLOR: 32773;
  ARRAY_BUFFER: 34962;
  ELEMENT_ARRAY_BUFFER: 34963;
  ARRAY_BUFFER_BINDING: 34964;
  ELEMENT_ARRAY_BUFFER_BINDING: 34965;
  STREAM_DRAW: 35040;
  STATIC_DRAW: 35044;
  DYNAMIC_DRAW: 35048;
  BUFFER_SIZE: 34660;
  BUFFER_USAGE: 34661;
  CURRENT_VERTEX_ATTRIB: 34342;
  FRONT: 1028;
  BACK: 1029;
  FRONT_AND_BACK: 1032;
  CULL_FACE: 2884;
  BLEND: 3042;
  DITHER: 3024;
  STENCIL_TEST: 2960;
  DEPTH_TEST: 2929;
  SCISSOR_TEST: 3089;
  POLYGON_OFFSET_FILL: 32823;
  SAMPLE_ALPHA_TO_COVERAGE: 32926;
  SAMPLE_COVERAGE: 32928;
  NO_ERROR: 0;
  INVALID_ENUM: 1280;
  INVALID_VALUE: 1281;
  INVALID_OPERATION: 1282;
  OUT_OF_MEMORY: 1285;
  CW: 2304;
  CCW: 2305;
  LINE_WIDTH: 2849;
  ALIASED_POINT_SIZE_RANGE: 33901;
  ALIASED_LINE_WIDTH_RANGE: 33902;
  CULL_FACE_MODE: 2885;
  FRONT_FACE: 2886;
  DEPTH_RANGE: 2928;
  DEPTH_WRITEMASK: 2930;
  DEPTH_CLEAR_VALUE: 2931;
  DEPTH_FUNC: 2932;
  STENCIL_CLEAR_VALUE: 2961;
  STENCIL_FUNC: 2962;
  STENCIL_FAIL: 2964;
  STENCIL_PASS_DEPTH_FAIL: 2965;
  STENCIL_PASS_DEPTH_PASS: 2966;
  STENCIL_REF: 2967;
  STENCIL_VALUE_MASK: 2963;
  STENCIL_WRITEMASK: 2968;
  STENCIL_BACK_FUNC: 34816;
  STENCIL_BACK_FAIL: 34817;
  STENCIL_BACK_PASS_DEPTH_FAIL: 34818;
  STENCIL_BACK_PASS_DEPTH_PASS: 34819;
  STENCIL_BACK_REF: 36003;
  STENCIL_BACK_VALUE_MASK: 36004;
  STENCIL_BACK_WRITEMASK: 36005;
  VIEWPORT: 2978;
  SCISSOR_BOX: 3088;
  COLOR_CLEAR_VALUE: 3106;
  COLOR_WRITEMASK: 3107;
  UNPACK_ALIGNMENT: 3317;
  PACK_ALIGNMENT: 3333;
  MAX_TEXTURE_SIZE: 3379;
  MAX_VIEWPORT_DIMS: 3386;
  SUBPIXEL_BITS: 3408;
  RED_BITS: 3410;
  GREEN_BITS: 3411;
  BLUE_BITS: 3412;
  ALPHA_BITS: 3413;
  DEPTH_BITS: 3414;
  STENCIL_BITS: 3415;
  POLYGON_OFFSET_UNITS: 10752;
  POLYGON_OFFSET_FACTOR: 32824;
  TEXTURE_BINDING_2D: 32873;
  SAMPLE_BUFFERS: 32936;
  SAMPLES: 32937;
  SAMPLE_COVERAGE_VALUE: 32938;
  SAMPLE_COVERAGE_INVERT: 32939;
  COMPRESSED_TEXTURE_FORMATS: 34467;
  DONT_CARE: 4352;
  FASTEST: 4353;
  NICEST: 4354;
  GENERATE_MIPMAP_HINT: 33170;
  BYTE: 5120;
  UNSIGNED_BYTE: 5121;
  SHORT: 5122;
  UNSIGNED_SHORT: 5123;
  INT: 5124;
  UNSIGNED_INT: 5125;
  FLOAT: 5126;
  DEPTH_COMPONENT: 6402;
  ALPHA: 6406;
  RGB: 6407;
  RGBA: 6408;
  LUMINANCE: 6409;
  LUMINANCE_ALPHA: 6410;
  UNSIGNED_SHORT_4_4_4_4: 32819;
  UNSIGNED_SHORT_5_5_5_1: 32820;
  UNSIGNED_SHORT_5_6_5: 33635;
  MAX_VERTEX_ATTRIBS: 34921;
  MAX_VERTEX_UNIFORM_VECTORS: 36347;
  MAX_VARYING_VECTORS: 36348;
  MAX_COMBINED_TEXTURE_IMAGE_UNITS: 35661;
  MAX_VERTEX_TEXTURE_IMAGE_UNITS: 35660;
  MAX_TEXTURE_IMAGE_UNITS: 34930;
  MAX_FRAGMENT_UNIFORM_VECTORS: 36349;
  SHADER_TYPE: 35663;
  DELETE_STATUS: 35712;
  LINK_STATUS: 35714;
  VALIDATE_STATUS: 35715;
  ATTACHED_SHADERS: 35717;
  ACTIVE_UNIFORMS: 35718;
  ACTIVE_ATTRIBUTES: 35721;
  SHADING_LANGUAGE_VERSION: 35724;
  CURRENT_PROGRAM: 35725;
  NEVER: 512;
  LESS: 513;
  EQUAL: 514;
  LEQUAL: 515;
  GREATER: 516;
  NOTEQUAL: 517;
  GEQUAL: 518;
  ALWAYS: 519;
  KEEP: 7680;
  REPLACE: 7681;
  INCR: 7682;
  DECR: 7683;
  INVERT: 5386;
  INCR_WRAP: 34055;
  DECR_WRAP: 34056;
  VENDOR: 7936;
  RENDERER: 7937;
  VERSION: 7938;
  NEAREST: 9728;
  LINEAR: 9729;
  NEAREST_MIPMAP_NEAREST: 9984;
  LINEAR_MIPMAP_NEAREST: 9985;
  NEAREST_MIPMAP_LINEAR: 9986;
  LINEAR_MIPMAP_LINEAR: 9987;
  TEXTURE_MAG_FILTER: 10240;
  TEXTURE_MIN_FILTER: 10241;
  TEXTURE_WRAP_S: 10242;
  TEXTURE_WRAP_T: 10243;
  TEXTURE_2D: 3553;
  TEXTURE: 5890;
  TEXTURE_CUBE_MAP: 34067;
  TEXTURE_BINDING_CUBE_MAP: 34068;
  TEXTURE_CUBE_MAP_POSITIVE_X: 34069;
  TEXTURE_CUBE_MAP_NEGATIVE_X: 34070;
  TEXTURE_CUBE_MAP_POSITIVE_Y: 34071;
  TEXTURE_CUBE_MAP_NEGATIVE_Y: 34072;
  TEXTURE_CUBE_MAP_POSITIVE_Z: 34073;
  TEXTURE_CUBE_MAP_NEGATIVE_Z: 34074;
  MAX_CUBE_MAP_TEXTURE_SIZE: 34076;
  TEXTURE0: 33984;
  TEXTURE1: 33985;
  TEXTURE2: 33986;
  TEXTURE3: 33987;
  TEXTURE4: 33988;
  TEXTURE5: 33989;
  TEXTURE6: 33990;
  TEXTURE7: 33991;
  TEXTURE8: 33992;
  TEXTURE9: 33993;
  TEXTURE10: 33994;
  TEXTURE11: 33995;
  TEXTURE12: 33996;
  TEXTURE13: 33997;
  TEXTURE14: 33998;
  TEXTURE15: 33999;
  TEXTURE16: 34000;
  TEXTURE17: 34001;
  TEXTURE18: 34002;
  TEXTURE19: 34003;
  TEXTURE20: 34004;
  TEXTURE21: 34005;
  TEXTURE22: 34006;
  TEXTURE23: 34007;
  TEXTURE24: 34008;
  TEXTURE25: 34009;
  TEXTURE26: 34010;
  TEXTURE27: 34011;
  TEXTURE28: 34012;
  TEXTURE29: 34013;
  TEXTURE30: 34014;
  TEXTURE31: 34015;
  ACTIVE_TEXTURE: 34016;
  REPEAT: 10497;
  CLAMP_TO_EDGE: 33071;
  MIRRORED_REPEAT: 33648;
  FLOAT_VEC2: 35664;
  FLOAT_VEC3: 35665;
  FLOAT_VEC4: 35666;
  INT_VEC2: 35667;
  INT_VEC3: 35668;
  INT_VEC4: 35669;
  BOOL: 35670;
  BOOL_VEC2: 35671;
  BOOL_VEC3: 35672;
  BOOL_VEC4: 35673;
  FLOAT_MAT2: 35674;
  FLOAT_MAT3: 35675;
  FLOAT_MAT4: 35676;
  SAMPLER_2D: 35678;
  SAMPLER_CUBE: 35680;
  VERTEX_ATTRIB_ARRAY_ENABLED: 34338;
  VERTEX_ATTRIB_ARRAY_SIZE: 34339;
  VERTEX_ATTRIB_ARRAY_STRIDE: 34340;
  VERTEX_ATTRIB_ARRAY_TYPE: 34341;
  VERTEX_ATTRIB_ARRAY_NORMALIZED: 34922;
  VERTEX_ATTRIB_ARRAY_POINTER: 34373;
  VERTEX_ATTRIB_ARRAY_BUFFER_BINDING: 34975;
  IMPLEMENTATION_COLOR_READ_TYPE: 35738;
  IMPLEMENTATION_COLOR_READ_FORMAT: 35739;
  COMPILE_STATUS: 35713;
  LOW_FLOAT: 36336;
  MEDIUM_FLOAT: 36337;
  HIGH_FLOAT: 36338;
  LOW_INT: 36339;
  MEDIUM_INT: 36340;
  HIGH_INT: 36341;
  FRAMEBUFFER: 36160;
  RENDERBUFFER: 36161;
  RGBA4: 32854;
  RGB5_A1: 32855;
  RGB565: 36194;
  DEPTH_COMPONENT16: 33189;
  STENCIL_INDEX8: 36168;
  DEPTH_STENCIL: 34041;
  RENDERBUFFER_WIDTH: 36162;
  RENDERBUFFER_HEIGHT: 36163;
  RENDERBUFFER_INTERNAL_FORMAT: 36164;
  RENDERBUFFER_RED_SIZE: 36176;
  RENDERBUFFER_GREEN_SIZE: 36177;
  RENDERBUFFER_BLUE_SIZE: 36178;
  RENDERBUFFER_ALPHA_SIZE: 36179;
  RENDERBUFFER_DEPTH_SIZE: 36180;
  RENDERBUFFER_STENCIL_SIZE: 36181;
  FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE: 36048;
  FRAMEBUFFER_ATTACHMENT_OBJECT_NAME: 36049;
  FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL: 36050;
  FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE: 36051;
  COLOR_ATTACHMENT0: 36064;
  DEPTH_ATTACHMENT: 36096;
  STENCIL_ATTACHMENT: 36128;
  DEPTH_STENCIL_ATTACHMENT: 33306;
  NONE: 0;
  FRAMEBUFFER_COMPLETE: 36053;
  FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 36054;
  FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: 36055;
  FRAMEBUFFER_INCOMPLETE_DIMENSIONS: 36057;
  FRAMEBUFFER_UNSUPPORTED: 36061;
  FRAMEBUFFER_BINDING: 36006;
  RENDERBUFFER_BINDING: 36007;
  MAX_RENDERBUFFER_SIZE: 34024;
  INVALID_FRAMEBUFFER_OPERATION: 1286;
  UNPACK_FLIP_Y_WEBGL: 37440;
  UNPACK_PREMULTIPLY_ALPHA_WEBGL: 37441;
  CONTEXT_LOST_WEBGL: 37442;
  UNPACK_COLORSPACE_CONVERSION_WEBGL: 37443;
  BROWSER_DEFAULT_WEBGL: 37444;

  canvas: HTMLCanvasElement | OffscreenCanvas;
  drawingBufferColorSpace: PredefinedColorSpace;

  get drawingBufferHeight(): number {
    return super.drawingBufferHeight;
  }
  get drawingBufferWidth(): number {
    return super.drawingBufferWidth;
  }

  constructor(_canvas: HTMLCanvasElement | OffscreenCanvas, options: WebGLContextAttributes) {
    super();
  }

  activeTexture(texture: number): void {
    throw new Error('Method not implemented.');
  }
  attachShader(program: WebGLProgram, shader: WebGLShader): void {
    super.attachShader(program, shader);
  }
  bindAttribLocation(program: WebGLProgram, index: number, name: string): void {
    throw new Error('Method not implemented.');
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
    throw new Error('Method not implemented.');
  }
  blendColor(red: number, green: number, blue: number, alpha: number): void {
    throw new Error('Method not implemented.');
  }
  blendEquation(mode: number): void {
    throw new Error('Method not implemented.');
  }
  blendEquationSeparate(modeRGB: number, modeAlpha: number): void {
    throw new Error('Method not implemented.');
  }
  blendFunc(sfactor: number, dfactor: number): void {
    throw new Error('Method not implemented.');
  }
  blendFuncSeparate(srcRGB: number, dstRGB: number, srcAlpha: number, dstAlpha: number): void {
    throw new Error('Method not implemented.');
  }
  checkFramebufferStatus(target: number): number {
    throw new Error('Method not implemented.');
  }
  clear(mask: number): void {
    super.clear(mask);
  }
  clearColor(red: number, green: number, blue: number, alpha: number): void {
    super.clearColor(red, green, blue, alpha);
  }
  clearDepth(depth: number): void {
    super.clearDepth(depth);
  }
  clearStencil(s: number): void {
    super.clearStencil(s);
  }
  colorMask(red: boolean, green: boolean, blue: boolean, alpha: boolean): void {
    throw new Error('Method not implemented.');
  }
  compileShader(shader: WebGLShader): void {
    super.compileShader(shader);
  }
  copyTexImage2D(target: number, level: number, internalformat: number, x: number, y: number, width: number, height: number, border: number): void {
    throw new Error('Method not implemented.');
  }
  copyTexSubImage2D(target: number, level: number, xoffset: number, yoffset: number, x: number, y: number, width: number, height: number): void {
    throw new Error('Method not implemented.');
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
    throw new Error('Method not implemented.');
  }
  createShader(type: number): WebGLShader {
    return super.createShader(type);
  }
  createTexture(): WebGLTexture {
    throw new Error('Method not implemented.');
  }
  cullFace(mode: number): void {
    throw new Error('Method not implemented.');
  }
  deleteBuffer(buffer: WebGLBuffer): void {
    throw new Error('Method not implemented.');
  }
  deleteFramebuffer(framebuffer: WebGLFramebuffer): void {
    throw new Error('Method not implemented.');
  }
  deleteProgram(program: WebGLProgram): void {
    throw new Error('Method not implemented.');
  }
  deleteRenderbuffer(renderbuffer: WebGLRenderbuffer): void {
    throw new Error('Method not implemented.');
  }
  deleteShader(shader: WebGLShader): void {
    super.deleteShader(shader);
  }
  deleteTexture(texture: WebGLTexture): void {
    throw new Error('Method not implemented.');
  }
  depthFunc(func: number): void {
    throw new Error('Method not implemented.');
  }
  depthMask(flag: boolean): void {
    throw new Error('Method not implemented.');
  }
  depthRange(zNear: number, zFar: number): void {
    throw new Error('Method not implemented.');
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
  framebufferRenderbuffer(target: number, attachment: number, renderbuffertarget: number, renderbuffer: WebGLRenderbuffer): void {
    throw new Error('Method not implemented.');
  }
  framebufferTexture2D(target: number, attachment: number, textarget: number, texture: WebGLTexture, level: number): void {
    throw new Error('Method not implemented.');
  }
  frontFace(mode: number): void {
    throw new Error('Method not implemented.');
  }
  generateMipmap(target: number): void {
    throw new Error('Method not implemented.');
  }
  getActiveAttrib(program: WebGLProgram, index: number): WebGLActiveInfo {
    throw new Error('Method not implemented.');
  }
  getActiveUniform(program: WebGLProgram, index: number): WebGLActiveInfo {
    throw new Error('Method not implemented.');
  }
  getAttachedShaders(program: WebGLProgram): WebGLShader[] {
    throw new Error('Method not implemented.');
  }
  getAttribLocation(program: WebGLProgram, name: string): number {
    throw new Error('Method not implemented.');
  }
  getBufferParameter(target: number, pname: number) {
    throw new Error('Method not implemented.');
  }
  getContextAttributes(): WebGLContextAttributes {
    throw new Error('Method not implemented.');
  }
  getError(): number {
    throw new Error('Method not implemented.');
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
    throw new Error('Method not implemented.');
  }
  getFramebufferAttachmentParameter(target: number, attachment: number, pname: number) {
    throw new Error('Method not implemented.');
  }
  getParameter(pname: number) {
    throw new Error('Method not implemented.');
  }
  getProgramInfoLog(program: WebGLProgram): string {
    throw new Error('Method not implemented.');
  }
  getProgramParameter(program: WebGLProgram, pname: number) {
    throw new Error('Method not implemented.');
  }
  getRenderbufferParameter(target: number, pname: number) {
    throw new Error('Method not implemented.');
  }
  getShaderInfoLog(shader: WebGLShader): string {
    throw new Error('Method not implemented.');
  }
  getShaderParameter(shader: WebGLShader, pname: number) {
    throw new Error('Method not implemented.');
  }
  getShaderPrecisionFormat(shadertype: number, precisiontype: number): WebGLShaderPrecisionFormat {
    throw new Error('Method not implemented.');
  }
  getShaderSource(shader: WebGLShader): string {
    return super.getShaderSource(shader);
  }
  getSupportedExtensions(): string[] {
    throw new Error('Method not implemented.');
  }
  getTexParameter(target: number, pname: number) {
    throw new Error('Method not implemented.');
  }
  getUniform(program: WebGLProgram, location: WebGLUniformLocation) {
    throw new Error('Method not implemented.');
  }
  getUniformLocation(program: WebGLProgram, name: string): WebGLUniformLocation {
    throw new Error('Method not implemented.');
  }
  getVertexAttrib(index: number, pname: number) {
    throw new Error('Method not implemented.');
  }
  getVertexAttribOffset(index: number, pname: number): number {
    throw new Error('Method not implemented.');
  }
  hint(target: number, mode: number): void {
    throw new Error('Method not implemented.');
  }
  isBuffer(buffer: WebGLBuffer): boolean {
    throw new Error('Method not implemented.');
  }
  isContextLost(): boolean {
    throw new Error('Method not implemented.');
  }
  isEnabled(cap: number): boolean {
    throw new Error('Method not implemented.');
  }
  isFramebuffer(framebuffer: WebGLFramebuffer): boolean {
    throw new Error('Method not implemented.');
  }
  isProgram(program: WebGLProgram): boolean {
    throw new Error('Method not implemented.');
  }
  isRenderbuffer(renderbuffer: WebGLRenderbuffer): boolean {
    throw new Error('Method not implemented.');
  }
  isShader(shader: WebGLShader): boolean {
    throw new Error('Method not implemented.');
  }
  isTexture(texture: WebGLTexture): boolean {
    throw new Error('Method not implemented.');
  }
  lineWidth(width: number): void {
    throw new Error('Method not implemented.');
  }
  linkProgram(program: WebGLProgram): void {
    super.linkProgram(program);
  }
  pixelStorei(pname: number, param: number | boolean): void {
    throw new Error('Method not implemented.');
  }
  polygonOffset(factor: number, units: number): void {
    throw new Error('Method not implemented.');
  }
  renderbufferStorage(target: number, internalformat: number, width: number, height: number): void {
    throw new Error('Method not implemented.');
  }
  sampleCoverage(value: number, invert: boolean): void {
    throw new Error('Method not implemented.');
  }
  scissor(x: number, y: number, width: number, height: number): void {
    super.scissor(x, y, width, height);
  }
  shaderSource(shader: WebGLShader, source: string): void {
    super.shaderSource(shader, source);
  }
  stencilFunc(func: number, ref: number, mask: number): void {
    throw new Error('Method not implemented.');
  }
  stencilFuncSeparate(face: number, func: number, ref: number, mask: number): void {
    throw new Error('Method not implemented.');
  }
  stencilMask(mask: number): void {
    throw new Error('Method not implemented.');
  }
  stencilMaskSeparate(face: number, mask: number): void {
    throw new Error('Method not implemented.');
  }
  stencilOp(fail: number, zfail: number, zpass: number): void {
    throw new Error('Method not implemented.');
  }
  stencilOpSeparate(face: number, fail: number, zfail: number, zpass: number): void {
    throw new Error('Method not implemented.');
  }
  texParameterf(target: number, pname: number, param: number): void {
    throw new Error('Method not implemented.');
  }
  texParameteri(target: number, pname: number, param: number): void {
    throw new Error('Method not implemented.');
  }
  uniform1f(location: WebGLUniformLocation, x: number): void {
    throw new Error('Method not implemented.');
  }
  uniform1i(location: WebGLUniformLocation, x: number): void {
    throw new Error('Method not implemented.');
  }
  uniform2f(location: WebGLUniformLocation, x: number, y: number): void {
    throw new Error('Method not implemented.');
  }
  uniform2i(location: WebGLUniformLocation, x: number, y: number): void {
    throw new Error('Method not implemented.');
  }
  uniform3f(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    throw new Error('Method not implemented.');
  }
  uniform3i(location: WebGLUniformLocation, x: number, y: number, z: number): void {
    throw new Error('Method not implemented.');
  }
  uniform4f(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    throw new Error('Method not implemented.');
  }
  uniform4i(location: WebGLUniformLocation, x: number, y: number, z: number, w: number): void {
    throw new Error('Method not implemented.');
  }
  useProgram(program: WebGLProgram): void {
    super.useProgram(program);
  }
  validateProgram(program: WebGLProgram): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib1f(index: number, x: number): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib1fv(index: number, values: Float32List): void;
  vertexAttrib1fv(index: number, values: Iterable<number>): void;
  vertexAttrib1fv(index: unknown, values: unknown): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib2f(index: number, x: number, y: number): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib2fv(index: number, values: Float32List): void;
  vertexAttrib2fv(index: number, values: Iterable<number>): void;
  vertexAttrib2fv(index: unknown, values: unknown): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib3f(index: number, x: number, y: number, z: number): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib3fv(index: number, values: Float32List): void;
  vertexAttrib3fv(index: number, values: Iterable<number>): void;
  vertexAttrib3fv(index: unknown, values: unknown): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib4f(index: number, x: number, y: number, z: number, w: number): void {
    throw new Error('Method not implemented.');
  }
  vertexAttrib4fv(index: number, values: Float32List): void;
  vertexAttrib4fv(index: number, values: Iterable<number>): void;
  vertexAttrib4fv(index: unknown, values: unknown): void {
    throw new Error('Method not implemented.');
  }
  vertexAttribPointer(index: number, size: number, type: number, normalized: boolean, stride: number, offset: number): void {
    super.vertexAttribPointer(index, size, type, normalized, stride, offset);
  }
  viewport(x: number, y: number, width: number, height: number): void {
    super.viewport(x, y, width, height);
  }
  makeXRCompatible(): Promise<void> {
    throw new Error('Method not implemented.');
  }
  bufferData(target: number, size: number, usage: number): void;
  bufferData(target: number, data: BufferSource, usage: number): void;
  bufferData(target: unknown, data: unknown, usage: unknown): void {
    throw new Error('Method not implemented.');
  }
  bufferSubData(target: number, offset: number, data: BufferSource): void {
    throw new Error('Method not implemented.');
  }
  compressedTexImage2D(target: number, level: number, internalformat: number, width: number, height: number, border: number, data: ArrayBufferView): void {
    throw new Error('Method not implemented.');
  }
  compressedTexSubImage2D(target: number, level: number, xoffset: number, yoffset: number, width: number, height: number, format: number, data: ArrayBufferView): void {
    throw new Error('Method not implemented.');
  }
  readPixels(x: number, y: number, width: number, height: number, format: number, type: number, pixels: ArrayBufferView): void {
    throw new Error('Method not implemented.');
  }
  texImage2D(target: number, level: number, internalformat: number, width: number, height: number, border: number, format: number, type: number, pixels: ArrayBufferView): void;
  texImage2D(target: number, level: number, internalformat: number, format: number, type: number, source: TexImageSource): void;
  texImage2D(target: unknown, level: unknown, internalformat: unknown, width: unknown, height: unknown, border: unknown, format?: unknown, type?: unknown, pixels?: unknown): void {
    throw new Error('Method not implemented.');
  }
  texSubImage2D(target: number, level: number, xoffset: number, yoffset: number, width: number, height: number, format: number, type: number, pixels: ArrayBufferView): void;
  texSubImage2D(target: number, level: number, xoffset: number, yoffset: number, format: number, type: number, source: TexImageSource): void;
  texSubImage2D(target: unknown, level: unknown, xoffset: unknown, yoffset: unknown, width: unknown, height: unknown, format: unknown, type?: unknown, pixels?: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform1fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform1fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform1fv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform1iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform1iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform1iv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform2fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform2fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform2fv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform2iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform2iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform2iv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform3fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform3fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform3fv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform3iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform3iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform3iv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform4fv(location: WebGLUniformLocation, v: Float32List): void;
  uniform4fv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform4fv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniform4iv(location: WebGLUniformLocation, v: Int32List): void;
  uniform4iv(location: WebGLUniformLocation, v: Iterable<number>): void;
  uniform4iv(location: unknown, v: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix2fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix2fv(location: unknown, transpose: unknown, value: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix3fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix3fv(location: unknown, transpose: unknown, value: unknown): void {
    throw new Error('Method not implemented.');
  }
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Float32List): void;
  uniformMatrix4fv(location: WebGLUniformLocation, transpose: boolean, value: Iterable<number>): void;
  uniformMatrix4fv(location: unknown, transpose: unknown, value: unknown): void {
    throw new Error('Method not implemented.');
  }
}
