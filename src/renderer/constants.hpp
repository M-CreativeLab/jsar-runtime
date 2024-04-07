#pragma once

/**
 * This is a list of constants that are used in the WebGLRenderingContext class.
 *
 * @see https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Constants#standard_webgl_1_constants
 */

// Standard WebGL 1 constants

// Clearing buffers

/**
 * Passed to `clear` to clear the current depth buffer.
 */
const int WEBGL_DEPTH_BUFFER_BIT = 0x00000100;

/**
 * Passed to `clear` to clear the current stencil buffer.
 */
const int WEBGL_STENCIL_BUFFER_BIT = 0x00000400;

/**
 * Passed to `clear` to clear the current color buffer.
 */
const int WEBGL_COLOR_BUFFER_BIT = 0x00004000;

// Rendering primitives

/**
 * Passed to `drawElements` or `drawArrays` to draw single points.
 */
const int WEBGL_POINTS = 0x0000;

/**
 * Passed to `drawElements` or `drawArrays` to draw lines. Each vertex connects to the one after it.
 */
const int WEBGL_LINES = 0x0001;

/**
 * Passed to `drawElements` or `drawArrays` to draw lines. Each set of two vertices is treated as a separate line segment.
 */
const int WEBGL_LINE_LOOP = 0x0002;

/**
 * Passed to `drawElements` or `drawArrays` to draw a connected group of line segments from the first vertex to the last.
 */
const int WEBGL_LINE_STRIP = 0x0003;

/**
 * Passed to `drawElements` or `drawArrays` to draw triangles. Each set of three vertices creates a separate triangle.
 */
const int WEBGL_TRIANGLES = 0x0004;

/**
 * Passed to `drawElements` or `drawArrays` to draw a connected group of triangles.
 */
const int WEBGL_TRIANGLE_STRIP = 0x0005;

/**
 * Passed to `drawElements` or `drawArrays` to draw a connected group of triangles. Each vertex connects to the previous and the first vertex in the fan.
 */
const int WEBGL_TRIANGLE_FAN = 0x0006;

// Blending modes

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to turn off a component.
 */
const int WEBGL_ZERO = 0;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to turn on a component.
 */
const int WEBGL_ONE = 1;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by the source elements color.
 */
const int WEBGL_SRC_COLOR = 0x0300;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by one minus the source elements color.
 */
const int WEBGL_ONE_MINUS_SRC_COLOR = 0x0301;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by the source's alpha.
 */
const int WEBGL_SRC_ALPHA = 0x0302;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by one minus the source's alpha.
 */
const int WEBGL_ONE_MINUS_SRC_ALPHA = 0x0303;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by the destination's alpha.
 */
const int WEBGL_DST_ALPHA = 0x0304;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by one minus the destination's alpha.
 */
const int WEBGL_ONE_MINUS_DST_ALPHA = 0x0305;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by the destination's color.
 */
const int WEBGL_DST_COLOR = 0x0306;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by one minus the destination's color.
 */
const int WEBGL_ONE_MINUS_DST_COLOR = 0x0307;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to multiply a component by the minimum of source's alpha or one minus the destination's alpha.
 */
const int WEBGL_SRC_ALPHA_SATURATE = 0x0308;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to specify a constant color blend function.
 */
const int WEBGL_CONSTANT_COLOR = 0x8001;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to specify one minus a constant color blend function.
 */
const int WEBGL_ONE_MINUS_CONSTANT_COLOR = 0x8002;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to specify a constant alpha blend function.
 */
const int WEBGL_CONSTANT_ALPHA = 0x8003;

/**
 * Passed to `blendFunc` or `blendFuncSeparate` to specify one minus a constant alpha blend function.
 */
const int WEBGL_ONE_MINUS_CONSTANT_ALPHA = 0x8004;

// Blending equations

/**
 * Passed to `blendEquation` or `blendEquationSeparate` to set an addition blend function.
 */
const int WEBGL_FUNC_ADD = 0x8006;

/**
 * Passed to `blendEquation` or `blendEquationSeparate` to specify a subtraction blend function (source - destination).
 */
const int WEBGL_FUNC_SUBTRACT = 0x800A;

/**
 * Passed to `blendEquation` or `blendEquationSeparate` to specify a reverse subtraction blend function (destination - source).
 */
const int WEBGL_FUNC_REVERSE_SUBTRACT = 0x800B;

// Getting GL parameter information

/**
 * Passed to `getParameter` to get the current RGB blend function.
 */
const int WEBGL_BLEND_EQUATION = 0x8009;

/**
 * Passed to `getParameter` to get the current RGB blend function.
 */
const int WEBGL_BLEND_EQUATION_RGB = 0x8009;

/**
 * Passed to `getParameter` to get the current alpha blend function.
 */
const int WEBGL_BLEND_EQUATION_ALPHA = 0x883D;

/**
 * Passed to `getParameter` to get the current destination RGB blend function.
 */
const int WEBGL_BLEND_DST_RGB = 0x80C8;

/**
 * Passed to `getParameter` to get the current source RGB blend function.
 */
const int WEBGL_BLEND_SRC_RGB = 0x80C9;

/**
 * Passed to `getParameter` to get the current destination alpha blend function.
 */
const int WEBGL_BLEND_DST_ALPHA = 0x80CA;

/**
 * Passed to `getParameter` to get the current source alpha blend function.
 */
const int WEBGL_BLEND_SRC_ALPHA = 0x80CB;

/**
 * Passed to `getParameter` to get the current blend color.
 */
const int WEBGL_BLEND_COLOR = 0x8005;

/**
 * Passed to `getParameter` to get the array buffer binding.
 */
const int WEBGL_ARRAY_BUFFER_BINDING = 0x8894;

/**
 * Passed to `getParameter` to get the element array buffer binding.
 */
const int WEBGL_ELEMENT_ARRAY_BUFFER_BINDING = 0x8895;

/**
 * Passed to `getParameter` to get the current `lineWidth` (set by the `lineWidth` method).
 */
const int WEBGL_LINE_WIDTH = 0x0B21;

/**
 * Passed to `getParameter` to get the current size of a point drawn with `gl.POINTS`.
 */
const int WEBGL_ALIASED_POINT_SIZE_RANGE = 0x846D;

/**
 * Passed to `getParameter` to get the range of available widths for a line. Returns a length-2 array with the lo
 * value at 0, and hight at 1.
 */
const int WEBGL_ALIASED_LINE_WIDTH_RANGE = 0x846E;

/**
 * Passed to `getParameter` to get the current value of `cullFace`. Should return `FRONT`, `BACK`, or `FRONT_AND_BACK`.
 */
const int WEBGL_CULL_FACE_MODE = 0x0B45;

/**
 * Passed to `getParameter` to determine the current value of `frontFace`. Should return `CW` or `CCW`.
 */
const int WEBGL_FRONT_FACE = 0x0B46;

/**
 * Passed to `getParameter` to return a length-2 array of floats giving the current depth range.
 */
const int WEBGL_DEPTH_RANGE = 0x0B70;

/**
 * Passed to `getParameter` to determine if the depth write mask is enabled.
 */
const int WEBGL_DEPTH_WRITEMASK = 0x0B72;

/**
 * Passed to `getParameter` to determine the current depth clear value.
 */
const int WEBGL_DEPTH_CLEAR_VALUE = 0x0B73;

/**
 * Passed to `getParameter` to determine the current depth function. Returns `NEVER`, `ALWAYS`, `LESS`, `EQUAL`, `LEQUAL`,
 * `GREATER`, `GEQUAL`, or `NOTEQUAL`.
 */
const int WEBGL_DEPTH_FUNC = 0x0B74;

/**
 * Passed to `getParameter` to get the value the stencil will be cleared to.
 */
const int WEBGL_STENCIL_CLEAR_VALUE = 0x0B91;

/**
 * Passed to `getParameter` to get the current stencil function. Returns `NEVER`, `ALWAYS`, `LESS`, `EQUAL`, `LEQUAL`,
 */
const int WEBGL_STENCIL_FUNC = 0x0B92;

/**
 * Passed to `getParameter` to get the current stencil value used when the stencil test fails.
 */
const int WEBGL_STENCIL_FAIL = 0x0B94;

/**
 * Passed to `getParameter` to get the current stencil value used when the stencil test passes, but the depth test fails.
 */
const int WEBGL_STENCIL_PASS_DEPTH_FAIL = 0x0B95;

/**
 * Passed to `getParameter` to get the current stencil value when the stencil test passes.
 */
const int WEBGL_STENCIL_PASS_DEPTH_PASS = 0x0B96;

/**
 * Passed to `getParameter` to get the reference value used for stencil tests.
 */
const int WEBGL_STENCIL_REF = 0x0B98;

const int WEBGL_STENCIL_VALUE_MASK = 0x0B93;
const int WEBGL_STENCIL_WRITEMASK = 0x0B98;
const int WEBGL_STENCIL_BACK_FUNC = 0x8800;
const int WEBGL_STENCIL_BACK_FAIL = 0x8801;
const int WEBGL_STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802;
const int WEBGL_STENCIL_BACK_PASS_DEPTH_PASS = 0x8803;
const int WEBGL_STENCIL_BACK_REF = 0x8CA3;
const int WEBGL_STENCIL_BACK_VALUE_MASK = 0x8CA4;
const int WEBGL_STENCIL_BACK_WRITEMASK = 0x8CA5;

/**
 * Returns an `Int32Array` with four elements for the current viewport dimensions.
 */
const int WEBGL_VIEWPORT = 0x0BA2;

/**
 * Returns an `Int32Array` with four elements for the current scissor box dimensions.
 */
const int WEBGL_SCISSOR_BOX = 0x0C10;

/**
 * Returns the current color write mask as an array of booleans.
 */
const int WEBGL_COLOR_CLEAR_VALUE = 0x0C22;

/**
 * Returns the current color write mask as an array of booleans.
 */
const int WEBGL_COLOR_WRITEMASK = 0x0C23;

/**
 * Returns the current value of `UNPACK_ALIGNMENT`, which is the byte alignment used when uploading textures.
 */
const int WEBGL_UNPACK_ALIGNMENT = 0x0CF5;

/**
 * Returns the current value of `PACK_ALIGNMENT`, which is the byte alignment used when uploading textures.
 */
const int WEBGL_PACK_ALIGNMENT = 0x0D05;

const int WEBGL_MAX_TEXTURE_SIZE = 0x0D33;
const int WEBGL_MAX_VIEWPORT_DIMS = 0x0D3A;
const int WEBGL_SUBPIXEL_BITS = 0x0D50;
const int WEBGL_RED_BITS = 0x0D52;
const int WEBGL_GREEN_BITS = 0x0D53;
const int WEBGL_BLUE_BITS = 0x0D54;
const int WEBGL_ALPHA_BITS = 0x0D55;
const int WEBGL_DEPTH_BITS = 0x0D56;
const int WEBGL_STENCIL_BITS = 0x0D57;
const int WEBGL_POLYGON_OFFSET_UNITS = 0x2A00;
const int WEBGL_POLYGON_OFFSET_FACTOR = 0x8038;
const int WEBGL_TEXTURE_BINDING_2D = 0x8069;
const int WEBGL_SAMPLE_BUFFERS = 0x80A8;
const int WEBGL_SAMPLES = 0x80A9;
const int WEBGL_SAMPLE_COVERAGE_VALUE = 0x80AA;
const int WEBGL_SAMPLE_COVERAGE_INVERT = 0x80AB;
const int WEBGL_COMPRESSED_TEXTURE_FORMATS = 0x86A3;
const int WEBGL_VENDOR = 0x1F00;
const int WEBGL_RENDERER = 0x1F01;
const int WEBGL_VERSION = 0x1F02;
const int WEBGL_IMPLEMENTATION_COLOR_READ_TYPE = 0x8B9A;
const int WEBGL_IMPLEMENTATION_COLOR_READ_FORMAT = 0x8B9B;
const int WEBGL_BROWSER_DEFAULT_WEBGL = 0x9244;

// Buffers

/**
 * Passed to `bufferData` as a hint about whether the contents of the buffer are likely to be used often and not change
 * often.
 */
const int WEBGL_STATIC_DRAW = 0x88E4;

/**
 * Passed to `bufferData` as a hint about whether the contents of the buffer are likely to not be used often.
 */
const int WEBGL_STREAM_DRAW = 0x88E0;

/**
 * Passed to `bufferData` as a hint about whether the contents of the buffer are likely to be used often and change often.
 */
const int WEBGL_DYNAMIC_DRAW = 0x88E8;

/**
 * Passed to `bindBuffer` or `createBuffer` to specify the type of buffer being used.
 */
const int WEBGL_ARRAY_BUFFER = 0x8892;

/**
 * Passed to `bindBuffer` or `createBuffer` to specify the type of buffer being used.
 */
const int WEBGL_ELEMENT_ARRAY_BUFFER = 0x8893;

/**
 * Passed to `getBufferParameter` as a hint about the type of data that will be read from the buffer.
 */
const int WEBGL_BUFFER_SIZE = 0x8764;

/**
 * Passed to `getBufferParameter` to get a buffer's usage pattern.
 */
const int WEBGL_BUFFER_USAGE = 0x8765;

// Vertex attributes

/**
 * Passed to `getVertexAttrib` to read back the current vertex attribute.
 */
const int WEBGL_CURRENT_VERTEX_ATTRIB = 0x8626;

const int WEBGL_VERTEX_ATTRIB_ARRAY_ENABLED = 0x8622;
const int WEBGL_VERTEX_ATTRIB_ARRAY_SIZE = 0x8623;
const int WEBGL_VERTEX_ATTRIB_ARRAY_STRIDE = 0x8624;
const int WEBGL_VERTEX_ATTRIB_ARRAY_TYPE = 0x8625;
const int WEBGL_VERTEX_ATTRIB_ARRAY_NORMALIZED = 0x886A;
const int WEBGL_VERTEX_ATTRIB_ARRAY_POINTER = 0x8645;
const int WEBGL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F;

// Culling

/**
 * Passed to enable/disable to turn on/off culling. Can also be used with getParameter to find the current culling method.
 */
const int WEBGL_CULL_FACE = 0x0B44;

/**
 * Passed to cullFace to specify that only front faces should be drawn.
 */
const int WEBGL_FRONT = 0x0404;

/**
 * Passed to cullFace to specify that only back faces should be drawn.
 */
const int WEBGL_BACK = 0x0405;

/**
 * Passed to cullFace to specify that front and back faces should be drawn.
 */
const int WEBGL_FRONT_AND_BACK = 0x0408;

// Enabling and disabling

/**
 * Passed to enable/disable to turn on/off blending. Can also be used with getParameter to find the current blending method.
 */
const int WEBGL_BLEND = 0x0BE2;

/**
 * Passed to enable/disable to turn on/off the depth test. Can also be used with getParameter to query the depth test.
 */
const int WEBGL_DEPTH_TEST = 0x0B71;

/**
 * Passed to enable/disable to turn on/off dithering. Can also be used with getParameter to find the current dithering method.
 */
const int WEBGL_DITHER = 0x0BD0;

/**
 * Passed to enable/disable to turn on/off the polygon offset. Can also be used with getParameter to query the polygon offset.
 */
const int WEBGL_POLYGON_OFFSET_FILL = 0x8037;

/**
 * Passed to enable/disable to turn on/off the alpha to coverage. Can also be used with getParameter to query the alpha to coverage.
 */
const int WEBGL_SAMPLE_ALPHA_TO_COVERAGE = 0x809E;

/**
 * Passed to enable/disable to turn on/off the sample coverage. Can also be used with getParameter to query the sample coverage.
 */
const int WEBGL_SAMPLE_COVERAGE = 0x80A0;

/**
 * Passed to enable/disable to turn on/off the scissor test. Can also be used with getParameter to query the scissor test.
 */
const int WEBGL_SCISSOR_TEST = 0x0C11;

/**
 * Passed to enable/disable to turn on/off the stencil test. Can also be used with getParameter to query the stencil test.
 */
const int WEBGL_STENCIL_TEST = 0x0B90;

// Errors

const int WEBGL_NO_ERROR = 0;
const int WEBGL_INVALID_ENUM = 0x0500;
const int WEBGL_INVALID_VALUE = 0x0501;
const int WEBGL_INVALID_OPERATION = 0x0502;
const int WEBGL_OUT_OF_MEMORY = 0x0505;
const int WEBGL_CONTEXT_LOST_WEBGL = 0x9242;

// Front face directions

/**
 * Passed to `frontFace` to specify the front face of a polygon is drawn in the clockwise direction.
 */
const int WEBGL_CW = 0x0900;

/**
 * Passed to `frontFace` to specify the front face of a polygon is drawn in the counter clockwise direction.
 */
const int WEBGL_CCW = 0x0901;

// Hints

/**
 * There is no preference for this behavior.
 */
const int WEBGL_DONT_CARE = 0x1100;

/**
 * The most efficient behavior should be used.
 */
const int WEBGL_FASTEST = 0x1101;

/**
 * The most correct or the highest quality option should be used.
 */
const int WEBGL_NICEST = 0x1102;

/**
 * Hint for the quality of filtering when generating mipmap images with `generateMipmap`.
 */
const int WEBGL_GENERATE_MIPMAP_HINT = 0x8192;

// Data types

const int WEBGL_BYTE = 0x1400;
const int WEBGL_UNSIGNED_BYTE = 0x1401;
const int WEBGL_SHORT = 0x1402;
const int WEBGL_UNSIGNED_SHORT = 0x1403;
const int WEBGL_INT = 0x1404;
const int WEBGL_UNSIGNED_INT = 0x1405;
const int WEBGL_FLOAT = 0x1406;

// Pixel formats

const int WEBGL_DEPTH_COMPONENT = 0x1902;
const int WEBGL_ALPHA = 0x1906;
const int WEBGL_RGB = 0x1907;
const int WEBGL_RGBA = 0x1908;
const int WEBGL_LUMINANCE = 0x1909;
const int WEBGL_LUMINANCE_ALPHA = 0x190A;

// Pixel types

// const int WEBGL_UNSIGNED_BYTE = 0x1401;  // duplicated from above
const int WEBGL_UNSIGNED_SHORT_4_4_4_4 = 0x8033;
const int WEBGL_UNSIGNED_SHORT_5_5_5_1 = 0x8034;
const int WEBGL_UNSIGNED_SHORT_5_6_5 = 0x8363;

// Shaders

/**
 * Passed to `createShader` to define a fragment shader.
 */
const int WEBGL_FRAGMENT_SHADER = 0x8B30;

/**
 * Passed to `createShader` to define a vertex shader
 */
const int WEBGL_VERTEX_SHADER = 0x8B31;

/**
 * Passed to `getShaderParameter` to get the status of the compilation. Returns false if the shader is not compiled.
 */
const int WEBGL_COMPILE_STATUS = 0x8B81;

/**
 * Passed to `getShaderParameter` to determine if a shader was deleted via `deleteShader`. Returns true if it was, false
 * otherwise.
 */
const int WEBGL_DELETE_STATUS = 0x8B80;

/**
 * Passed to `getProgramParameter` after calling `linkProgram` to determine if a program was linked correctly. Returns 
 * false if there were errors. Use `getProgramInfoLog` to find the exact error.
 */
const int WEBGL_LINK_STATUS = 0x8B82;

/**
 * Passed to `getProgramParameter` after calling `validateProgram` to determine if it is valid. Returns false if there were
 * errors.
 */
const int WEBGL_VALIDATE_STATUS = 0x8B83;

/**
 * Passed to `getProgramParameter` after calling `attachShader` to determine if the shader was attached correctly. Returns
 * false if there were errors.
 */
const int WEBGL_ATTACHED_SHADERS = 0x8B85;

/**
 * Passed to `getProgramParameter` to get the number of attributes active in a program.
 */
const int WEBGL_ACTIVE_ATTRIBUTES = 0x8B89;

/**
 * Passed to `getProgramParameter` to get the number of uniforms active in a program.
 */
const int WEBGL_ACTIVE_UNIFORMS = 0x8B86;

/**
 * The maximum number of entries possible in the vertex attribute list.
 */
const int WEBGL_MAX_VERTEX_ATTRIBS = 0x8869;

/**
 * The maximum number of entries possible in the uniform list.
 */
const int WEBGL_MAX_VERTEX_UNIFORM_VECTORS = 0x8DFB;

/**
 * The maximum number of entries possible in the uniform list.
 */
const int WEBGL_MAX_VARYING_VECTORS = 0x8DFC;

/**
 * The maximum number of entries possible in the uniform list.
 */
const int WEBGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D;

/**
 * The maximum number of entries possible in the uniform list.
 */
const int WEBGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C;

/**
 * The maximum number of entries possible in the uniform list.
 */
const int WEBGL_MAX_TEXTURE_IMAGE_UNITS = 0x8872;

const int WEBGL_MAX_FRAGMENT_UNIFORM_VECTORS = 0x8DFD;
const int WEBGL_SHADER_TYPE = 0x8B4F;
const int WEBGL_SHADING_LANGUAGE_VERSION = 0x8B8C;
const int WEBGL_CURRENT_PROGRAM = 0x8B8D;

// Depth or stencil tests

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will never pass. i.e. Nothing will be
 * drawn.
 */
const int WEBGL_NEVER = 0x0200;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will pass if the new depth value is
 * less than the stored value.
 */
const int WEBGL_LESS = 0x0201;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will pass if the new depth value is
 * equals to the stored value.
 */
const int WEBGL_EQUAL = 0x0202;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will pass if the new depth value is
 * less than or equal to the stored value.
 */
const int WEBGL_LEQUAL = 0x0203;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will pass if the new depth value is
 * greater than the stored value.
 */
const int WEBGL_GREATER = 0x0204;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will pass if the new depth value is
 * not equal to the stored value.
 */
const int WEBGL_NOTEQUAL = 0x0205;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will pass if the new depth value is
 * greater than or equal to the stored value.
 */
const int WEBGL_GEQUAL = 0x0206;

/**
 * Passed to `depthFunction` or `stencilFunction` to specify depth or stencil tests will always pass. i.e. Pixels will be
 * drawn regardless of depth.
 */
const int WEBGL_ALWAYS = 0x0207;

// Stencil actions

const int WEBGL_KEEP = 0x1E00;
const int WEBGL_REPLACE = 0x1E01;
const int WEBGL_INCR = 0x1E02;
const int WEBGL_DECR = 0x1E03;
const int WEBGL_INVERT = 0x150A;
const int WEBGL_INCR_WRAP = 0x8507;
const int WEBGL_DECR_WRAP = 0x8508;

// Textures

const int WEBGL_NEAREST = 0x2600;
const int WEBGL_LINEAR = 0x2601;
const int WEBGL_NEAREST_MIPMAP_NEAREST = 0x2700;
const int WEBGL_LINEAR_MIPMAP_NEAREST = 0x2701;
const int WEBGL_NEAREST_MIPMAP_LINEAR = 0x2702;
const int WEBGL_LINEAR_MIPMAP_LINEAR = 0x2703;
const int WEBGL_TEXTURE_MAG_FILTER = 0x2800;
const int WEBGL_TEXTURE_MIN_FILTER = 0x2801;
const int WEBGL_TEXTURE_WRAP_S = 0x2802;
const int WEBGL_TEXTURE_WRAP_T = 0x2803;
const int WEBGL_TEXTURE_2D = 0x0DE1;
const int WEBGL_TEXTURE = 0x1702;
const int WEBGL_TEXTURE_CUBE_MAP = 0x8513;
const int WEBGL_TEXTURE_BINDING_CUBE_MAP = 0x8514;
const int WEBGL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515;
const int WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516;
const int WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517;
const int WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518;
const int WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519;
const int WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A;
const int WEBGL_MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C;
const int WEBGL_TEXTURE0 = 0x84C0;
const int WEBGL_TEXTURE1 = 0x84C1;
const int WEBGL_TEXTURE2 = 0x84C2;
const int WEBGL_TEXTURE3 = 0x84C3;
const int WEBGL_TEXTURE4 = 0x84C4;
const int WEBGL_TEXTURE5 = 0x84C5;
const int WEBGL_TEXTURE6 = 0x84C6;
const int WEBGL_TEXTURE7 = 0x84C7;
const int WEBGL_TEXTURE8 = 0x84C8;
const int WEBGL_TEXTURE9 = 0x84C9;
const int WEBGL_TEXTURE10 = 0x84CA;
const int WEBGL_TEXTURE11 = 0x84CB;
const int WEBGL_TEXTURE12 = 0x84CC;
const int WEBGL_TEXTURE13 = 0x84CD;
const int WEBGL_TEXTURE14 = 0x84CE;
const int WEBGL_TEXTURE15 = 0x84CF;
const int WEBGL_TEXTURE16 = 0x84D0;
const int WEBGL_TEXTURE17 = 0x84D1;
const int WEBGL_TEXTURE18 = 0x84D2;
const int WEBGL_TEXTURE19 = 0x84D3;
const int WEBGL_TEXTURE20 = 0x84D4;
const int WEBGL_TEXTURE21 = 0x84D5;
const int WEBGL_TEXTURE22 = 0x84D6;
const int WEBGL_TEXTURE23 = 0x84D7;
const int WEBGL_TEXTURE24 = 0x84D8;
const int WEBGL_TEXTURE25 = 0x84D9;
const int WEBGL_TEXTURE26 = 0x84DA;
const int WEBGL_TEXTURE27 = 0x84DB;
const int WEBGL_TEXTURE28 = 0x84DC;
const int WEBGL_TEXTURE29 = 0x84DD;
const int WEBGL_TEXTURE30 = 0x84DE;
const int WEBGL_TEXTURE31 = 0x84DF;
const int WEBGL_ACTIVE_TEXTURE = 0x84E0;
const int WEBGL_REPEAT = 0x2901;
const int WEBGL_CLAMP_TO_EDGE = 0x812F;
const int WEBGL_MIRRORED_REPEAT = 0x8370;

// Uniform types

const int WEBGL_FLOAT_VEC2 = 0x8B50;
const int WEBGL_FLOAT_VEC3 = 0x8B51;
const int WEBGL_FLOAT_VEC4 = 0x8B52;
const int WEBGL_INT_VEC2 = 0x8B53;
const int WEBGL_INT_VEC3 = 0x8B54;
const int WEBGL_INT_VEC4 = 0x8B55;
const int WEBGL_BOOL = 0x8B56;
const int WEBGL_BOOL_VEC2 = 0x8B57;
const int WEBGL_BOOL_VEC3 = 0x8B58;
const int WEBGL_BOOL_VEC4 = 0x8B59;
const int WEBGL_FLOAT_MAT2 = 0x8B5A;
const int WEBGL_FLOAT_MAT3 = 0x8B5B;
const int WEBGL_FLOAT_MAT4 = 0x8B5C;
const int WEBGL_SAMPLER_2D = 0x8B5E;
const int WEBGL_SAMPLER_CUBE = 0x8B60;

// Shader precision-specified types

const int WEBGL_LOW_FLOAT = 0x8DF0;
const int WEBGL_MEDIUM_FLOAT = 0x8DF1;
const int WEBGL_HIGH_FLOAT = 0x8DF2;
const int WEBGL_LOW_INT = 0x8DF3;
const int WEBGL_MEDIUM_INT = 0x8DF4;
const int WEBGL_HIGH_INT = 0x8DF5;

// Framebuffers and renderbuffers

const int WEBGL_FRAMEBUFFER = 0x8D40;
const int WEBGL_RENDERBUFFER = 0x8D41;
const int WEBGL_RGBA4 = 0x8056;
const int WEBGL_RGB5_A1 = 0x8057;
const int WEBGL_RGB565 = 0x8D62;
const int WEBGL_DEPTH_COMPONENT16 = 0x81A5;
const int WEBGL_STENCIL_INDEX8 = 0x8D48;
const int WEBGL_DEPTH_STENCIL = 0x84F9;
const int WEBGL_RENDERBUFFER_WIDTH = 0x8D42;
const int WEBGL_RENDERBUFFER_HEIGHT = 0x8D43;
const int WEBGL_RENDERBUFFER_INTERNAL_FORMAT = 0x8D44;
const int WEBGL_RENDERBUFFER_RED_SIZE = 0x8D50;
const int WEBGL_RENDERBUFFER_GREEN_SIZE = 0x8D51;
const int WEBGL_RENDERBUFFER_BLUE_SIZE = 0x8D52;
const int WEBGL_RENDERBUFFER_ALPHA_SIZE = 0x8D53;
const int WEBGL_RENDERBUFFER_DEPTH_SIZE = 0x8D54;
const int WEBGL_RENDERBUFFER_STENCIL_SIZE = 0x8D55;
const int WEBGL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = 0x8CD0;
const int WEBGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME = 0x8CD1;
const int WEBGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL = 0x8CD2;
const int WEBGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3;
const int WEBGL_COLOR_ATTACHMENT0 = 0x8CE0;
const int WEBGL_DEPTH_ATTACHMENT = 0x8D00;
const int WEBGL_STENCIL_ATTACHMENT = 0x8D20;
const int WEBGL_DEPTH_STENCIL_ATTACHMENT = 0x821A;
const int WEBGL_NONE = 0;
const int WEBGL_FRAMEBUFFER_COMPLETE = 0x8CD5;
const int WEBGL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT = 0x8CD6;
const int WEBGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7;
const int WEBGL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS = 0x8CD9;
const int WEBGL_FRAMEBUFFER_UNSUPPORTED = 0x8CDD;
const int WEBGL_FRAMEBUFFER_BINDING = 0x8CA6;
const int WEBGL_RENDERBUFFER_BINDING = 0x8CA7;
const int WEBGL_MAX_RENDERBUFFER_SIZE = 0x84E8;
const int WEBGL_INVALID_FRAMEBUFFER_OPERATION = 0x0506;

// Pixel storage modes

const int WEBGL_UNPACK_FLIP_Y_WEBGL = 0x9240;
const int WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL = 0x9241;
const int WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL = 0x9243;

/**
 * WebGL2 constants
 */

// GL Parameters

const int WEBGL2_READ_BUFFER = 0x0C02;
const int WEBGL2_UNPACK_ROW_LENGTH = 0x0CF2;
const int WEBGL2_UNPACK_SKIP_ROWS = 0x0CF3;
const int WEBGL2_UNPACK_SKIP_PIXELS = 0x0CF4;
const int WEBGL2_PACK_ROW_LENGTH = 0x0D02;
const int WEBGL2_PACK_SKIP_ROWS = 0x0D03;
const int WEBGL2_PACK_SKIP_PIXELS = 0x0D04;
const int WEBGL2_TEXTURE_BINDING_3D = 0x806A;
const int WEBGL2_UNPACK_SKIP_IMAGES = 0x806D;
const int WEBGL2_UNPACK_IMAGE_HEIGHT = 0x806E;
const int WEBGL2_MAX_3D_TEXTURE_SIZE = 0x8073;
const int WEBGL2_MAX_ELEMENTS_VERTICES = 0x80E8;
const int WEBGL2_MAX_ELEMENTS_INDICES = 0x80E9;
const int WEBGL2_MAX_TEXTURE_LOD_BIAS = 0x84FD;
const int WEBGL2_MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49;
const int WEBGL2_MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A;
const int WEBGL2_MAX_ARRAY_TEXTURE_LAYERS = 0x88FF;
const int WEBGL2_MIN_PROGRAM_TEXEL_OFFSET = 0x8904;
const int WEBGL2_MAX_PROGRAM_TEXEL_OFFSET = 0x8905;
const int WEBGL2_MAX_VARYING_COMPONENTS = 0x8B4B;
const int WEBGL2_FRAGMENT_SHADER_DERIVATIVE_HINT = 0x8B8B;
const int WEBGL2_RASTERIZER_DISCARD = 0x8C89;
const int WEBGL2_VERTEX_ARRAY_BINDING = 0x85B5;
const int WEBGL2_MAX_VERTEX_OUTPUT_COMPONENTS = 0x9122;
const int WEBGL2_MAX_FRAGMENT_INPUT_COMPONENTS = 0x9125;
const int WEBGL2_MAX_SERVER_WAIT_TIMEOUT = 0x9111;
const int WEBGL2_MAX_ELEMENT_INDEX = 0x8D6B;

// Textures

const int WEBGL2_RED = 0x1903;
const int WEBGL2_RGB8 = 0x8051;
const int WEBGL2_RGBA8 = 0x8058;
const int WEBGL2_RGB10_A2 = 0x8059;
const int WEBGL2_TEXTURE_3D = 0x806F;
const int WEBGL2_TEXTURE_WRAP_R = 0x8072;
const int WEBGL2_TEXTURE_MIN_LOD = 0x813A;
const int WEBGL2_TEXTURE_MAX_LOD = 0x813B;
const int WEBGL2_TEXTURE_BASE_LEVEL = 0x813C;
const int WEBGL2_TEXTURE_MAX_LEVEL = 0x813D;
const int WEBGL2_TEXTURE_COMPARE_MODE = 0x884C;
const int WEBGL2_TEXTURE_COMPARE_FUNC = 0x884D;
const int WEBGL2_SRGB = 0x8C40;
const int WEBGL2_SRGB8 = 0x8C41;
const int WEBGL2_SRGB8_ALPHA8 = 0x8C43;
const int WEBGL2_COMPARE_REF_TO_TEXTURE = 0x884E;
const int WEBGL2_RGBA32F = 0x8814;
const int WEBGL2_RGB32F = 0x8815;
const int WEBGL2_RGBA16F = 0x881A;
const int WEBGL2_RGB16F = 0x881B;
const int WEBGL2_TEXTURE_2D_ARRAY = 0x8C1A;
const int WEBGL2_TEXTURE_BINDING_2D_ARRAY = 0x8C1D;
const int WEBGL2_R11F_G11F_B10F = 0x8C3A;
const int WEBGL2_RGB9_E5 = 0x8C3D;
const int WEBGL2_RGBA32UI = 0x8D70;
const int WEBGL2_RGB32UI = 0x8D71;
const int WEBGL2_RGBA16UI = 0x8D76;
const int WEBGL2_RGB16UI = 0x8D77;
const int WEBGL2_RGBA8UI = 0x8D7C;
const int WEBGL2_RGB8UI = 0x8D7D;
const int WEBGL2_RGBA32I = 0x8D82;
const int WEBGL2_RGB32I = 0x8D83;
const int WEBGL2_RGBA16I = 0x8D88;
const int WEBGL2_RGB16I = 0x8D89;
const int WEBGL2_RGBA8I = 0x8D8E;
const int WEBGL2_RGB8I = 0x8D8F;
const int WEBGL2_RED_INTEGER = 0x8D94;
const int WEBGL2_RGB_INTEGER = 0x8D98;
const int WEBGL2_RGBA_INTEGER = 0x8D99;
const int WEBGL2_R8 = 0x8229;
const int WEBGL2_RG8 = 0x822B;
const int WEBGL2_R16F = 0x822D;
const int WEBGL2_R32F = 0x822E;
const int WEBGL2_RG16F = 0x822F;
const int WEBGL2_RG32F = 0x8230;
const int WEBGL2_R8I = 0x8231;
const int WEBGL2_R8UI = 0x8232;
const int WEBGL2_R16I = 0x8233;
const int WEBGL2_R16UI = 0x8234;
const int WEBGL2_R32I = 0x8235;
const int WEBGL2_R32UI = 0x8236;
const int WEBGL2_RG8I = 0x8237;
const int WEBGL2_RG8UI = 0x8238;
const int WEBGL2_RG16I = 0x8239;
const int WEBGL2_RG16UI = 0x823A;
const int WEBGL2_RG32I = 0x823B;
const int WEBGL2_RG32UI = 0x823C;
const int WEBGL2_R8_SNORM = 0x8F94;
const int WEBGL2_RG8_SNORM = 0x8F95;
const int WEBGL2_RGB8_SNORM = 0x8F96;
const int WEBGL2_RGBA8_SNORM = 0x8F97;
const int WEBGL2_RGB10_A2UI = 0x906F;
const int WEBGL2_TEXTURE_IMMUTABLE_FORMAT = 0x912F;
const int WEBGL2_TEXTURE_IMMUTABLE_LEVELS = 0x82DF;

// Pixel types

const int WEBGL2_UNSIGNED_INT_2_10_10_10_REV = 0x8368;
const int WEBGL2_UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B;
const int WEBGL2_UNSIGNED_INT_5_9_9_9_REV = 0x8C3E;
const int WEBGL2_FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD;
const int WEBGL2_UNSIGNED_INT_24_8 = 0x84FA;
const int WEBGL2_HALF_FLOAT = 0x140B;
const int WEBGL2_RG = 0x8227;
const int WEBGL2_RG_INTEGER = 0x8228;
const int WEBGL2_INT_2_10_10_10_REV = 0x8D9F;

// Queries

const int WEBGL2_CURRENT_QUERY = 0x8865;
const int WEBGL2_QUERY_RESULT = 0x8866;
const int WEBGL2_QUERY_RESULT_AVAILABLE = 0x8867;
const int WEBGL2_ANY_SAMPLES_PASSED = 0x8C2F;
const int WEBGL2_ANY_SAMPLES_PASSED_CONSERVATIVE = 0x8D6A;

// Draw buffers

const int WEBGL2_MAX_DRAW_BUFFERS = 0x8824;
const int WEBGL2_DRAW_BUFFER0 = 0x8825;
const int WEBGL2_DRAW_BUFFER1 = 0x8826;
const int WEBGL2_DRAW_BUFFER2 = 0x8827;
const int WEBGL2_DRAW_BUFFER3 = 0x8828;
const int WEBGL2_DRAW_BUFFER4 = 0x8829;
const int WEBGL2_DRAW_BUFFER5 = 0x882A;
const int WEBGL2_DRAW_BUFFER6 = 0x882B;
const int WEBGL2_DRAW_BUFFER7 = 0x882C;
const int WEBGL2_DRAW_BUFFER8 = 0x882D;
const int WEBGL2_DRAW_BUFFER9 = 0x882E;
const int WEBGL2_DRAW_BUFFER10 = 0x882F;
const int WEBGL2_DRAW_BUFFER11 = 0x8830;
const int WEBGL2_DRAW_BUFFER12 = 0x8831;
const int WEBGL2_DRAW_BUFFER13 = 0x8832;
const int WEBGL2_DRAW_BUFFER14 = 0x8833;
const int WEBGL2_DRAW_BUFFER15 = 0x8834;
const int WEBGL2_MAX_COLOR_ATTACHMENTS = 0x8CDF;
const int WEBGL2_COLOR_ATTACHMENT1 = 0x8CE1;
const int WEBGL2_COLOR_ATTACHMENT2 = 0x8CE2;
const int WEBGL2_COLOR_ATTACHMENT3 = 0x8CE3;
const int WEBGL2_COLOR_ATTACHMENT4 = 0x8CE4;
const int WEBGL2_COLOR_ATTACHMENT5 = 0x8CE5;
const int WEBGL2_COLOR_ATTACHMENT6 = 0x8CE6;
const int WEBGL2_COLOR_ATTACHMENT7 = 0x8CE7;
const int WEBGL2_COLOR_ATTACHMENT8 = 0x8CE8;
const int WEBGL2_COLOR_ATTACHMENT9 = 0x8CE9;
const int WEBGL2_COLOR_ATTACHMENT10 = 0x8CEA;
const int WEBGL2_COLOR_ATTACHMENT11 = 0x8CEB;
const int WEBGL2_COLOR_ATTACHMENT12 = 0x8CEC;
const int WEBGL2_COLOR_ATTACHMENT13 = 0x8CED;
const int WEBGL2_COLOR_ATTACHMENT14 = 0x8CEE;
const int WEBGL2_COLOR_ATTACHMENT15 = 0x8CEF;

// Samplers

const int WEBGL2_SAMPLER_3D = 0x8B5F;
const int WEBGL2_SAMPLER_2D_SHADOW = 0x8B62;
const int WEBGL2_SAMPLER_2D_ARRAY = 0x8DC1;
const int WEBGL2_SAMPLER_2D_ARRAY_SHADOW = 0x8DC4;
const int WEBGL2_SAMPLER_CUBE_SHADOW = 0x8DC5;
const int WEBGL2_INT_SAMPLER_2D = 0x8DCA;
const int WEBGL2_INT_SAMPLER_3D = 0x8DCB;
const int WEBGL2_INT_SAMPLER_CUBE = 0x8DCC;
const int WEBGL2_INT_SAMPLER_2D_ARRAY = 0x8DCF;
const int WEBGL2_UNSIGNED_INT_SAMPLER_2D = 0x8DD2;
const int WEBGL2_UNSIGNED_INT_SAMPLER_3D = 0x8DD3;
const int WEBGL2_UNSIGNED_INT_SAMPLER_CUBE = 0x8DD4;
const int WEBGL2_UNSIGNED_INT_SAMPLER_2D_ARRAY = 0x8DD7;
const int WEBGL2_MAX_SAMPLES = 0x8D57;
const int WEBGL2_SAMPLER_BINDING = 0x8919;

// Buffers

const int WEBGL2_PIXEL_PACK_BUFFER = 0x88EB;
const int WEBGL2_PIXEL_UNPACK_BUFFER = 0x88EC;
const int WEBGL2_PIXEL_PACK_BUFFER_BINDING = 0x88ED;
const int WEBGL2_PIXEL_UNPACK_BUFFER_BINDING = 0x88EF;
const int WEBGL2_COPY_READ_BUFFER = 0x8F36;
const int WEBGL2_COPY_WRITE_BUFFER = 0x8F37;
const int WEBGL2_COPY_READ_BUFFER_BINDING = 0x8F36;
const int WEBGL2_COPY_WRITE_BUFFER_BINDING = 0x8F37;

// Data types

const int WEBGL2_FLOAT_MAT2x3 = 0x8B65;
const int WEBGL2_FLOAT_MAT2x4 = 0x8B66;
const int WEBGL2_FLOAT_MAT3x2 = 0x8B67;
const int WEBGL2_FLOAT_MAT3x4 = 0x8B68;
const int WEBGL2_FLOAT_MAT4x2 = 0x8B69;
const int WEBGL2_FLOAT_MAT4x3 = 0x8B6A;
const int WEBGL2_UNSIGNED_INT_VEC2 = 0x8DC6;
const int WEBGL2_UNSIGNED_INT_VEC3 = 0x8DC7;
const int WEBGL2_UNSIGNED_INT_VEC4 = 0x8DC8;
const int WEBGL2_UNSIGNED_NORMALIZED = 0x8C17;
const int WEBGL2_SIGNED_NORMALIZED = 0x8F9C;

// Vertex attributes

const int WEBGL2_VERTEX_ATTRIB_ARRAY_INTEGER = 0x88FD;
const int WEBGL2_VERTEX_ATTRIB_ARRAY_DIVISOR = 0x88FE;

// Transform feedback

const int WEBGL2_TRANSFORM_FEEDBACK_BUFFER_MODE = 0x8C7F;
const int WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80;
const int WEBGL2_TRANSFORM_FEEDBACK_VARYINGS = 0x8C83;
const int WEBGL2_TRANSFORM_FEEDBACK_BUFFER_START = 0x8C84;
const int WEBGL2_TRANSFORM_FEEDBACK_BUFFER_SIZE = 0x8C85;
const int WEBGL2_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88;
const int WEBGL2_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A;
const int WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B;
const int WEBGL2_INTERLEAVED_ATTRIBS = 0x8C8C;
const int WEBGL2_SEPARATE_ATTRIBS = 0x8C8D;
const int WEBGL2_TRANSFORM_FEEDBACK_BUFFER = 0x8C8E;
const int WEBGL2_TRANSFORM_FEEDBACK_BUFFER_BINDING = 0x8C8F;
const int WEBGL2_TRANSFORM_FEEDBACK = 0x8E22;
const int WEBGL2_TRANSFORM_FEEDBACK_PAUSED = 0x8E23;
const int WEBGL2_TRANSFORM_FEEDBACK_ACTIVE = 0x8E24;
const int WEBGL2_TRANSFORM_FEEDBACK_BINDING = 0x8E25;

// Framebuffers and renderbuffers

const int WEBGL2_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING = 0x8210;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE = 0x8211;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_RED_SIZE = 0x8212;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE = 0x8213;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE = 0x8214;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE = 0x8215;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE = 0x8216;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE = 0x8217;
const int WEBGL2_FRAMEBUFFER_DEFAULT = 0x8218;
const int WEBGL2_DEPTH_STENCIL_ATTACHMENT = 0x821A;
const int WEBGL2_DEPTH_STENCIL = 0x84F9;
const int WEBGL2_DEPTH24_STENCIL8 = 0x88F0;
const int WEBGL2_DRAW_FRAMEBUFFER_BINDING = 0x8CA6;
const int WEBGL2_READ_FRAMEBUFFER = 0x8CA8;
const int WEBGL2_DRAW_FRAMEBUFFER = 0x8CA9;
const int WEBGL2_READ_FRAMEBUFFER_BINDING = 0x8CAA;
const int WEBGL2_RENDERBUFFER_SAMPLES = 0x8CAB;
const int WEBGL2_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER = 0x8CD4;
const int WEBGL2_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE = 0x8D56;

// Uniforms

const int WEBGL2_UNIFORM_BUFFER = 0x8A11;
const int WEBGL2_UNIFORM_BUFFER_BINDING = 0x8A28;
const int WEBGL2_UNIFORM_BUFFER_START = 0x8A29;
const int WEBGL2_UNIFORM_BUFFER_SIZE = 0x8A2A;
const int WEBGL2_MAX_VERTEX_UNIFORM_BLOCKS = 0x8A2B;
const int WEBGL2_MAX_FRAGMENT_UNIFORM_BLOCKS = 0x8A2D;
const int WEBGL2_MAX_COMBINED_UNIFORM_BLOCKS = 0x8A2E;
const int WEBGL2_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F;
const int WEBGL2_MAX_UNIFORM_BLOCK_SIZE = 0x8A30;
const int WEBGL2_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = 0x8A31;
const int WEBGL2_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33;
const int WEBGL2_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34;
const int WEBGL2_ACTIVE_UNIFORM_BLOCKS = 0x8A36;
const int WEBGL2_UNIFORM_TYPE = 0x8A37;
const int WEBGL2_UNIFORM_SIZE = 0x8A38;
const int WEBGL2_UNIFORM_BLOCK_INDEX = 0x8A3A;
const int WEBGL2_UNIFORM_OFFSET = 0x8A3B;
const int WEBGL2_UNIFORM_ARRAY_STRIDE = 0x8A3C;
const int WEBGL2_UNIFORM_MATRIX_STRIDE = 0x8A3D;
const int WEBGL2_UNIFORM_IS_ROW_MAJOR = 0x8A3E;
const int WEBGL2_UNIFORM_BLOCK_BINDING = 0x8A3F;
const int WEBGL2_UNIFORM_BLOCK_DATA_SIZE = 0x8A40;
const int WEBGL2_UNIFORM_BLOCK_ACTIVE_UNIFORMS = 0x8A42;
const int WEBGL2_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES = 0x8A43;
const int WEBGL2_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER = 0x8A44;
const int WEBGL2_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER = 0x8A46;

// Sync objects

const int WEBGL2_OBJECT_TYPE = 0x9112;
const int WEBGL2_SYNC_CONDITION = 0x9113;
const int WEBGL2_SYNC_STATUS = 0x9114;
const int WEBGL2_SYNC_FLAGS = 0x9115;
const int WEBGL2_SYNC_FENCE = 0x9116;
const int WEBGL2_SYNC_GPU_COMMANDS_COMPLETE = 0x9117;
const int WEBGL2_UNSIGNALED = 0x9118;
const int WEBGL2_SIGNALED = 0x9119;
const int WEBGL2_ALREADY_SIGNALED = 0x911A;
const int WEBGL2_TIMEOUT_EXPIRED = 0x911B;
const int WEBGL2_CONDITION_SATISFIED = 0x911C;
const int WEBGL2_WAIT_FAILED = 0x911D;
const int WEBGL2_SYNC_FLUSH_COMMANDS_BIT = 0x00000001;

// Miscellaneous

const int WEBGL2_COLOR = 0x1800;
const int WEBGL2_DEPTH = 0x1801;
const int WEBGL2_STENCIL = 0x1802;
const int WEBGL2_MIN = 0x8007;
const int WEBGL2_MAX = 0x8008;
const int WEBGL2_DEPTH_COMPONENT24 = 0x81A6;
const int WEBGL2_STREAM_READ = 0x88E1;
const int WEBGL2_STREAM_COPY = 0x88E2;
const int WEBGL2_STATIC_READ = 0x88E5;
const int WEBGL2_STATIC_COPY = 0x88E6;
const int WEBGL2_DYNAMIC_READ = 0x88E9;
const int WEBGL2_DYNAMIC_COPY = 0x88EA;
const int WEBGL2_DEPTH_COMPONENT32F = 0x8CAC;
const int WEBGL2_DEPTH32F_STENCIL8 = 0x8CAD;
const int WEBGL2_INVALID_INDEX = 0xFFFFFFFF;
const int WEBGL2_TIMEOUT_IGNORED = -1;
const int WEBGL2_MAX_CLIENT_WAIT_TIMEOUT_WEBGL = 0x9247;
