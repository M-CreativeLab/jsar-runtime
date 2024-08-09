#include "rendering_context.hpp"
#include "../canvas/image_bitmap.hpp"
#include "../canvas/image_data.hpp"
#include "../canvas/canvas.hpp"

#include "idgen.hpp"
#include "placeholders.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "uniform_location.hpp"

using namespace std;
using namespace node;
using namespace commandbuffers;
using namespace bindings::canvas;
using namespace canvasbinding;

namespace webgl
{
  Napi::FunctionReference *WebGLRenderingContext::webglConstructor;
  Napi::FunctionReference *WebGL2RenderingContext::webgl2Constructor;

  /**
   * The id generators for the client-side.
   */
  static TrIdGenerator programIdGen(1);
  static TrIdGenerator shaderIdGen(1);
  static TrIdGenerator bufferIdGen(1);
  static TrIdGenerator framebufferIdGen(1);
  static TrIdGenerator renderbufferIdGen(1);
  static TrIdGenerator vaoIdGen(1);
  static TrIdGenerator textureIdGen(1);

#define TBASE(T) WebGLBaseRenderingContext<T>

#define ADD_WEBGL_CONSTANT(name)                              \
  InstanceValue(#name, Napi::Number::New(env, WEBGL_##name)), \
      StaticValue(#name, Napi::Number::New(env, WEBGL_##name)),

#define ADD_WEBGL2_CONSTANT(name)                              \
  InstanceValue(#name, Napi::Number::New(env, WEBGL2_##name)), \
      StaticValue(#name, Napi::Number::New(env, WEBGL2_##name)),

#define WEBGL1_CONSTANTS_CLEARING_BUFFERS \
  ADD_WEBGL_CONSTANT(DEPTH_BUFFER_BIT)    \
  ADD_WEBGL_CONSTANT(STENCIL_BUFFER_BIT)  \
  ADD_WEBGL_CONSTANT(COLOR_BUFFER_BIT)

#define WEBGL1_CONSTANTS_RENDERING_PRIMITIVES \
  ADD_WEBGL_CONSTANT(POINTS)                  \
  ADD_WEBGL_CONSTANT(LINES)                   \
  ADD_WEBGL_CONSTANT(LINE_LOOP)               \
  ADD_WEBGL_CONSTANT(LINE_STRIP)              \
  ADD_WEBGL_CONSTANT(TRIANGLES)               \
  ADD_WEBGL_CONSTANT(TRIANGLE_STRIP)          \
  ADD_WEBGL_CONSTANT(TRIANGLE_FAN)

#define WEBGL1_CONSTANTS_BLENDING                      \
  ADD_WEBGL_CONSTANT(ZERO)                             \
  ADD_WEBGL_CONSTANT(ONE)                              \
  ADD_WEBGL_CONSTANT(SRC_COLOR)                        \
  ADD_WEBGL_CONSTANT(ONE_MINUS_SRC_COLOR)              \
  ADD_WEBGL_CONSTANT(SRC_ALPHA)                        \
  ADD_WEBGL_CONSTANT(ONE_MINUS_SRC_ALPHA)              \
  ADD_WEBGL_CONSTANT(DST_ALPHA)                        \
  ADD_WEBGL_CONSTANT(ONE_MINUS_DST_ALPHA)              \
  ADD_WEBGL_CONSTANT(DST_COLOR)                        \
  ADD_WEBGL_CONSTANT(ONE_MINUS_DST_COLOR)              \
  ADD_WEBGL_CONSTANT(SRC_ALPHA_SATURATE)               \
  ADD_WEBGL_CONSTANT(CONSTANT_COLOR)                   \
  ADD_WEBGL_CONSTANT(ONE_MINUS_CONSTANT_COLOR)         \
  ADD_WEBGL_CONSTANT(CONSTANT_ALPHA)                   \
  ADD_WEBGL_CONSTANT(ONE_MINUS_CONSTANT_ALPHA)         \
  ADD_WEBGL_CONSTANT(FUNC_ADD)                         \
  ADD_WEBGL_CONSTANT(FUNC_SUBTRACT)                    \
  ADD_WEBGL_CONSTANT(FUNC_REVERSE_SUBTRACT)            \
  ADD_WEBGL_CONSTANT(BLEND_EQUATION)                   \
  ADD_WEBGL_CONSTANT(BLEND_EQUATION_RGB)               \
  ADD_WEBGL_CONSTANT(BLEND_EQUATION_ALPHA)             \
  ADD_WEBGL_CONSTANT(BLEND_DST_RGB)                    \
  ADD_WEBGL_CONSTANT(BLEND_SRC_RGB)                    \
  ADD_WEBGL_CONSTANT(BLEND_DST_ALPHA)                  \
  ADD_WEBGL_CONSTANT(BLEND_SRC_ALPHA)                  \
  ADD_WEBGL_CONSTANT(BLEND_COLOR)                      \
  ADD_WEBGL_CONSTANT(ARRAY_BUFFER_BINDING)             \
  ADD_WEBGL_CONSTANT(ELEMENT_ARRAY_BUFFER_BINDING)     \
  ADD_WEBGL_CONSTANT(LINE_WIDTH)                       \
  ADD_WEBGL_CONSTANT(ALIASED_POINT_SIZE_RANGE)         \
  ADD_WEBGL_CONSTANT(ALIASED_LINE_WIDTH_RANGE)         \
  ADD_WEBGL_CONSTANT(CULL_FACE_MODE)                   \
  ADD_WEBGL_CONSTANT(FRONT_FACE)                       \
  ADD_WEBGL_CONSTANT(DEPTH_RANGE)                      \
  ADD_WEBGL_CONSTANT(DEPTH_WRITEMASK)                  \
  ADD_WEBGL_CONSTANT(DEPTH_CLEAR_VALUE)                \
  ADD_WEBGL_CONSTANT(DEPTH_FUNC)                       \
  ADD_WEBGL_CONSTANT(STENCIL_CLEAR_VALUE)              \
  ADD_WEBGL_CONSTANT(STENCIL_FUNC)                     \
  ADD_WEBGL_CONSTANT(STENCIL_FAIL)                     \
  ADD_WEBGL_CONSTANT(STENCIL_PASS_DEPTH_FAIL)          \
  ADD_WEBGL_CONSTANT(STENCIL_PASS_DEPTH_PASS)          \
  ADD_WEBGL_CONSTANT(STENCIL_REF)                      \
  ADD_WEBGL_CONSTANT(STENCIL_VALUE_MASK)               \
  ADD_WEBGL_CONSTANT(STENCIL_WRITEMASK)                \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_FUNC)                \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_FAIL)                \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_PASS_DEPTH_FAIL)     \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_PASS_DEPTH_PASS)     \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_REF)                 \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_VALUE_MASK)          \
  ADD_WEBGL_CONSTANT(STENCIL_BACK_WRITEMASK)           \
  ADD_WEBGL_CONSTANT(VIEWPORT)                         \
  ADD_WEBGL_CONSTANT(SCISSOR_BOX)                      \
  ADD_WEBGL_CONSTANT(COLOR_CLEAR_VALUE)                \
  ADD_WEBGL_CONSTANT(COLOR_WRITEMASK)                  \
  ADD_WEBGL_CONSTANT(UNPACK_ALIGNMENT)                 \
  ADD_WEBGL_CONSTANT(PACK_ALIGNMENT)                   \
  ADD_WEBGL_CONSTANT(MAX_TEXTURE_SIZE)                 \
  ADD_WEBGL_CONSTANT(MAX_VIEWPORT_DIMS)                \
  ADD_WEBGL_CONSTANT(SUBPIXEL_BITS)                    \
  ADD_WEBGL_CONSTANT(RED_BITS)                         \
  ADD_WEBGL_CONSTANT(GREEN_BITS)                       \
  ADD_WEBGL_CONSTANT(BLUE_BITS)                        \
  ADD_WEBGL_CONSTANT(ALPHA_BITS)                       \
  ADD_WEBGL_CONSTANT(DEPTH_BITS)                       \
  ADD_WEBGL_CONSTANT(STENCIL_BITS)                     \
  ADD_WEBGL_CONSTANT(POLYGON_OFFSET_UNITS)             \
  ADD_WEBGL_CONSTANT(POLYGON_OFFSET_FACTOR)            \
  ADD_WEBGL_CONSTANT(TEXTURE_BINDING_2D)               \
  ADD_WEBGL_CONSTANT(SAMPLE_BUFFERS)                   \
  ADD_WEBGL_CONSTANT(SAMPLES)                          \
  ADD_WEBGL_CONSTANT(SAMPLE_COVERAGE_VALUE)            \
  ADD_WEBGL_CONSTANT(SAMPLE_COVERAGE_INVERT)           \
  ADD_WEBGL_CONSTANT(COMPRESSED_TEXTURE_FORMATS)       \
  ADD_WEBGL_CONSTANT(VENDOR)                           \
  ADD_WEBGL_CONSTANT(RENDERER)                         \
  ADD_WEBGL_CONSTANT(VERSION)                          \
  ADD_WEBGL_CONSTANT(IMPLEMENTATION_COLOR_READ_TYPE)   \
  ADD_WEBGL_CONSTANT(IMPLEMENTATION_COLOR_READ_FORMAT) \
  ADD_WEBGL_CONSTANT(BROWSER_DEFAULT_WEBGL)

#define WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS     \
  ADD_WEBGL_CONSTANT(STATIC_DRAW)                    \
  ADD_WEBGL_CONSTANT(STREAM_DRAW)                    \
  ADD_WEBGL_CONSTANT(DYNAMIC_DRAW)                   \
  ADD_WEBGL_CONSTANT(ARRAY_BUFFER)                   \
  ADD_WEBGL_CONSTANT(ELEMENT_ARRAY_BUFFER)           \
  ADD_WEBGL_CONSTANT(BUFFER_SIZE)                    \
  ADD_WEBGL_CONSTANT(BUFFER_USAGE)                   \
  ADD_WEBGL_CONSTANT(CURRENT_VERTEX_ATTRIB)          \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_ENABLED)    \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_SIZE)       \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_STRIDE)     \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_TYPE)       \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_NORMALIZED) \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_POINTER)    \
  ADD_WEBGL_CONSTANT(VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)

#define WEBGL1_CONSTANTS_CULLING \
  ADD_WEBGL_CONSTANT(CULL_FACE)  \
  ADD_WEBGL_CONSTANT(FRONT)      \
  ADD_WEBGL_CONSTANT(BACK)       \
  ADD_WEBGL_CONSTANT(FRONT_AND_BACK)

#define WEBGL1_CONSTANTS_ENABLE_AND_DISABLE    \
  ADD_WEBGL_CONSTANT(BLEND)                    \
  ADD_WEBGL_CONSTANT(DEPTH_TEST)               \
  ADD_WEBGL_CONSTANT(DITHER)                   \
  ADD_WEBGL_CONSTANT(POLYGON_OFFSET_FILL)      \
  ADD_WEBGL_CONSTANT(SAMPLE_ALPHA_TO_COVERAGE) \
  ADD_WEBGL_CONSTANT(SAMPLE_COVERAGE)          \
  ADD_WEBGL_CONSTANT(SCISSOR_TEST)             \
  ADD_WEBGL_CONSTANT(STENCIL_TEST)

#define WEBGL1_CONSTANTS_ERRORS         \
  ADD_WEBGL_CONSTANT(NO_ERROR)          \
  ADD_WEBGL_CONSTANT(INVALID_ENUM)      \
  ADD_WEBGL_CONSTANT(INVALID_VALUE)     \
  ADD_WEBGL_CONSTANT(INVALID_OPERATION) \
  ADD_WEBGL_CONSTANT(OUT_OF_MEMORY)     \
  ADD_WEBGL_CONSTANT(CONTEXT_LOST_WEBGL)

#define WEBGL1_CONSTANTS_FRONTFACE \
  ADD_WEBGL_CONSTANT(CW)           \
  ADD_WEBGL_CONSTANT(CCW)

#define WEBGL1_CONSTANTS_HINTS  \
  ADD_WEBGL_CONSTANT(DONT_CARE) \
  ADD_WEBGL_CONSTANT(FASTEST)   \
  ADD_WEBGL_CONSTANT(NICEST)

#define WEBGL1_CONSTANTS_DATATYPES   \
  ADD_WEBGL_CONSTANT(BYTE)           \
  ADD_WEBGL_CONSTANT(UNSIGNED_BYTE)  \
  ADD_WEBGL_CONSTANT(SHORT)          \
  ADD_WEBGL_CONSTANT(UNSIGNED_SHORT) \
  ADD_WEBGL_CONSTANT(INT)            \
  ADD_WEBGL_CONSTANT(UNSIGNED_INT)   \
  ADD_WEBGL_CONSTANT(FLOAT)

#define WEBGL1_CONSTANTS_PIXELS                      \
  ADD_WEBGL_CONSTANT(DEPTH_COMPONENT)                \
  ADD_WEBGL_CONSTANT(ALPHA)                          \
  ADD_WEBGL_CONSTANT(RGB)                            \
  ADD_WEBGL_CONSTANT(RGBA)                           \
  ADD_WEBGL_CONSTANT(LUMINANCE)                      \
  ADD_WEBGL_CONSTANT(LUMINANCE_ALPHA)                \
  ADD_WEBGL_CONSTANT(UNSIGNED_SHORT_4_4_4_4)         \
  ADD_WEBGL_CONSTANT(UNSIGNED_SHORT_5_5_5_1)         \
  ADD_WEBGL_CONSTANT(UNSIGNED_SHORT_5_6_5)           \
  ADD_WEBGL_CONSTANT(UNPACK_FLIP_Y_WEBGL)            \
  ADD_WEBGL_CONSTANT(UNPACK_PREMULTIPLY_ALPHA_WEBGL) \
  ADD_WEBGL_CONSTANT(UNPACK_COLORSPACE_CONVERSION_WEBGL)

#define WEBGL1_CONSTANTS_SHADERS                       \
  ADD_WEBGL_CONSTANT(FRAGMENT_SHADER)                  \
  ADD_WEBGL_CONSTANT(VERTEX_SHADER)                    \
  ADD_WEBGL_CONSTANT(COMPILE_STATUS)                   \
  ADD_WEBGL_CONSTANT(DELETE_STATUS)                    \
  ADD_WEBGL_CONSTANT(LINK_STATUS)                      \
  ADD_WEBGL_CONSTANT(VALIDATE_STATUS)                  \
  ADD_WEBGL_CONSTANT(ATTACHED_SHADERS)                 \
  ADD_WEBGL_CONSTANT(ACTIVE_ATTRIBUTES)                \
  ADD_WEBGL_CONSTANT(ACTIVE_UNIFORMS)                  \
  ADD_WEBGL_CONSTANT(MAX_VERTEX_ATTRIBS)               \
  ADD_WEBGL_CONSTANT(MAX_VERTEX_UNIFORM_VECTORS)       \
  ADD_WEBGL_CONSTANT(MAX_VARYING_VECTORS)              \
  ADD_WEBGL_CONSTANT(MAX_COMBINED_TEXTURE_IMAGE_UNITS) \
  ADD_WEBGL_CONSTANT(MAX_VERTEX_TEXTURE_IMAGE_UNITS)   \
  ADD_WEBGL_CONSTANT(MAX_TEXTURE_IMAGE_UNITS)          \
  ADD_WEBGL_CONSTANT(MAX_FRAGMENT_UNIFORM_VECTORS)     \
  ADD_WEBGL_CONSTANT(SHADER_TYPE)                      \
  ADD_WEBGL_CONSTANT(SHADING_LANGUAGE_VERSION)         \
  ADD_WEBGL_CONSTANT(CURRENT_PROGRAM)                  \
  ADD_WEBGL_CONSTANT(LOW_FLOAT)                        \
  ADD_WEBGL_CONSTANT(MEDIUM_FLOAT)                     \
  ADD_WEBGL_CONSTANT(HIGH_FLOAT)                       \
  ADD_WEBGL_CONSTANT(LOW_INT)                          \
  ADD_WEBGL_CONSTANT(MEDIUM_INT)                       \
  ADD_WEBGL_CONSTANT(HIGH_INT)

#define WEBGL1_CONSTANTS_DEPTH_AND_STENCIL \
  ADD_WEBGL_CONSTANT(NEVER)                \
  ADD_WEBGL_CONSTANT(LESS)                 \
  ADD_WEBGL_CONSTANT(EQUAL)                \
  ADD_WEBGL_CONSTANT(LEQUAL)               \
  ADD_WEBGL_CONSTANT(GREATER)              \
  ADD_WEBGL_CONSTANT(NOTEQUAL)             \
  ADD_WEBGL_CONSTANT(GEQUAL)               \
  ADD_WEBGL_CONSTANT(ALWAYS)               \
  ADD_WEBGL_CONSTANT(KEEP)                 \
  ADD_WEBGL_CONSTANT(REPLACE)              \
  ADD_WEBGL_CONSTANT(INCR)                 \
  ADD_WEBGL_CONSTANT(DECR)                 \
  ADD_WEBGL_CONSTANT(INVERT)               \
  ADD_WEBGL_CONSTANT(INCR_WRAP)            \
  ADD_WEBGL_CONSTANT(DECR_WRAP)

#define WEBGL1_CONSTANTS_TEXTURE                  \
  ADD_WEBGL_CONSTANT(NEAREST)                     \
  ADD_WEBGL_CONSTANT(LINEAR)                      \
  ADD_WEBGL_CONSTANT(NEAREST_MIPMAP_NEAREST)      \
  ADD_WEBGL_CONSTANT(LINEAR_MIPMAP_NEAREST)       \
  ADD_WEBGL_CONSTANT(NEAREST_MIPMAP_LINEAR)       \
  ADD_WEBGL_CONSTANT(LINEAR_MIPMAP_LINEAR)        \
  ADD_WEBGL_CONSTANT(TEXTURE_MAG_FILTER)          \
  ADD_WEBGL_CONSTANT(TEXTURE_MIN_FILTER)          \
  ADD_WEBGL_CONSTANT(TEXTURE_WRAP_S)              \
  ADD_WEBGL_CONSTANT(TEXTURE_WRAP_T)              \
  ADD_WEBGL_CONSTANT(TEXTURE_2D)                  \
  ADD_WEBGL_CONSTANT(TEXTURE)                     \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP)            \
  ADD_WEBGL_CONSTANT(TEXTURE_BINDING_CUBE_MAP)    \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_X) \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_X) \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_Y) \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_Y) \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_Z) \
  ADD_WEBGL_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_Z) \
  ADD_WEBGL_CONSTANT(MAX_CUBE_MAP_TEXTURE_SIZE)   \
  ADD_WEBGL_CONSTANT(TEXTURE0)                    \
  ADD_WEBGL_CONSTANT(TEXTURE1)                    \
  ADD_WEBGL_CONSTANT(TEXTURE2)                    \
  ADD_WEBGL_CONSTANT(TEXTURE3)                    \
  ADD_WEBGL_CONSTANT(TEXTURE4)                    \
  ADD_WEBGL_CONSTANT(TEXTURE5)                    \
  ADD_WEBGL_CONSTANT(TEXTURE6)                    \
  ADD_WEBGL_CONSTANT(TEXTURE7)                    \
  ADD_WEBGL_CONSTANT(TEXTURE8)                    \
  ADD_WEBGL_CONSTANT(TEXTURE9)                    \
  ADD_WEBGL_CONSTANT(TEXTURE10)                   \
  ADD_WEBGL_CONSTANT(TEXTURE11)                   \
  ADD_WEBGL_CONSTANT(TEXTURE12)                   \
  ADD_WEBGL_CONSTANT(TEXTURE13)                   \
  ADD_WEBGL_CONSTANT(TEXTURE14)                   \
  ADD_WEBGL_CONSTANT(TEXTURE15)                   \
  ADD_WEBGL_CONSTANT(TEXTURE16)                   \
  ADD_WEBGL_CONSTANT(TEXTURE17)                   \
  ADD_WEBGL_CONSTANT(TEXTURE18)                   \
  ADD_WEBGL_CONSTANT(TEXTURE19)                   \
  ADD_WEBGL_CONSTANT(TEXTURE20)                   \
  ADD_WEBGL_CONSTANT(TEXTURE21)                   \
  ADD_WEBGL_CONSTANT(TEXTURE22)                   \
  ADD_WEBGL_CONSTANT(TEXTURE23)                   \
  ADD_WEBGL_CONSTANT(TEXTURE24)                   \
  ADD_WEBGL_CONSTANT(TEXTURE25)                   \
  ADD_WEBGL_CONSTANT(TEXTURE26)                   \
  ADD_WEBGL_CONSTANT(TEXTURE27)                   \
  ADD_WEBGL_CONSTANT(TEXTURE28)                   \
  ADD_WEBGL_CONSTANT(TEXTURE29)                   \
  ADD_WEBGL_CONSTANT(TEXTURE30)                   \
  ADD_WEBGL_CONSTANT(TEXTURE31)                   \
  ADD_WEBGL_CONSTANT(ACTIVE_TEXTURE)              \
  ADD_WEBGL_CONSTANT(REPEAT)                      \
  ADD_WEBGL_CONSTANT(CLAMP_TO_EDGE)               \
  ADD_WEBGL_CONSTANT(MIRRORED_REPEAT)

#define WEBGL1_CONSTANTS_UNIFORM_TYPES \
  ADD_WEBGL_CONSTANT(FLOAT_VEC2)       \
  ADD_WEBGL_CONSTANT(FLOAT_VEC3)       \
  ADD_WEBGL_CONSTANT(FLOAT_VEC4)       \
  ADD_WEBGL_CONSTANT(INT_VEC2)         \
  ADD_WEBGL_CONSTANT(INT_VEC3)         \
  ADD_WEBGL_CONSTANT(INT_VEC4)         \
  ADD_WEBGL_CONSTANT(BOOL)             \
  ADD_WEBGL_CONSTANT(BOOL_VEC2)        \
  ADD_WEBGL_CONSTANT(BOOL_VEC3)        \
  ADD_WEBGL_CONSTANT(BOOL_VEC4)        \
  ADD_WEBGL_CONSTANT(FLOAT_MAT2)       \
  ADD_WEBGL_CONSTANT(FLOAT_MAT3)       \
  ADD_WEBGL_CONSTANT(FLOAT_MAT4)       \
  ADD_WEBGL_CONSTANT(SAMPLER_2D)       \
  ADD_WEBGL_CONSTANT(SAMPLER_CUBE)

#define WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER              \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER)                                  \
  ADD_WEBGL_CONSTANT(RENDERBUFFER)                                 \
  ADD_WEBGL_CONSTANT(RGBA4)                                        \
  ADD_WEBGL_CONSTANT(RGB5_A1)                                      \
  ADD_WEBGL_CONSTANT(RGB565)                                       \
  ADD_WEBGL_CONSTANT(DEPTH_COMPONENT16)                            \
  ADD_WEBGL_CONSTANT(STENCIL_INDEX8)                               \
  ADD_WEBGL_CONSTANT(DEPTH_STENCIL)                                \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_WIDTH)                           \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_HEIGHT)                          \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_INTERNAL_FORMAT)                 \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_RED_SIZE)                        \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_GREEN_SIZE)                      \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_BLUE_SIZE)                       \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_ALPHA_SIZE)                      \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_DEPTH_SIZE)                      \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_STENCIL_SIZE)                    \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE)           \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_ATTACHMENT_OBJECT_NAME)           \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL)         \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE) \
  ADD_WEBGL_CONSTANT(COLOR_ATTACHMENT0)                            \
  ADD_WEBGL_CONSTANT(DEPTH_ATTACHMENT)                             \
  ADD_WEBGL_CONSTANT(STENCIL_ATTACHMENT)                           \
  ADD_WEBGL_CONSTANT(DEPTH_STENCIL_ATTACHMENT)                     \
  ADD_WEBGL_CONSTANT(NONE)                                         \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_COMPLETE)                         \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_INCOMPLETE_ATTACHMENT)            \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)    \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_INCOMPLETE_DIMENSIONS)            \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_UNSUPPORTED)                      \
  ADD_WEBGL_CONSTANT(FRAMEBUFFER_BINDING)                          \
  ADD_WEBGL_CONSTANT(RENDERBUFFER_BINDING)                         \
  ADD_WEBGL_CONSTANT(MAX_RENDERBUFFER_SIZE)                        \
  ADD_WEBGL_CONSTANT(INVALID_FRAMEBUFFER_OPERATION)

#define WEBGL1_CONSTANTS                        \
  WEBGL1_CONSTANTS_CLEARING_BUFFERS             \
  WEBGL1_CONSTANTS_RENDERING_PRIMITIVES         \
  WEBGL1_CONSTANTS_BLENDING                     \
  WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS      \
  WEBGL1_CONSTANTS_CULLING                      \
  WEBGL1_CONSTANTS_ENABLE_AND_DISABLE           \
  WEBGL1_CONSTANTS_ERRORS                       \
  WEBGL1_CONSTANTS_FRONTFACE                    \
  WEBGL1_CONSTANTS_HINTS                        \
  WEBGL1_CONSTANTS_DATATYPES                    \
  WEBGL1_CONSTANTS_PIXELS                       \
  WEBGL1_CONSTANTS_SHADERS                      \
  WEBGL1_CONSTANTS_DEPTH_AND_STENCIL            \
  WEBGL1_CONSTANTS_TEXTURE                      \
  WEBGL1_CONSTANTS_UNIFORM_TYPES                \
  WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER \
  InstanceValue("__webgl1_constants__", Napi::Boolean::New(env, true))

#define WEBGL2_CONSTANTS_PARAMETERS                    \
  ADD_WEBGL2_CONSTANT(READ_BUFFER)                     \
  ADD_WEBGL2_CONSTANT(UNPACK_ROW_LENGTH)               \
  ADD_WEBGL2_CONSTANT(UNPACK_SKIP_ROWS)                \
  ADD_WEBGL2_CONSTANT(UNPACK_SKIP_PIXELS)              \
  ADD_WEBGL2_CONSTANT(PACK_ROW_LENGTH)                 \
  ADD_WEBGL2_CONSTANT(PACK_SKIP_ROWS)                  \
  ADD_WEBGL2_CONSTANT(PACK_SKIP_PIXELS)                \
  ADD_WEBGL2_CONSTANT(TEXTURE_BINDING_3D)              \
  ADD_WEBGL2_CONSTANT(UNPACK_SKIP_IMAGES)              \
  ADD_WEBGL2_CONSTANT(UNPACK_IMAGE_HEIGHT)             \
  ADD_WEBGL2_CONSTANT(MAX_3D_TEXTURE_SIZE)             \
  ADD_WEBGL2_CONSTANT(MAX_ELEMENTS_VERTICES)           \
  ADD_WEBGL2_CONSTANT(MAX_ELEMENTS_INDICES)            \
  ADD_WEBGL2_CONSTANT(MAX_TEXTURE_LOD_BIAS)            \
  ADD_WEBGL2_CONSTANT(MAX_FRAGMENT_UNIFORM_COMPONENTS) \
  ADD_WEBGL2_CONSTANT(MAX_VERTEX_UNIFORM_COMPONENTS)   \
  ADD_WEBGL2_CONSTANT(MAX_ARRAY_TEXTURE_LAYERS)        \
  ADD_WEBGL2_CONSTANT(MIN_PROGRAM_TEXEL_OFFSET)        \
  ADD_WEBGL2_CONSTANT(MAX_PROGRAM_TEXEL_OFFSET)        \
  ADD_WEBGL2_CONSTANT(MAX_VARYING_COMPONENTS)          \
  ADD_WEBGL2_CONSTANT(FRAGMENT_SHADER_DERIVATIVE_HINT) \
  ADD_WEBGL2_CONSTANT(RASTERIZER_DISCARD)              \
  ADD_WEBGL2_CONSTANT(VERTEX_ARRAY_BINDING)            \
  ADD_WEBGL2_CONSTANT(MAX_VERTEX_OUTPUT_COMPONENTS)    \
  ADD_WEBGL2_CONSTANT(MAX_FRAGMENT_INPUT_COMPONENTS)   \
  ADD_WEBGL2_CONSTANT(MAX_SERVER_WAIT_TIMEOUT)         \
  ADD_WEBGL2_CONSTANT(MAX_ELEMENT_INDEX)

#define WEBGL2_CONSTANTS_TEXTURES               \
  ADD_WEBGL2_CONSTANT(RED)                      \
  ADD_WEBGL2_CONSTANT(RGB8)                     \
  ADD_WEBGL2_CONSTANT(RGBA8)                    \
  ADD_WEBGL2_CONSTANT(RGB10_A2)                 \
  ADD_WEBGL2_CONSTANT(TEXTURE_3D)               \
  ADD_WEBGL2_CONSTANT(TEXTURE_WRAP_R)           \
  ADD_WEBGL2_CONSTANT(TEXTURE_MIN_LOD)          \
  ADD_WEBGL2_CONSTANT(TEXTURE_MAX_LOD)          \
  ADD_WEBGL2_CONSTANT(TEXTURE_BASE_LEVEL)       \
  ADD_WEBGL2_CONSTANT(TEXTURE_MAX_LEVEL)        \
  ADD_WEBGL2_CONSTANT(TEXTURE_COMPARE_MODE)     \
  ADD_WEBGL2_CONSTANT(TEXTURE_COMPARE_FUNC)     \
  ADD_WEBGL2_CONSTANT(SRGB)                     \
  ADD_WEBGL2_CONSTANT(SRGB8)                    \
  ADD_WEBGL2_CONSTANT(SRGB8_ALPHA8)             \
  ADD_WEBGL2_CONSTANT(COMPARE_REF_TO_TEXTURE)   \
  ADD_WEBGL2_CONSTANT(RGBA32F)                  \
  ADD_WEBGL2_CONSTANT(RGB32F)                   \
  ADD_WEBGL2_CONSTANT(RGBA16F)                  \
  ADD_WEBGL2_CONSTANT(RGB16F)                   \
  ADD_WEBGL2_CONSTANT(TEXTURE_2D_ARRAY)         \
  ADD_WEBGL2_CONSTANT(TEXTURE_BINDING_2D_ARRAY) \
  ADD_WEBGL2_CONSTANT(R11F_G11F_B10F)           \
  ADD_WEBGL2_CONSTANT(RGB9_E5)                  \
  ADD_WEBGL2_CONSTANT(RGBA32UI)                 \
  ADD_WEBGL2_CONSTANT(RGB32UI)                  \
  ADD_WEBGL2_CONSTANT(RGBA16UI)                 \
  ADD_WEBGL2_CONSTANT(RGB16UI)                  \
  ADD_WEBGL2_CONSTANT(RGBA8UI)                  \
  ADD_WEBGL2_CONSTANT(RGB8UI)                   \
  ADD_WEBGL2_CONSTANT(RGBA32I)                  \
  ADD_WEBGL2_CONSTANT(RGB32I)                   \
  ADD_WEBGL2_CONSTANT(RGBA16I)                  \
  ADD_WEBGL2_CONSTANT(RGB16I)                   \
  ADD_WEBGL2_CONSTANT(RGBA8I)                   \
  ADD_WEBGL2_CONSTANT(RGB8I)                    \
  ADD_WEBGL2_CONSTANT(RED_INTEGER)              \
  ADD_WEBGL2_CONSTANT(RGB_INTEGER)              \
  ADD_WEBGL2_CONSTANT(RGBA_INTEGER)             \
  ADD_WEBGL2_CONSTANT(R8)                       \
  ADD_WEBGL2_CONSTANT(RG8)                      \
  ADD_WEBGL2_CONSTANT(R16F)                     \
  ADD_WEBGL2_CONSTANT(RG16F)                    \
  ADD_WEBGL2_CONSTANT(R32F)                     \
  ADD_WEBGL2_CONSTANT(RG32F)                    \
  ADD_WEBGL2_CONSTANT(R8I)                      \
  ADD_WEBGL2_CONSTANT(R8UI)                     \
  ADD_WEBGL2_CONSTANT(R16I)                     \
  ADD_WEBGL2_CONSTANT(R16UI)                    \
  ADD_WEBGL2_CONSTANT(R32I)                     \
  ADD_WEBGL2_CONSTANT(R32UI)                    \
  ADD_WEBGL2_CONSTANT(RG8I)                     \
  ADD_WEBGL2_CONSTANT(RG8UI)                    \
  ADD_WEBGL2_CONSTANT(RG16I)                    \
  ADD_WEBGL2_CONSTANT(RG16UI)                   \
  ADD_WEBGL2_CONSTANT(RG32I)                    \
  ADD_WEBGL2_CONSTANT(RG32UI)                   \
  ADD_WEBGL2_CONSTANT(R8_SNORM)                 \
  ADD_WEBGL2_CONSTANT(RG8_SNORM)                \
  ADD_WEBGL2_CONSTANT(RGB8_SNORM)               \
  ADD_WEBGL2_CONSTANT(RGBA8_SNORM)              \
  ADD_WEBGL2_CONSTANT(RGB10_A2UI)               \
  ADD_WEBGL2_CONSTANT(TEXTURE_IMMUTABLE_FORMAT) \
  ADD_WEBGL2_CONSTANT(TEXTURE_IMMUTABLE_LEVELS)

#define WEBGL2_CONSTANTS_PIXELTYPES                   \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_2_10_10_10_REV)    \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_10F_11F_11F_REV)   \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_5_9_9_9_REV)       \
  ADD_WEBGL2_CONSTANT(FLOAT_32_UNSIGNED_INT_24_8_REV) \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_24_8)              \
  ADD_WEBGL2_CONSTANT(HALF_FLOAT)                     \
  ADD_WEBGL2_CONSTANT(RG)                             \
  ADD_WEBGL2_CONSTANT(RG_INTEGER)                     \
  ADD_WEBGL2_CONSTANT(INT_2_10_10_10_REV)

#define WEBGL2_CONSTANTS_QUERIES              \
  ADD_WEBGL2_CONSTANT(CURRENT_QUERY)          \
  ADD_WEBGL2_CONSTANT(QUERY_RESULT)           \
  ADD_WEBGL2_CONSTANT(QUERY_RESULT_AVAILABLE) \
  ADD_WEBGL2_CONSTANT(ANY_SAMPLES_PASSED)     \
  ADD_WEBGL2_CONSTANT(ANY_SAMPLES_PASSED_CONSERVATIVE)

#define WEBGL2_CONSTANTS_DRAWBUFFERS         \
  ADD_WEBGL2_CONSTANT(MAX_DRAW_BUFFERS)      \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER0)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER1)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER2)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER3)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER4)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER5)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER6)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER7)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER8)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER9)          \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER10)         \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER11)         \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER12)         \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER13)         \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER14)         \
  ADD_WEBGL2_CONSTANT(DRAW_BUFFER15)         \
  ADD_WEBGL2_CONSTANT(MAX_COLOR_ATTACHMENTS) \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT1)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT2)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT3)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT4)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT5)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT6)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT7)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT8)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT9)     \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT10)    \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT11)    \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT12)    \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT13)    \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT14)    \
  ADD_WEBGL2_CONSTANT(COLOR_ATTACHMENT15)

#define WEBGL2_CONSTANTS_SAMPLERS                    \
  ADD_WEBGL2_CONSTANT(SAMPLER_3D)                    \
  ADD_WEBGL2_CONSTANT(SAMPLER_2D_SHADOW)             \
  ADD_WEBGL2_CONSTANT(SAMPLER_2D_ARRAY)              \
  ADD_WEBGL2_CONSTANT(SAMPLER_2D_ARRAY_SHADOW)       \
  ADD_WEBGL2_CONSTANT(SAMPLER_CUBE_SHADOW)           \
  ADD_WEBGL2_CONSTANT(INT_SAMPLER_2D)                \
  ADD_WEBGL2_CONSTANT(INT_SAMPLER_3D)                \
  ADD_WEBGL2_CONSTANT(INT_SAMPLER_CUBE)              \
  ADD_WEBGL2_CONSTANT(INT_SAMPLER_2D_ARRAY)          \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_SAMPLER_2D)       \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_SAMPLER_3D)       \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_SAMPLER_CUBE)     \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_SAMPLER_2D_ARRAY) \
  ADD_WEBGL2_CONSTANT(MAX_SAMPLES)                   \
  ADD_WEBGL2_CONSTANT(SAMPLER_BINDING)

#define WEBGL2_CONSTANTS_BUFFERS                   \
  ADD_WEBGL2_CONSTANT(PIXEL_PACK_BUFFER)           \
  ADD_WEBGL2_CONSTANT(PIXEL_UNPACK_BUFFER)         \
  ADD_WEBGL2_CONSTANT(PIXEL_PACK_BUFFER_BINDING)   \
  ADD_WEBGL2_CONSTANT(PIXEL_UNPACK_BUFFER_BINDING) \
  ADD_WEBGL2_CONSTANT(COPY_READ_BUFFER)            \
  ADD_WEBGL2_CONSTANT(COPY_WRITE_BUFFER)           \
  ADD_WEBGL2_CONSTANT(COPY_READ_BUFFER_BINDING)    \
  ADD_WEBGL2_CONSTANT(COPY_WRITE_BUFFER_BINDING)

#define WEBGL2_CONSTANTS_DATA_TYPES        \
  ADD_WEBGL2_CONSTANT(FLOAT_MAT2x3)        \
  ADD_WEBGL2_CONSTANT(FLOAT_MAT2x4)        \
  ADD_WEBGL2_CONSTANT(FLOAT_MAT3x2)        \
  ADD_WEBGL2_CONSTANT(FLOAT_MAT3x4)        \
  ADD_WEBGL2_CONSTANT(FLOAT_MAT4x2)        \
  ADD_WEBGL2_CONSTANT(FLOAT_MAT4x3)        \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_VEC2)   \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_VEC3)   \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_VEC4)   \
  ADD_WEBGL2_CONSTANT(UNSIGNED_NORMALIZED) \
  ADD_WEBGL2_CONSTANT(SIGNED_NORMALIZED)

#define WEBGL2_CONSTANTS_VERTEX_ATTRIBS            \
  ADD_WEBGL2_CONSTANT(VERTEX_ATTRIB_ARRAY_INTEGER) \
  ADD_WEBGL2_CONSTANT(VERTEX_ATTRIB_ARRAY_DIVISOR)

#define WEBGL2_CONSTANTS_TRANSFORM_FEEDBACK                          \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_BUFFER_MODE)                \
  ADD_WEBGL2_CONSTANT(MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS)    \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_VARYINGS)                   \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_BUFFER_START)               \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_BUFFER_SIZE)                \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)         \
  ADD_WEBGL2_CONSTANT(MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS) \
  ADD_WEBGL2_CONSTANT(MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS)       \
  ADD_WEBGL2_CONSTANT(INTERLEAVED_ATTRIBS)                           \
  ADD_WEBGL2_CONSTANT(SEPARATE_ATTRIBS)                              \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_BUFFER)                     \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_BUFFER_BINDING)             \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK)                            \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_PAUSED)                     \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_ACTIVE)                     \
  ADD_WEBGL2_CONSTANT(TRANSFORM_FEEDBACK_BINDING)

#define WEBGL2_CONSTANTS_FRAMEBUFFERS_AND_RENDERBUFFERS      \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING) \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE) \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_RED_SIZE)       \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_GREEN_SIZE)     \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_BLUE_SIZE)      \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE)     \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE)     \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE)   \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_DEFAULT)                   \
  ADD_WEBGL2_CONSTANT(DEPTH_STENCIL_ATTACHMENT)              \
  ADD_WEBGL2_CONSTANT(DEPTH_STENCIL)                         \
  ADD_WEBGL2_CONSTANT(DEPTH24_STENCIL8)                      \
  ADD_WEBGL2_CONSTANT(DRAW_FRAMEBUFFER_BINDING)              \
  ADD_WEBGL2_CONSTANT(READ_FRAMEBUFFER)                      \
  ADD_WEBGL2_CONSTANT(DRAW_FRAMEBUFFER)                      \
  ADD_WEBGL2_CONSTANT(READ_FRAMEBUFFER_BINDING)              \
  ADD_WEBGL2_CONSTANT(RENDERBUFFER_SAMPLES)                  \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER)  \
  ADD_WEBGL2_CONSTANT(FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)

#define WEBGL2_CONSTANTS_UNIFORMS                                \
  ADD_WEBGL2_CONSTANT(UNIFORM_BUFFER)                            \
  ADD_WEBGL2_CONSTANT(UNIFORM_BUFFER_BINDING)                    \
  ADD_WEBGL2_CONSTANT(UNIFORM_BUFFER_START)                      \
  ADD_WEBGL2_CONSTANT(UNIFORM_BUFFER_SIZE)                       \
  ADD_WEBGL2_CONSTANT(MAX_VERTEX_UNIFORM_BLOCKS)                 \
  ADD_WEBGL2_CONSTANT(MAX_FRAGMENT_UNIFORM_BLOCKS)               \
  ADD_WEBGL2_CONSTANT(MAX_COMBINED_UNIFORM_BLOCKS)               \
  ADD_WEBGL2_CONSTANT(MAX_UNIFORM_BUFFER_BINDINGS)               \
  ADD_WEBGL2_CONSTANT(MAX_UNIFORM_BLOCK_SIZE)                    \
  ADD_WEBGL2_CONSTANT(MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS)    \
  ADD_WEBGL2_CONSTANT(MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS)  \
  ADD_WEBGL2_CONSTANT(UNIFORM_BUFFER_OFFSET_ALIGNMENT)           \
  ADD_WEBGL2_CONSTANT(ACTIVE_UNIFORM_BLOCKS)                     \
  ADD_WEBGL2_CONSTANT(UNIFORM_TYPE)                              \
  ADD_WEBGL2_CONSTANT(UNIFORM_SIZE)                              \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_INDEX)                       \
  ADD_WEBGL2_CONSTANT(UNIFORM_OFFSET)                            \
  ADD_WEBGL2_CONSTANT(UNIFORM_ARRAY_STRIDE)                      \
  ADD_WEBGL2_CONSTANT(UNIFORM_MATRIX_STRIDE)                     \
  ADD_WEBGL2_CONSTANT(UNIFORM_IS_ROW_MAJOR)                      \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_BINDING)                     \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_DATA_SIZE)                   \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_ACTIVE_UNIFORMS)             \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES)      \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER) \
  ADD_WEBGL2_CONSTANT(UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER)

#define WEBGL2_CONSTANTS_SYNC_OBJECTS             \
  ADD_WEBGL2_CONSTANT(OBJECT_TYPE)                \
  ADD_WEBGL2_CONSTANT(SYNC_CONDITION)             \
  ADD_WEBGL2_CONSTANT(SYNC_STATUS)                \
  ADD_WEBGL2_CONSTANT(SYNC_FLAGS)                 \
  ADD_WEBGL2_CONSTANT(SYNC_FENCE)                 \
  ADD_WEBGL2_CONSTANT(SYNC_GPU_COMMANDS_COMPLETE) \
  ADD_WEBGL2_CONSTANT(UNSIGNALED)                 \
  ADD_WEBGL2_CONSTANT(SIGNALED)                   \
  ADD_WEBGL2_CONSTANT(ALREADY_SIGNALED)           \
  ADD_WEBGL2_CONSTANT(TIMEOUT_EXPIRED)            \
  ADD_WEBGL2_CONSTANT(CONDITION_SATISFIED)        \
  ADD_WEBGL2_CONSTANT(WAIT_FAILED)                \
  ADD_WEBGL2_CONSTANT(SYNC_FLUSH_COMMANDS_BIT)

#define WEBGL2_CONSTANTS_MISCELLANEOUS    \
  ADD_WEBGL2_CONSTANT(COLOR)              \
  ADD_WEBGL2_CONSTANT(DEPTH)              \
  ADD_WEBGL2_CONSTANT(STENCIL)            \
  ADD_WEBGL2_CONSTANT(MIN)                \
  ADD_WEBGL2_CONSTANT(MAX)                \
  ADD_WEBGL2_CONSTANT(DEPTH_COMPONENT24)  \
  ADD_WEBGL2_CONSTANT(STREAM_READ)        \
  ADD_WEBGL2_CONSTANT(STREAM_COPY)        \
  ADD_WEBGL2_CONSTANT(STATIC_READ)        \
  ADD_WEBGL2_CONSTANT(STATIC_COPY)        \
  ADD_WEBGL2_CONSTANT(DYNAMIC_READ)       \
  ADD_WEBGL2_CONSTANT(DYNAMIC_COPY)       \
  ADD_WEBGL2_CONSTANT(DEPTH_COMPONENT32F) \
  ADD_WEBGL2_CONSTANT(DEPTH32F_STENCIL8)  \
  ADD_WEBGL2_CONSTANT(INVALID_INDEX)      \
  ADD_WEBGL2_CONSTANT(TIMEOUT_IGNORED)    \
  ADD_WEBGL2_CONSTANT(MAX_CLIENT_WAIT_TIMEOUT_WEBGL)

#define WEBGL2_CONSTANTS                          \
  WEBGL2_CONSTANTS_PARAMETERS                     \
  WEBGL2_CONSTANTS_TEXTURES                       \
  WEBGL2_CONSTANTS_PIXELTYPES                     \
  WEBGL2_CONSTANTS_QUERIES                        \
  WEBGL2_CONSTANTS_DRAWBUFFERS                    \
  WEBGL2_CONSTANTS_SAMPLERS                       \
  WEBGL2_CONSTANTS_BUFFERS                        \
  WEBGL2_CONSTANTS_DATA_TYPES                     \
  WEBGL2_CONSTANTS_VERTEX_ATTRIBS                 \
  WEBGL2_CONSTANTS_TRANSFORM_FEEDBACK             \
  WEBGL2_CONSTANTS_FRAMEBUFFERS_AND_RENDERBUFFERS \
  WEBGL2_CONSTANTS_UNIFORMS                       \
  WEBGL2_CONSTANTS_SYNC_OBJECTS                   \
  WEBGL2_CONSTANTS_MISCELLANEOUS                  \
  InstanceValue("__webgl2_constants__", Napi::Boolean::New(env, true))

#define WEBGL1_ACCESSORS(T)                                                                                         \
  InstanceAccessor("drawingBufferWidth", &TBASE(T)::DrawingBufferWidthGetter, &TBASE(T)::DrawingBufferWidthSetter), \
      InstanceAccessor("drawingBufferHeight", &TBASE(T)::DrawingBufferHeightGetter, &TBASE(T)::DrawingBufferHeightSetter)

#define WEBGL1_METHODS(T)                                                       \
  InstanceMethod("makeXRCompatible", &T::MakeXRCompatible),                     \
      InstanceMethod("getContextAttributes", &T::GetContextAttributes),         \
      InstanceMethod("createProgram", &T::CreateProgram),                       \
      InstanceMethod("deleteProgram", &T::DeleteProgram),                       \
      InstanceMethod("linkProgram", &T::LinkProgram),                           \
      InstanceMethod("useProgram", &T::UseProgram),                             \
      InstanceMethod("getProgramParameter", &T::GetProgramParameter),           \
      InstanceMethod("getProgramInfoLog", &T::GetProgramInfoLog),               \
      InstanceMethod("attachShader", &T::AttachShader),                         \
      InstanceMethod("detachShader", &T::DetachShader),                         \
      InstanceMethod("createShader", &T::CreateShader),                         \
      InstanceMethod("deleteShader", &T::DeleteShader),                         \
      InstanceMethod("shaderSource", &T::ShaderSource),                         \
      InstanceMethod("compileShader", &T::CompileShader),                       \
      InstanceMethod("getShaderSource", &T::GetShaderSource),                   \
      InstanceMethod("getShaderParameter", &T::GetShaderParameter),             \
      InstanceMethod("getShaderInfoLog", &T::GetShaderInfoLog),                 \
      InstanceMethod("createBuffer", &T::CreateBuffer),                         \
      InstanceMethod("deleteBuffer", &T::DeleteBuffer),                         \
      InstanceMethod("bindBuffer", &T::BindBuffer),                             \
      InstanceMethod("bufferData", &T::BufferData),                             \
      InstanceMethod("bufferSubData", &T::BufferSubData),                       \
      InstanceMethod("createFramebuffer", &T::CreateFramebuffer),               \
      InstanceMethod("deleteFramebuffer", &T::DeleteFramebuffer),               \
      InstanceMethod("bindFramebuffer", &T::BindFramebuffer),                   \
      InstanceMethod("framebufferRenderbuffer", &T::FramebufferRenderbuffer),   \
      InstanceMethod("framebufferTexture2D", &T::FramebufferTexture2D),         \
      InstanceMethod("checkFramebufferStatus", &T::CheckFramebufferStatus),     \
      InstanceMethod("createRenderbuffer", &T::CreateRenderbuffer),             \
      InstanceMethod("deleteRenderbuffer", &T::DeleteRenderbuffer),             \
      InstanceMethod("bindRenderbuffer", &T::BindRenderbuffer),                 \
      InstanceMethod("renderbufferStorage", &T::RenderbufferStorage),           \
      InstanceMethod("createTexture", &T::CreateTexture),                       \
      InstanceMethod("deleteTexture", &T::DeleteTexture),                       \
      InstanceMethod("bindTexture", &T::BindTexture),                           \
      InstanceMethod("texImage2D", &T::TexImage2D),                             \
      InstanceMethod("texSubImage2D", &T::TexSubImage2D),                       \
      InstanceMethod("copyTexImage2D", &T::CopyTexImage2D),                     \
      InstanceMethod("copyTexSubImage2D", &T::CopyTexSubImage2D),               \
      InstanceMethod("texParameteri", &T::TexParameteri),                       \
      InstanceMethod("activeTexture", &T::ActiveTexture),                       \
      InstanceMethod("generateMipmap", &T::GenerateMipmap),                     \
      InstanceMethod("enableVertexAttribArray", &T::EnableVertexAttribArray),   \
      InstanceMethod("disableVertexAttribArray", &T::DisableVertexAttribArray), \
      InstanceMethod("vertexAttribPointer", &T::VertexAttribPointer),           \
      InstanceMethod("getAttribLocation", &T::GetAttribLocation),               \
      InstanceMethod("getUniformLocation", &T::GetUniformLocation),             \
      InstanceMethod("uniform1f", &T::Uniform1f),                               \
      InstanceMethod("uniform1fv", &T::Uniform1fv),                             \
      InstanceMethod("uniform1i", &T::Uniform1i),                               \
      InstanceMethod("uniform1iv", &T::Uniform1iv),                             \
      InstanceMethod("uniform2f", &T::Uniform2f),                               \
      InstanceMethod("uniform2fv", &T::Uniform2fv),                             \
      InstanceMethod("uniform2i", &T::Uniform2i),                               \
      InstanceMethod("uniform2iv", &T::Uniform2iv),                             \
      InstanceMethod("uniform3f", &T::Uniform3f),                               \
      InstanceMethod("uniform3fv", &T::Uniform3fv),                             \
      InstanceMethod("uniform3i", &T::Uniform3i),                               \
      InstanceMethod("uniform3iv", &T::Uniform3iv),                             \
      InstanceMethod("uniform4f", &T::Uniform4f),                               \
      InstanceMethod("uniform4fv", &T::Uniform4fv),                             \
      InstanceMethod("uniform4i", &T::Uniform4i),                               \
      InstanceMethod("uniform4iv", &T::Uniform4iv),                             \
      InstanceMethod("uniformMatrix2fv", &T::UniformMatrix2fv),                 \
      InstanceMethod("uniformMatrix3fv", &T::UniformMatrix3fv),                 \
      InstanceMethod("uniformMatrix4fv", &T::UniformMatrix4fv),                 \
      InstanceMethod("drawArrays", &T::DrawArrays),                             \
      InstanceMethod("drawElements", &T::DrawElements),                         \
      InstanceMethod("pixelStorei", &T::PixelStorei),                           \
      InstanceMethod("polygonOffset", &T::PolygonOffset),                       \
      InstanceMethod("viewport", &T::Viewport),                                 \
      InstanceMethod("scissor", &T::Scissor),                                   \
      InstanceMethod("clearColor", &T::ClearColor),                             \
      InstanceMethod("clearDepth", &T::ClearDepth),                             \
      InstanceMethod("clearStencil", &T::ClearStencil),                         \
      InstanceMethod("clear", &T::Clear),                                       \
      InstanceMethod("depthMask", &T::DepthMask),                               \
      InstanceMethod("depthFunc", &T::DepthFunc),                               \
      InstanceMethod("depthRange", &T::DepthRange),                             \
      InstanceMethod("stencilFunc", &T::StencilFunc),                           \
      InstanceMethod("stencilFuncSeparate", &T::StencilFuncSeparate),           \
      InstanceMethod("stencilMask", &T::StencilMask),                           \
      InstanceMethod("stencilMaskSeparate", &T::StencilMaskSeparate),           \
      InstanceMethod("stencilOp", &T::StencilOp),                               \
      InstanceMethod("stencilOpSeparate", &T::StencilOpSeparate),               \
      InstanceMethod("blendColor", &T::BlendColor),                             \
      InstanceMethod("blendEquation", &T::BlendEquation),                       \
      InstanceMethod("blendEquationSeparate", &T::BlendEquationSeparate),       \
      InstanceMethod("blendFunc", &T::BlendFunc),                               \
      InstanceMethod("blendFuncSeparate", &T::BlendFuncSeparate),               \
      InstanceMethod("colorMask", &T::ColorMask),                               \
      InstanceMethod("cullFace", &T::CullFace),                                 \
      InstanceMethod("frontFace", &T::FrontFace),                               \
      InstanceMethod("enable", &T::Enable),                                     \
      InstanceMethod("disable", &T::Disable),                                   \
      InstanceMethod("getParameter", &T::GetParameter),                         \
      InstanceMethod("getShaderPrecisionFormat", &T::GetShaderPrecisionFormat), \
      InstanceMethod("getError", &T::GetError),                                 \
      InstanceMethod("getSupportedExtensions", &T::GetSupportedExtensions)

#define WEBGL2_METHODS(T)                                                                   \
  InstanceMethod("readBuffer", &T::ReadBuffer),                                             \
      InstanceMethod("bindBufferBase", &T::BindBufferBase),                                 \
      InstanceMethod("bindBufferRange", &T::BindBufferRange),                               \
      InstanceMethod("blitFramebuffer", &T::BlitFramebuffer),                               \
      InstanceMethod("renderbufferStorageMultisample", &T::RenderbufferStorageMultisample), \
      InstanceMethod("createVertexArray", &T::CreateVertexArray),                           \
      InstanceMethod("deleteVertexArray", &T::DeleteVertexArray),                           \
      InstanceMethod("bindVertexArray", &T::BindVertexArray),                               \
      InstanceMethod("texImage3D", &T::TexImage3D),                                         \
      InstanceMethod("texSubImage3D", &T::TexSubImage3D),                                   \
      InstanceMethod("getUniformBlockIndex", &T::GetUniformBlockIndex),                     \
      InstanceMethod("uniformBlockBinding", &T::UniformBlockBinding),                       \
      InstanceMethod("vertexAttribIPointer", &T::VertexAttribIPointer),                     \
      InstanceMethod("vertexAttribDivisor", &T::VertexAttribDivisor),                       \
      InstanceMethod("drawBuffers", &T::DrawBuffers),                                       \
      InstanceMethod("drawArraysInstanced", &T::DrawArraysInstanced),                       \
      InstanceMethod("drawElementsInstanced", &T::DrawElementsInstanced),                   \
      InstanceMethod("drawRangeElements", &T::DrawRangeElements)

  template <typename T>
  WebGLBaseRenderingContext<T>::WebGLBaseRenderingContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    m_clientContext = TrClientContextPerProcess::Get();
    assert(m_clientContext != nullptr);

    if (info.Length() >= 1 && info[0].IsObject())
    {
      auto jsContextAttribs = info[0].ToObject();
      if (jsContextAttribs.Has("alpha"))
        contextAttributes.alpha = jsContextAttribs.Get("alpha").ToBoolean();
      if (jsContextAttribs.Has("antialias"))
        contextAttributes.antialias = jsContextAttribs.Get("antialias").ToBoolean();
      if (jsContextAttribs.Has("depth"))
        contextAttributes.depth = jsContextAttribs.Get("depth").ToBoolean();
      if (jsContextAttribs.Has("stencil"))
        contextAttributes.stencil = jsContextAttribs.Get("stencil").ToBoolean();
      if (jsContextAttribs.Has("failIfMajorPerformanceCaveat"))
        contextAttributes.failIfMajorPerformanceCaveat = jsContextAttribs.Get("failIfMajorPerformanceCaveat").ToBoolean();
      if (jsContextAttribs.Has("premultipliedAlpha"))
        contextAttributes.premultipliedAlpha = jsContextAttribs.Get("premultipliedAlpha").ToBoolean();
      if (jsContextAttribs.Has("preserveDrawingBuffer"))
        contextAttributes.preserveDrawingBuffer = jsContextAttribs.Get("preserveDrawingBuffer").ToBoolean();
      if (jsContextAttribs.Has("xrCompatible"))
        contextAttributes.xrCompatible = jsContextAttribs.Get("xrCompatible").ToBoolean();
      if (jsContextAttribs.Has("powerPreference"))
      {
        auto powerPreference = jsContextAttribs.Get("powerPreference").ToString().Utf8Value();
        if (powerPreference == "high-performance")
          contextAttributes.powerPreference = "high-performance";
        else if (powerPreference == "low-power")
          contextAttributes.powerPreference = "low-power";
      }
    }

    auto initCommandBuffer = WebGL1ContextInitCommandBufferRequest();
    sendCommandBufferRequest(initCommandBuffer, true);

    auto resp = recvCommandBufferResponse<WebGL1ContextInitCommandBufferResponse>(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES);
    if (resp == nullptr)
    {
      Napi::TypeError::New(env, "Failed to initialize WebGL context")
          .ThrowAsJavaScriptException();
      return;
    }

    this->viewport = resp->drawingViewport;
    this->maxCombinedTextureImageUnits = resp->maxCombinedTextureImageUnits;
    this->maxCubeMapTextureSize = resp->maxCubeMapTextureSize;
    this->maxFragmentUniformVectors = resp->maxFragmentUniformVectors;
    this->maxRenderbufferSize = resp->maxRenderbufferSize;
    this->maxTextureImageUnits = resp->maxTextureImageUnits;
    this->maxTextureSize = resp->maxTextureSize;
    this->maxVaryingVectors = resp->maxVaryingVectors;
    this->maxVertexAttribs = resp->maxVertexAttribs;
    this->maxVertexTextureImageUnits = resp->maxVertexTextureImageUnits;
    this->maxVertexUniformVectors = resp->maxVertexUniformVectors;
    this->vendor = resp->vendor;
    this->version = resp->version;
    this->renderer = resp->renderer;
    delete resp;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::MakeXRCompatible(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    contextAttributes.xrCompatible = true;
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Resolve(env.Undefined());
    return deferred.Promise();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetContextAttributes(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Object attribs = Napi::Object::New(env);
    attribs.Set("alpha", Napi::Boolean::New(env, contextAttributes.alpha));
    attribs.Set("antialias", Napi::Boolean::New(env, contextAttributes.antialias));
    attribs.Set("depth", Napi::Boolean::New(env, contextAttributes.depth));
    attribs.Set("stencil", Napi::Boolean::New(env, contextAttributes.stencil));
    attribs.Set("failIfMajorPerformanceCaveat", Napi::Boolean::New(env, contextAttributes.failIfMajorPerformanceCaveat));
    attribs.Set("premultipliedAlpha", Napi::Boolean::New(env, contextAttributes.premultipliedAlpha));
    attribs.Set("preserveDrawingBuffer", Napi::Boolean::New(env, contextAttributes.preserveDrawingBuffer));
    attribs.Set("xrCompatible", Napi::Boolean::New(env, contextAttributes.xrCompatible));
    attribs.Set("powerPreference", Napi::String::New(env, contextAttributes.powerPreference));
    return attribs;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto id = programIdGen.get();
    auto commandBuffer = CreateProgramCommandBufferRequest(id);
    sendCommandBufferRequest(commandBuffer);
    return WebGLProgram::constructor->New({Napi::Number::New(env, id)});
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DeleteProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "deleteProgram() 1st argument must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto req = DeleteProgramCommandBufferRequest(program->GetId());
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::LinkProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "linkProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    // check if the argument is a WebGLProgram type
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "linkProgram() takes a WebGLProgram as argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto req = LinkProgramCommandBufferRequest(program->GetId());
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<LinkProgramCommandBufferResponse>(COMMAND_BUFFER_LINK_PROGRAM_RES);
    if (resp == nullptr)
    {
      string msg = "Failed to link program(" + to_string(program->GetId()) + "): timeout.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!resp->success)
    {
      delete resp;
      string msg = "Failed to link program(" + to_string(program->GetId()) + "): not successful.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    /**
     * Mark the program as linked.
     */
    program->SetLinkStatus(true);

    /**
     * Update the program's attribute locations.
     */
    for (auto &attribLocation : resp->attribLocations)
      program->SetAttribLocation(attribLocation.name, attribLocation.location);

    /**
     * See https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getUniformLocation#name
     *
     * When uniforms declared as an array, the valid name might be like the followings:
     *
     * - foo
     * - foo[0]
     * - foo[1]
     */
    for (auto &uniformLocation : resp->uniformLocations)
    {
      auto name = uniformLocation.name;
      auto location = uniformLocation.location;
      auto size = uniformLocation.size;

      /**
       * FIXME: The OpenGL returns "foo[0]" from `glGetActiveUniform()`, thus we need to handle it here:
       *
       * 1. check if the name ends with "[0]"
       * 2. grab the name without "[0]"
       * 3. set the uniform location for the name without "[0]"
       * 4. set the uniform location for the name with "[0]" and the index
       * 5. repeat 4 for the rest of the indices
       *
       * After the above steps, we will have the names looks like: foo, foo[0], foo[1], foo[2], ...
       */
      std::string arraySuffix = "[0]";
      int endedAt = name.length() - arraySuffix.length();
      bool endsWithArray = name.size() > arraySuffix.size() && name.rfind(arraySuffix) != std::string::npos;

      /**
       * Check if size is 1 and not ends with [0], WebGL developers might use 1-size array such as: `[0]`.
       */
      if (size == 1 && !endsWithArray)
      {
        program->SetUniformLocation(name, location);
      }
      else if (endsWithArray)
      {
        auto arrayName = name.substr(0, endedAt);
        program->SetUniformLocation(arrayName, location);
        program->SetUniformLocation(name, location);
        for (int i = 1; i < size; i++)
          program->SetUniformLocation(arrayName + "[" + std::to_string(i) + "]", location + i);
      }
      else
      {
        // TODO: warning size is invalid?
        continue;
      }
    }

    if (m_isWebGL2 == true)
    {
      /**
       * Save the uniform block indices to the program object
       */
      for (auto &uniformBlock : resp->uniformBlocks)
        program->SetUniformBlockIndex(uniformBlock.name, uniformBlock.index);
    }
    delete resp;
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::UseProgram(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "useProgram() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "useProgram() 1st argument must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto req = UseProgramCommandBufferRequest(program->GetId());
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetProgramParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getProgramParameter() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "getProgramParameter() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    int pname = info[1].As<Napi::Number>().Int32Value();

    /**
     * The following parameters are carried when linkProgram() is responded, thus we could return them from the client-side
     * `WebGLProgram` object directly.
     */
    if (pname == WEBGL_LINK_STATUS)
      return Napi::Boolean::New(env, program->GetLinkStatus());

    /**
     * Send a command buffer request and wait for the response if not hit the above conditions.
     */
    auto req = GetProgramParamCommandBufferRequest(program->GetId(), pname);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetProgramParamCommandBufferResponse>(COMMAND_BUFFER_GET_PROGRAM_PARAM_RES);
    if (resp != nullptr)
    {
      auto r = Napi::Number::New(env, resp->value);
      delete resp;
      return r;
    }
    else
    {
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetProgramInfoLog(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getProgramInfoLog() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "getProgramInfoLog() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto req = GetProgramInfoLogCommandBufferRequest(program->GetId());
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetProgramInfoLogCommandBufferResponse>(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_RES);
    if (resp != nullptr)
    {
      auto r = Napi::String::New(env, resp->infoLog);
      delete resp;
      return r;
    }
    else
    {
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::AttachShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "attachShader() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "attachShader() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "attachShader() 2nd argument(shader) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto shader = info[1].As<Napi::Number>().Int32Value();
    auto req = AttachShaderCommandBufferRequest(program->GetId(), shader);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DetachShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "detachShader() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "detachShader() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    auto shader = info[1].As<Napi::Number>().Int32Value();
    auto req = DetachShaderCommandBufferRequest(program->GetId(), shader);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "createShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int type = info[0].As<Napi::Number>().Int32Value();

    auto id = shaderIdGen.get();
    auto req = CreateShaderCommandBufferRequest(id, type);
    sendCommandBufferRequest(req);
    return Napi::Number::New(env, id);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DeleteShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    auto commandBuffer = DeleteShaderCommandBufferRequest(shader);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::ShaderSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "shaderSource() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    std::string source = info[1].As<Napi::String>().Utf8Value();

    // split by line
    // std::vector<std::string> lines;
    // std::istringstream f(source);
    // std::string line;
    // while (std::getline(f, line))
    // {
    //   DEBUG("Unity", "[src]: %s", line.c_str());
    // }

    auto commandBuffer = ShaderSourceCommandBufferRequest(shader, source);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CompileShader(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "compileShader() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    auto req = CompileShaderCommandBufferRequest(shader);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetShaderSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getShaderSource() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    auto req = GetShaderSourceCommandBufferRequest(shader);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderSourceCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_SOURCE_RES);
    if (resp != nullptr)
    {
      auto shaderSource = Napi::String::New(env, resp->source);
      delete resp;
      return shaderSource;
    }
    else
    {
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetShaderParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getShaderParameter() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    int pname = info[1].As<Napi::Number>().Int32Value();
    auto req = GetShaderParamCommandBufferRequest(shader, pname);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderParamCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_PARAM_RES);
    if (resp != nullptr)
    {
      auto r = Napi::Number::New(env, resp->value);
      delete resp;
      return r;
    }
    else
    {
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetShaderInfoLog(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getShaderInfoLog() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shader = info[0].As<Napi::Number>().Int32Value();
    auto req = GetShaderInfoLogCommandBufferRequest(shader);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderInfoLogCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_INFO_LOG_RES);
    if (resp != nullptr)
    {
      auto infoLogString = Napi::String::New(env, resp->infoLog);
      delete resp;
      return infoLogString;
    }
    else
    {
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t id = bufferIdGen.get();
    auto req = CreateBufferCommandBufferRequest(id);
    sendCommandBufferRequest(req);
    return Napi::Number::New(env, id);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DeleteBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteBuffer() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int buffer = info[0].As<Napi::Number>().Int32Value();
    auto req = DeleteBufferCommandBufferRequest(buffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BindBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindBuffer() takes 2 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the first argument(target) should be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int buffer = 0;
    if (info[1].IsNumber())
      buffer = info[1].As<Napi::Number>().Int32Value();

    auto req = BindBufferCommandBufferRequest(target, buffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BufferData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bufferData() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    /**
     * TODO: support bufferData() with size
     */
    if (info[1].IsNumber())
    {
      Napi::TypeError::New(env, "bufferData(target, size, usage) is not supported yet.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int usage = info[2].As<Napi::Number>().Int32Value();

    // auto jsBuffer = info[1];
    void *bufferData = nullptr;
    size_t bufferSize = 0;
    // if (jsBuffer.IsDataView() || jsBuffer.IsTypedArray())
    // {
    //   Napi::ArrayBuffer byteBuffer;
    //   size_t byteLength, byteOffset;
    //   if (jsBuffer.IsDataView())
    //   {
    //     auto dataView = jsBuffer.As<Napi::DataView>();
    //     byteBuffer = dataView.ArrayBuffer();
    //     byteLength = dataView.ByteLength();
    //     byteOffset = dataView.ByteOffset();
    //   }
    //   else
    //   {
    //     auto typedArray = jsBuffer.As<Napi::TypedArray>();
    //     byteBuffer = typedArray.ArrayBuffer();
    //     byteLength = typedArray.ByteLength();
    //     byteOffset = typedArray.ByteOffset();
    //   }
    //   auto buffer = Napi::Uint8Array::New(env, byteLength, byteBuffer, byteOffset);
    //   bufferData = buffer.Data();
    //   bufferSize = buffer.ByteLength();
    // }
    // else if (jsBuffer.IsArrayBuffer())
    // {
    //   auto buffer = jsBuffer.As<Napi::ArrayBuffer>();
    //   bufferData = buffer.Data();
    //   bufferSize = buffer.ByteLength();
    // }
    // else
    // {
    //   Napi::TypeError::New(env, "the 2nd argument(data) should be an ArrayBuffer, TypedArray or DataView in bufferData().")
    //       .ThrowAsJavaScriptException();
    //   return env.Undefined();
    // }

    Napi::Uint8Array buffer = info[1].As<Napi::Uint8Array>();
    bufferData = buffer.Data();
    bufferSize = buffer.ByteLength();

    auto req = BufferDataCommandBufferRequest(target, bufferSize, bufferData, usage);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BufferSubData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bufferSubData() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsArrayBuffer() && !info[2].IsTypedArray())
    {
      Napi::TypeError::New(env, "the 3rd argument should be an ArrayBuffer or TypedArray when calling bufferSubData().")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int offset = info[1].As<Napi::Number>().Int32Value();

    auto jsBuffer = info[2];
    void *bufferData = nullptr;
    size_t bufferSize = 0;
    if (jsBuffer.IsDataView() || jsBuffer.IsTypedArray())
    {
      Napi::ArrayBuffer byteBuffer;
      size_t byteLength, byteOffset;
      if (jsBuffer.IsDataView())
      {
        auto dataView = jsBuffer.As<Napi::DataView>();
        byteBuffer = dataView.ArrayBuffer();
        byteLength = dataView.ByteLength();
        byteOffset = dataView.ByteOffset();
      }
      else
      {
        auto typedArray = jsBuffer.As<Napi::TypedArray>();
        byteBuffer = typedArray.ArrayBuffer();
        byteLength = typedArray.ByteLength();
        byteOffset = typedArray.ByteOffset();
      }
      auto buffer = Napi::Uint8Array::New(env, byteLength, byteBuffer, byteOffset);
      bufferData = buffer.Data();
      bufferSize = buffer.ByteLength();
    }
    else if (jsBuffer.IsArrayBuffer())
    {
      auto buffer = jsBuffer.As<Napi::ArrayBuffer>();
      bufferData = buffer.Data();
      bufferSize = buffer.ByteLength();
    }
    else
    {
      Napi::TypeError::New(env, "the 2nd argument(data) should be an ArrayBuffer, TypedArray or DataView in bufferData().")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto req = BufferSubDataCommandBufferRequest(target, offset, bufferSize, bufferData);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto id = framebufferIdGen.get();
    auto req = CreateFramebufferCommandBufferRequest(id);
    sendCommandBufferRequest(req);
    return Napi::Number::New(env, id);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DeleteFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteFramebuffer() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int framebuffer = info[0].As<Napi::Number>().Int32Value();
    auto req = DeleteFramebufferCommandBufferRequest(framebuffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BindFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindFramebuffer() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the first argument should be a number when calling bindFramebuffer().")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int framebuffer = 0;
    if (info[1].IsNumber())
      framebuffer = info[1].As<Napi::Number>().Int32Value();

    auto req = BindFramebufferCommandBufferRequest(target, framebuffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::FramebufferRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "framebufferRenderbuffer() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    int attachment = info[1].As<Napi::Number>().Int32Value();
    int renderbuffertarget = info[2].As<Napi::Number>().Int32Value();
    int renderbuffer = info[3].As<Napi::Number>().Int32Value();

    auto req = FramebufferRenderbufferCommandBufferRequest(target, attachment, renderbuffertarget, renderbuffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::FramebufferTexture2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "framebufferTexture2D() takes 5 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto jsTexture = info[3];
    if (!jsTexture.IsObject() || !jsTexture.As<Napi::Object>().InstanceOf(WebGLTexture::constructor->Value()))
    {
      Napi::TypeError::New(env, "framebufferTexture2D() 4th argument(texture) must be a WebGLTexture.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int attachment = info[1].As<Napi::Number>().Int32Value();
    int textarget = info[2].As<Napi::Number>().Int32Value();
    auto texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(jsTexture.As<Napi::Object>());
    int level = info[4].As<Napi::Number>().Int32Value();

    auto req = FramebufferTexture2DCommandBufferRequest(target, attachment, textarget, texture->GetId(), level);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CheckFramebufferStatus(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "checkFramebufferStatus() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    auto req = CheckFramebufferStatusCommandBufferRequest(target);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<CheckFramebufferStatusCommandBufferResponse>(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES);
    if (resp != nullptr)
    {
      auto r = Napi::Number::New(env, resp->status);
      delete resp;
      return r;
    }
    else
    {
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto id = renderbufferIdGen.get();
    auto req = CreateRenderbufferCommandBufferRequest(id);
    sendCommandBufferRequest(req);
    return Napi::Number::New(env, id);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DeleteRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteRenderbuffer() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int renderbuffer = info[0].As<Napi::Number>().Int32Value();
    auto req = DeleteRenderbufferCommandBufferRequest(renderbuffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BindRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindRenderbuffer() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the first argument should be a number when calling bindRenderbuffer().")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int renderbuffer = 0;
    if (info[1].IsNumber())
      renderbuffer = info[1].As<Napi::Number>().Int32Value();

    auto req = BindRenderbufferCommandBufferRequest(target, renderbuffer);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::RenderbufferStorage(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "renderbufferStorage() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int target = info[0].As<Napi::Number>().Int32Value();
    int internalformat = info[1].As<Napi::Number>().Int32Value();
    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();

    auto req = RenderbufferStorageCommandBufferRequest(target, internalformat, width, height);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto id = textureIdGen.get();
    auto req = CreateTextureCommandBufferRequest(id);
    sendCommandBufferRequest(req);
    return WebGLTexture::constructor->New({Napi::Number::New(env, id)});
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DeleteTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteTexture() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLTexture::constructor->Value()))
    {
      Napi::TypeError::New(env, "deleteTexture() takes a WebGLTexture as argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[0].As<Napi::Object>());
    auto req = DeleteTextureCommandBufferRequest(texture->GetId());
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BindTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "bindTexture() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "the target to bindTexture() is invalid.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    uint32_t texture = 0;
    if (info[1].IsNull())
    {
      texture = 0;
    }
    else if (info[1].IsObject() && info[1].As<Napi::Object>().InstanceOf(WebGLTexture::constructor->Value()))
    {
      auto textureObj = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[1].As<Napi::Object>());
      texture = textureObj->GetId();
    }
    else
    {
      Napi::TypeError::New(env, "the texture to bindTexture() is invalid, must be null or a WebGLTexture object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto req = BindTextureCommandBufferRequest(target, texture);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::TexImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "texImage2D() takes at least 6 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto target = info[0].As<Napi::Number>().Uint32Value();
    auto level = info[1].As<Napi::Number>().Uint32Value();
    auto internalformat = info[2].As<Napi::Number>().Uint32Value();

    if (target != WEBGL_TEXTURE_2D)
    {
      Napi::TypeError::New(env, "texImage2D() only supports target TEXTURE_2D.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    TextureImage2DCommandBufferRequest req(target, level, internalformat);
    unsigned char *pixelsData = nullptr;
    if (info.Length() == 6)
    {
      req.format = info[3].As<Napi::Number>().Uint32Value();
      req.pixelType = info[4].As<Napi::Number>().Uint32Value();
      if (!info[5].IsObject())
      {
        Napi::TypeError::New(env, "the image source should be an object.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      auto imageSourceObject = info[5].ToObject();
      if (imageSourceObject.InstanceOf(ImageBitmap::constructor->Value()))
      {
        auto imageBitmap = ImageBitmap::Unwrap(imageSourceObject);
        SkBitmap *bitmap = imageBitmap->getSkBitmap();
        req.width = bitmap->width();
        req.height = bitmap->height();
        req.format = WEBGL_RGBA;
        req.internalformat = WEBGL2_RGBA8;
        pixelsData = reinterpret_cast<unsigned char *>(bitmap->getPixels());
      }
      else if (imageSourceObject.InstanceOf(OffscreenCanvas::constructor->Value()))
      {
        auto canvas = OffscreenCanvas::Unwrap(imageSourceObject);
        SkBitmap *bitmap = canvas->getSkBitmap();
        req.width = bitmap->width();
        req.height = bitmap->height();
        req.format = WEBGL_RGBA;
        req.internalformat = WEBGL2_RGBA8;
        pixelsData = reinterpret_cast<unsigned char *>(bitmap->getPixels());
      }
      else
      {
        env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>()({imageSourceObject});
        Napi::TypeError::New(env, "Unsupported `imageSource` type")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else if (info.Length() == 9)
    {
      if (!info[3].IsNumber() || !info[4].IsNumber())
      {
        Napi::Object debugObject = Napi::Object::New(env);
        debugObject.Set("width", info[3]);
        debugObject.Set("height", info[4]);
        env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>()({debugObject});
        Napi::TypeError::New(env, "width or height is not a number.")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }

      req.width = info[3].As<Napi::Number>().Uint32Value();
      req.height = info[4].As<Napi::Number>().Uint32Value();
      req.border = info[5].As<Napi::Number>().Uint32Value();
      req.format = info[6].As<Napi::Number>().Uint32Value();
      req.pixelType = info[7].As<Napi::Number>().Uint32Value();

      Napi::Value pixelsValue = info[8];
      if (pixelsValue.IsNull() || pixelsValue.IsUndefined())
      {
        pixelsData = nullptr;
      }
      else if (pixelsValue.IsTypedArray() || pixelsValue.IsArrayBuffer())
      {
        if (pixelsValue.IsArrayBuffer())
        {
          auto arraybuffer = pixelsValue.As<Napi::ArrayBuffer>();
          pixelsData = static_cast<unsigned char *>(arraybuffer.Data());
        }
        else if (pixelsValue.IsTypedArray())
        {
          auto typedarray = pixelsValue.As<Napi::TypedArray>();
          pixelsData = static_cast<unsigned char *>(typedarray.ArrayBuffer().Data()) + typedarray.ByteOffset();
        }
      }
      else
      {
        Napi::TypeError::New(env, "The pixels should be a TypedArray or ArrayBuffer.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    if (m_unpackFlipY || m_unpackPremultiplyAlpha)
    {
      unsigned char *unpacked = unpackPixels(req.pixelType,
                                             req.format,
                                             req.width,
                                             req.height,
                                             pixelsData);
      if (unpacked == nullptr)
      {
        Napi::TypeError::New(env, "Failed to unpack pixels, the source data is null.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      req.setPixels(unpacked);
      delete[] unpacked;
    }
    else
    {
      req.setPixels(pixelsData);
    }
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::TexSubImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int xoffset = info[2].As<Napi::Number>().Int32Value();
    int yoffset = info[3].As<Napi::Number>().Int32Value();
    int width = info[4].As<Napi::Number>().Int32Value();
    int height = info[5].As<Napi::Number>().Int32Value();
    int format = info[6].As<Napi::Number>().Int32Value();
    int type = info[7].As<Napi::Number>().Int32Value();

    Napi::Value imageSource = info[8];
    if (imageSource.IsNull())
    {
      // TODO: When the image source is null, just create TexSubImage2DCommandBuffer with empty mode
    }

    // Otherwise, the image source should be a TypedArray
    if (!imageSource.IsTypedArray())
    {
      Napi::TypeError::New(env, "the pixels should be a TypedArray.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::TypedArray pixels = info[8].As<Napi::TypedArray>();
    Napi::ArrayBuffer data = pixels.ArrayBuffer();

    if (m_unpackFlipY == true || m_unpackPremultiplyAlpha == true)
    {
      unsigned char *packedPixels = reinterpret_cast<unsigned char *>(data.Data());
      unsigned char *pixels = unpackPixels(type, format, width, height, packedPixels);
      if (pixels == nullptr)
      {
        Napi::TypeError::New(env, "Failed to unpack pixels, the source data is null.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      auto req = TextureSubImage2DCommandBufferRequest(target, level, xoffset, yoffset, width, height, format, type,
                                                       pixels);
      sendCommandBufferRequest(req);
      delete[] pixels;
    }
    else
    {
      auto req = TextureSubImage2DCommandBufferRequest(target, level, xoffset, yoffset, width, height, format, type,
                                                       reinterpret_cast<unsigned char *>(data.Data()));
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CopyTexImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int internalformat = info[2].As<Napi::Number>().Int32Value();
    int x = info[3].As<Napi::Number>().Int32Value();
    int y = info[4].As<Napi::Number>().Int32Value();
    int width = info[5].As<Napi::Number>().Int32Value();
    int height = info[6].As<Napi::Number>().Int32Value();
    int border = info[7].As<Napi::Number>().Int32Value();

    auto req = CopyTextureImage2DCommandBufferRequest(target, level, internalformat, x, y, width, height, border);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CopyTexSubImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int xoffset = info[2].As<Napi::Number>().Int32Value();
    int yoffset = info[3].As<Napi::Number>().Int32Value();
    int x = info[4].As<Napi::Number>().Int32Value();
    int y = info[5].As<Napi::Number>().Int32Value();
    int width = info[6].As<Napi::Number>().Int32Value();
    int height = info[7].As<Napi::Number>().Int32Value();

    auto req = CopyTextureSubImage2DCommandBufferRequest(target, level, xoffset, yoffset, x, y, width, height);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::TexParameteri(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int pname = info[1].As<Napi::Number>().Int32Value();
    int param = info[2].As<Napi::Number>().Int32Value();

    auto req = TextureParameteriCommandBufferRequest(target, pname, param);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::ActiveTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "activeTexture() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "activeTexture() 1st argument(texture) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int textureUnit = info[0].As<Napi::Number>().Int32Value();
    auto req = ActiveTextureCommandBufferRequest(textureUnit);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GenerateMipmap(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    auto req = GenerateMipmapCommandBufferRequest(target);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::EnableVertexAttribArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "enableVertexAttribArray() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int index = info[0].As<Napi::Number>().Int32Value();
    auto req = EnableVertexAttribArrayCommandBufferRequest(index);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DisableVertexAttribArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "disableVertexAttribArray() takes 1 argument.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint32_t index = info[0].As<Napi::Number>().Uint32Value();
    auto req = DisableVertexAttribArrayCommandBufferRequest(index);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::VertexAttribPointer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "vertexAttribPointer() takes 6 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint32_t index = info[0].As<Napi::Number>().Uint32Value();
    uint32_t size = info[1].As<Napi::Number>().Uint32Value();
    uint32_t type = info[2].As<Napi::Number>().Uint32Value();
    bool normalized = info[3].As<Napi::Boolean>().Value();
    uint32_t stride = info[4].As<Napi::Number>().Uint32Value();
    uint32_t offset = info[5].As<Napi::Number>().Uint32Value();

    auto req = VertexAttribPointerCommandBufferRequest(index, size, type, normalized, stride, offset);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetAttribLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getAttribLocation() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "getAttribLocation() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();

    int loc = -1;
    if (program->HasAttribLocation(name))
      loc = program->GetAttribLocation(name);
    return Napi::Number::New(env, loc);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetUniformLocation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getUniformLocation() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "getUniformLocation() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();

    if (!program->HasUniformLocation(name))
      return env.Null();

    auto jsUniformLocation = WebGLUniformLocation::constructor->New({Napi::Number::New(env, program->GetUniformLocation(name))});
    auto uniformLocation = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(jsUniformLocation);
    uniformLocation->SetName(name);
    return jsUniformLocation;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform1f(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform1f() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform1f() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    auto x = info[1].As<Napi::Number>().FloatValue();
    auto req = Uniform1fCommandBufferRequest(location->GetValue(), x);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform1fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform1fv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform1fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Float32Array array = info[1].As<Napi::Float32Array>();
    size_t length = array.ElementLength();

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto req = Uniform1fvCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform1i(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform1i() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform1i() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    int x = info[1].As<Napi::Number>().Int32Value();

    auto req = Uniform1iCommandBufferRequest(location->GetValue(), x);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform1iv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform1iv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform1iv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Int32Array array = info[1].As<Napi::Int32Array>();
    size_t length = array.ElementLength();

    std::vector<int> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().Int32Value();

    auto req = Uniform1ivCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform2f(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniform2f() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform2f() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    float x = info[1].As<Napi::Number>().FloatValue();
    float y = info[2].As<Napi::Number>().FloatValue();

    auto req = Uniform2fCommandBufferRequest(location->GetValue(), x, y);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform2fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform2fv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform2fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Float32Array array = info[1].As<Napi::Float32Array>();
    size_t length = array.ElementLength();
    if (length != 2)
    {
      Napi::TypeError::New(env, "uniform2fv() takes 2 float elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto req = Uniform2fvCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform2i(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniform2i() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform2i() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    int x = info[1].As<Napi::Number>().Int32Value();
    int y = info[2].As<Napi::Number>().Int32Value();

    auto req = Uniform2iCommandBufferRequest(location->GetValue(), x, y);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform2iv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform2iv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform2iv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Int32Array array = info[1].As<Napi::Int32Array>();
    size_t length = array.ElementLength();
    if (length != 2)
    {
      Napi::TypeError::New(env, "uniform2iv() takes 2 int elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<int> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().Int32Value();

    auto req = Uniform2ivCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform3f(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "uniform3f() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform3f() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    float x = info[1].As<Napi::Number>().FloatValue();
    float y = info[2].As<Napi::Number>().FloatValue();
    float z = info[3].As<Napi::Number>().FloatValue();

    auto req = Uniform3fCommandBufferRequest(location->GetValue(), x, y, z);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform3fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform3fv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform3fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Float32Array array = info[1].As<Napi::Float32Array>();
    size_t length = array.ElementLength();
    if (length != 3)
    {
      Napi::TypeError::New(env, "uniform3fv() takes 3 float elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto req = Uniform3fvCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform3i(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "uniform3i() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform3i() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    int x = info[1].As<Napi::Number>().Int32Value();
    int y = info[2].As<Napi::Number>().Int32Value();
    int z = info[3].As<Napi::Number>().Int32Value();

    auto req = Uniform3iCommandBufferRequest(location->GetValue(), x, y, z);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform3iv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform3iv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform3iv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Int32Array array = info[1].As<Napi::Int32Array>();
    size_t length = array.ElementLength();
    if (length != 3)
    {
      Napi::TypeError::New(env, "uniform3iv() takes 3 int elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<int> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().Int32Value();

    auto req = Uniform3ivCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform4f(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "uniform4f() takes 5 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform4f() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    float x = info[1].As<Napi::Number>().FloatValue();
    float y = info[2].As<Napi::Number>().FloatValue();
    float z = info[3].As<Napi::Number>().FloatValue();
    float w = info[4].As<Napi::Number>().FloatValue();

    auto req = Uniform4fCommandBufferRequest(location->GetValue(), x, y, z, w);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform4fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform4fv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform4fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Float32Array array = info[1].As<Napi::Float32Array>();
    size_t length = array.ElementLength();
    if (length != 4)
    {
      Napi::TypeError::New(env, "uniform4fv() takes 4 float elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto req = Uniform4fvCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform4i(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "uniform4i() takes 5 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform4i() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    int x = info[1].As<Napi::Number>().Int32Value();
    int y = info[2].As<Napi::Number>().Int32Value();
    int z = info[3].As<Napi::Number>().Int32Value();
    int w = info[4].As<Napi::Number>().Int32Value();

    auto req = Uniform4iCommandBufferRequest(location->GetValue(), x, y, z, w);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Uniform4iv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "uniform4iv() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniform4iv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    Napi::Int32Array array = info[1].As<Napi::Int32Array>();
    size_t length = array.ElementLength();
    if (length != 4)
    {
      Napi::TypeError::New(env, "uniform4iv() takes 4 int elements array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<int> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().Int32Value();

    auto req = Uniform4ivCommandBufferRequest(location->GetValue(), data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::UniformMatrix2fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniformMatrix2fv() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniformMatrix2fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    bool transpose = info[1].As<Napi::Boolean>().Value();
    Napi::Float32Array array = info[2].As<Napi::Float32Array>();
    size_t length = array.ElementLength();
    if (length % 4 != 0)
    {
      Napi::TypeError::New(env, "uniformMatrix2fv() takes 4x float elements array.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto req = UniformMatrix2fvCommandBufferRequest(location->GetValue(), transpose, data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::UniformMatrix3fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniformMatrix3fv() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniformMatrix3fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    bool transpose = info[1].As<Napi::Boolean>().Value();
    Napi::Float32Array array = info[2].As<Napi::Float32Array>();
    size_t length = array.ElementLength();
    if (length % 9 != 0)
    {
      Napi::TypeError::New(env, "uniformMatrix3fv() takes 9x float elements array.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<float> data(length);
    for (size_t i = 0; i < length; i++)
      data[i] = array.Get(i).ToNumber().FloatValue();

    auto req = UniformMatrix3fvCommandBufferRequest(location->GetValue(), transpose, data);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::UniformMatrix4fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (TR_UNLIKELY(info.Length() < 3))
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    else if (TR_UNLIKELY(info.Length() > 3))
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() don't support the parameters: `srcLength` and `srcOffset`")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (TR_UNLIKELY(!info[0].IsObject()))
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() 1st argument(program) must be an object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    bool transpose = info[1].As<Napi::Boolean>().Value();
    Napi::Float32Array matricesArray = info[2].As<Napi::Float32Array>();
    UniformMatrix4fvCommandBufferRequest req(location->GetValue(), transpose);

    if (matricesArray.Has(WEBGL_PLACEHOLDERS_PLACEHOLDER_ID_KEY))
    {
      auto placeholderIdValue = matricesArray.Get(WEBGL_PLACEHOLDERS_PLACEHOLDER_ID_KEY);
      if (!placeholderIdValue.IsNumber())
      {
        Napi::TypeError::New(env, "uniformMatrix4fv() placeholderId must be a number.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      auto placeholderId = (WebGLMatrixPlaceholderId)placeholderIdValue.ToNumber().Uint32Value();
      auto handedness = matricesArray.Get(WEBGL_PLACEHOLDERS_USE_RIGHTHANDED_KEY).ToBoolean()
                            ? MatrixHandedness::MATRIX_RIGHT_HANDED
                            : MatrixHandedness::MATRIX_LEFT_HANDED;
      MatrixComputationGraph computationGraph(placeholderId, handedness);

      if (matricesArray.Has(WEBGL_PLACEHOLDERS_INVERSE_MATRIX_KEY))
        computationGraph.inverseMatrix = matricesArray.Get(WEBGL_PLACEHOLDERS_INVERSE_MATRIX_KEY).ToBoolean().Value();
      req.computationGraph4values = computationGraph;
    }
    else
    {
      size_t length = matricesArray.ElementLength();
      if (TR_UNLIKELY(length % 16 != 0))
      {
        Napi::TypeError::New(env,
                             "uniformMatrix4fv() must take 16x float elements array but accept " + std::to_string(length) + ".")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      req.values.resize(length);
      auto valuesSrc = matricesArray.Data();
      std::copy(valuesSrc, valuesSrc + length, req.values.begin());
    }
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DrawArrays(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "drawArrays() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int mode = info[0].As<Napi::Number>().Int32Value();
    int first = info[1].As<Napi::Number>().Int32Value();
    int count = info[2].As<Napi::Number>().Int32Value();

    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawArrays()");
    auto req = DrawArraysCommandBufferRequest(mode, first, count);
    sendCommandBufferRequest(req);
    sendFirstContentfulPaintMetrics();
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DrawElements(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "drawElements() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int mode = info[0].As<Napi::Number>().Int32Value();
    int count = info[1].As<Napi::Number>().Int32Value();
    int type = info[2].As<Napi::Number>().Int32Value();
    int offset = info[3].As<Napi::Number>().Int32Value();

    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawElements()");
    auto req = DrawElementsCommandBufferRequest(mode, count, type, offset);
    sendCommandBufferRequest(req);
    sendFirstContentfulPaintMetrics();
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::PixelStorei(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "pixelStorei() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "pixelStorei() 1st argument(pname) must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "pixelStorei() 2nd argument(param) must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int pname = info[0].As<Napi::Number>().Int32Value();
    int param = info[1].ToNumber().Int32Value();

    if (pname == WEBGL_UNPACK_FLIP_Y_WEBGL)
    {
      m_unpackFlipY = param;
    }
    else if (pname == WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL)
    {
      m_unpackPremultiplyAlpha = param;
    }
    else if (pname == WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL)
    {
      // TODO: Implement this
    }
    else
    {
      auto req = PixelStoreiCommandBufferRequest(pname, param);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::PolygonOffset(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "polygonOffset() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    float factor = info[0].As<Napi::Number>().FloatValue();
    float units = info[1].As<Napi::Number>().FloatValue();

    auto req = PolygonOffsetCommandBufferRequest(factor, units);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Viewport(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "viewport() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();
    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();

    if (x < 0 || y < 0 || width < 0 || height < 0)
    {
      string msg = "viewport() arguments must be positive. x: " + std::to_string(x) + ", y: " + std::to_string(y) +
                   ", width: " + std::to_string(width) + ", height: " + std::to_string(height);
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!viewport.isEqual(width, height, x, y))
    {
      /** Send the command to server if only the viewport is changed */
      auto req = SetViewportCommandBufferRequest(x, y, width, height);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Scissor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "viewport() takes 4 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();
    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();

    auto req = SetScissorCommandBufferRequest(x, y, width, height);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::ClearColor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (info.Length() >= 4)
    // {
    //   float r = info[0].ToNumber().FloatValue();
    //   float g = info[1].ToNumber().FloatValue();
    //   float b = info[2].ToNumber().FloatValue();
    //   float a = info[3].ToNumber().FloatValue();
    //   addCommandBuffer(new renderer::ClearColorCommandBuffer(r, g, b, a));
    // }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::ClearDepth(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (info.Length() >= 1)
    // {
    //   float depth = info[0].ToNumber().FloatValue();
    //   addCommandBuffer(new renderer::ClearDepthCommandBuffer(depth));
    // }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::ClearStencil(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (info.Length() >= 1)
    // {
    //   int stencil = info[0].ToNumber().Int32Value();
    //   addCommandBuffer(new renderer::ClearStencilCommandBuffer(stencil));
    // }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Clear(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // if (info.Length() >= 1)
    // {
    //   uint32_t mask = info[0].ToNumber().Uint32Value();
    //   m_renderAPI->Clear(mask);
    // }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DepthMask(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      bool flag = info[0].ToBoolean().Value();
      auto req = DepthMaskCommandBufferRequest(flag);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DepthFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t func = info[0].ToNumber().Uint32Value();
      auto req = DepthFuncCommandBufferRequest(func);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DepthRange(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 2)
    {
      float near = info[0].ToNumber().FloatValue();
      float far = info[1].ToNumber().FloatValue();
      auto req = DepthRangeCommandBufferRequest(near, far);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::StencilFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 3)
    {
      uint32_t func = info[0].ToNumber().Uint32Value();
      int ref = info[1].ToNumber().Int32Value();
      uint32_t mask = info[2].ToNumber().Uint32Value();
      auto req = StencilFuncCommandBufferRequest(func, ref, mask);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::StencilFuncSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      uint32_t face = info[0].ToNumber().Uint32Value();
      uint32_t func = info[1].ToNumber().Uint32Value();
      int ref = info[2].ToNumber().Int32Value();
      uint32_t mask = info[3].ToNumber().Uint32Value();
      auto req = StencilFuncSeparateCommandBufferRequest(face, func, ref, mask);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::StencilMask(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      auto req = StencilMaskCommandBufferRequest(mask);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::StencilMaskSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 2)
    {
      uint32_t face = info[0].ToNumber().Uint32Value();
      uint32_t mask = info[1].ToNumber().Uint32Value();
      auto req = StencilMaskSeparateCommandBufferRequest(face, mask);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::StencilOp(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 3)
    {
      uint32_t fail = info[0].ToNumber().Uint32Value();
      uint32_t zfail = info[1].ToNumber().Uint32Value();
      uint32_t zpass = info[2].ToNumber().Uint32Value();
      auto req = StencilOpCommandBufferRequest(fail, zfail, zpass);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::StencilOpSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      uint32_t face = info[0].ToNumber().Uint32Value();
      uint32_t fail = info[1].ToNumber().Uint32Value();
      uint32_t zfail = info[2].ToNumber().Uint32Value();
      uint32_t zpass = info[3].ToNumber().Uint32Value();
      auto req = StencilOpSeparateCommandBufferRequest(face, fail, zfail, zpass);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendColor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 4)
    {
      float r = info[0].ToNumber().FloatValue();
      float g = info[1].ToNumber().FloatValue();
      float b = info[2].ToNumber().FloatValue();
      float a = info[3].ToNumber().FloatValue();
      auto req = BlendColorCommandBufferRequest(r, g, b, a);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendEquation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t mode = info[0].ToNumber().Uint32Value();
    auto req = BlendEquationCommandBufferRequest(mode);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendEquationSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t modeRGB = info[0].ToNumber().Uint32Value();
    uint32_t modeAlpha = info[1].ToNumber().Uint32Value();
    auto req = BlendEquationSeparateCommandBufferRequest(modeRGB, modeAlpha);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t sfactor = info[0].ToNumber().Uint32Value();
    uint32_t dfactor = info[1].ToNumber().Uint32Value();
    auto req = BlendFuncCommandBufferRequest(sfactor, dfactor);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendFuncSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t srcRGB = info[0].ToNumber().Uint32Value();
    uint32_t dstRGB = info[1].ToNumber().Uint32Value();
    uint32_t srcAlpha = info[2].ToNumber().Uint32Value();
    uint32_t dstAlpha = info[3].ToNumber().Uint32Value();
    auto req = BlendFuncSeparateCommandBufferRequest(srcRGB, dstRGB, srcAlpha, dstAlpha);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::ColorMask(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    bool r = info[0].ToBoolean().Value();
    bool g = info[1].ToBoolean().Value();
    bool b = info[2].ToBoolean().Value();
    bool a = info[3].ToBoolean().Value();
    auto req = ColorMaskCommandBufferRequest(r, g, b, a);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CullFace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int32_t mode = info[0].ToNumber().Int32Value();
    auto req = CullFaceCommandBufferRequest(mode);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::FrontFace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int32_t mode = info[0].ToNumber().Int32Value();
    auto req = FrontFaceCommandBufferRequest(mode);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Enable(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      auto req = EnableCommandBufferRequest(mask);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::Disable(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t mask = info[0].ToNumber().Uint32Value();
      auto req = DisableCommandBufferRequest(mask);
      sendCommandBufferRequest(req);
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t pname = info[0].ToNumber().Uint32Value();
      /**
       * Check for the static parameters which is defined at initialization.
       */
      if (pname == WEBGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
      {
        return Napi::Number::New(env, maxCombinedTextureImageUnits);
      }
      else if (pname == WEBGL_MAX_CUBE_MAP_TEXTURE_SIZE)
      {
        return Napi::Number::New(env, maxCubeMapTextureSize);
      }
      else if (pname == WEBGL_MAX_FRAGMENT_UNIFORM_VECTORS)
      {
        return Napi::Number::New(env, maxFragmentUniformVectors);
      }
      else if (pname == WEBGL_MAX_RENDERBUFFER_SIZE)
      {
        return Napi::Number::New(env, maxRenderbufferSize);
      }
      else if (pname == WEBGL_MAX_TEXTURE_IMAGE_UNITS)
      {
        return Napi::Number::New(env, maxTextureImageUnits);
      }
      else if (pname == WEBGL_MAX_TEXTURE_SIZE)
      {
        return Napi::Number::New(env, maxTextureSize);
      }
      else if (pname == WEBGL_MAX_VARYING_VECTORS)
      {
        return Napi::Number::New(env, maxVaryingVectors);
      }
      else if (pname == WEBGL_MAX_VERTEX_ATTRIBS)
      {
        return Napi::Number::New(env, maxVertexAttribs);
      }
      else if (pname == WEBGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS)
      {
        return Napi::Number::New(env, maxVertexTextureImageUnits);
      }
      else if (pname == WEBGL_MAX_VERTEX_UNIFORM_VECTORS)
      {
        return Napi::Number::New(env, maxVertexUniformVectors);
      }
      else if (pname == WEBGL_VENDOR)
      {
        return Napi::String::New(env, vendor);
      }
      else if (pname == WEBGL_RENDERER)
      {
        return Napi::String::New(env, renderer);
      }
      else if (pname == WEBGL_VERSION)
      {
        return Napi::String::New(env, version);
      }
      switch (pname)
      {
      // GLenum
      case WEBGL_ACTIVE_TEXTURE:
      case WEBGL_BLEND_DST_ALPHA:
      case WEBGL_BLEND_DST_RGB:
      case WEBGL_BLEND_EQUATION:
      // case WEBGL_BLEND_EQUATION_RGB: /** same as BLEND_EQUATION */
      case WEBGL_BLEND_EQUATION_ALPHA:
      case WEBGL_BLEND_SRC_ALPHA:
      case WEBGL_BLEND_SRC_RGB:
      case WEBGL_CULL_FACE_MODE:
      case WEBGL_DEPTH_FUNC:
      case WEBGL_FRONT_FACE:
      case WEBGL_GENERATE_MIPMAP_HINT:
      case WEBGL_IMPLEMENTATION_COLOR_READ_FORMAT:
      case WEBGL_IMPLEMENTATION_COLOR_READ_TYPE:
      case WEBGL_STENCIL_BACK_FAIL:
      case WEBGL_STENCIL_BACK_FUNC:
      case WEBGL_STENCIL_BACK_PASS_DEPTH_FAIL:
      case WEBGL_STENCIL_BACK_PASS_DEPTH_PASS:
      case WEBGL_STENCIL_FAIL:
      case WEBGL_STENCIL_FUNC:
      case WEBGL_STENCIL_PASS_DEPTH_FAIL:
      case WEBGL_STENCIL_PASS_DEPTH_PASS:
      case WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL:
      {
        auto req = GetIntegervCommandBufferRequest(pname);
        sendCommandBufferRequest(req, true);
        auto resp = recvCommandBufferResponse<GetIntegervCommandBufferResponse>(COMMAND_BUFFER_GET_INTEGERV_RES);
        if (resp == nullptr)
        {
          Napi::TypeError::New(env, "getParameter() failed to get the parameter: " + std::to_string(pname))
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
        auto value = Napi::Number::New(env, resp->value);
        delete resp;
        return value;
      }
      // GLboolean
      case WEBGL_BLEND:
      case WEBGL_CULL_FACE:
      case WEBGL_DEPTH_TEST:
      case WEBGL_DEPTH_WRITEMASK:
      case WEBGL_DITHER:
      case WEBGL_POLYGON_OFFSET_FILL:
      case WEBGL_SAMPLE_COVERAGE_INVERT:
      case WEBGL_SCISSOR_TEST:
      case WEBGL_STENCIL_TEST:
      case WEBGL_UNPACK_FLIP_Y_WEBGL:
      case WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL:
      {
        auto req = GetBooleanvCommandBufferRequest(pname);
        sendCommandBufferRequest(req, true);
        auto resp = recvCommandBufferResponse<GetBooleanvCommandBufferResponse>(COMMAND_BUFFER_GET_BOOLEANV_RES);
        if (resp == nullptr)
        {
          Napi::TypeError::New(env, "getParameter() failed to get the parameter: " + std::to_string(pname))
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
        auto value = Napi::Boolean::New(env, resp->value);
        delete resp;
        return value;
      }
      // GLint
      case WEBGL_ALPHA_BITS:
      case WEBGL_BLUE_BITS:
      case WEBGL_RED_BITS:
      case WEBGL_DEPTH_BITS:
      case WEBGL_GREEN_BITS:
      case WEBGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
      case WEBGL_MAX_CUBE_MAP_TEXTURE_SIZE:
      case WEBGL_MAX_FRAGMENT_UNIFORM_VECTORS:
      case WEBGL_MAX_RENDERBUFFER_SIZE:
      case WEBGL_MAX_TEXTURE_IMAGE_UNITS:
      case WEBGL_MAX_TEXTURE_SIZE:
      case WEBGL_MAX_VARYING_VECTORS:
      case WEBGL_MAX_VERTEX_ATTRIBS:
      case WEBGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
      case WEBGL_MAX_VERTEX_UNIFORM_VECTORS:
      case WEBGL_PACK_ALIGNMENT:
      case WEBGL_SAMPLE_BUFFERS:
      case WEBGL_SAMPLES:
      case WEBGL_STENCIL_BACK_REF:
      case WEBGL_STENCIL_BITS:
      case WEBGL_STENCIL_CLEAR_VALUE:
      case WEBGL_STENCIL_REF:
      case WEBGL_SUBPIXEL_BITS:
      case WEBGL_UNPACK_ALIGNMENT:
      {
        auto req = GetIntegervCommandBufferRequest(pname);
        sendCommandBufferRequest(req, true);
        auto resp = recvCommandBufferResponse<GetIntegervCommandBufferResponse>(COMMAND_BUFFER_GET_INTEGERV_RES);
        if (resp == nullptr)
        {
          Napi::TypeError::New(env, "getParameter() failed to get the parameter: " + std::to_string(pname))
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
        auto value = Napi::Number::New(env, resp->value);
        delete resp;
        return value;
      }
      // GLubyte/string
      case WEBGL_RENDERER:
      case WEBGL_SHADING_LANGUAGE_VERSION:
      case WEBGL_VENDOR:
      case WEBGL_VERSION:
      {
        auto req = GetStringCommandBufferRequest(pname);
        sendCommandBufferRequest(req, true);
        auto resp = recvCommandBufferResponse<GetStringCommandBufferResponse>(COMMAND_BUFFER_GET_STRING_RES);
        if (resp == nullptr)
        {
          Napi::TypeError::New(env, "getParameter() failed to get the parameter: " + std::to_string(pname))
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
        auto value = Napi::String::New(env, resp->value);
        delete resp;
        return value;
      }
      default:
        Napi::TypeError::New(env, "getParameter() don't support the parameter: " + std::to_string(pname))
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else
    {
      Napi::TypeError::New(env, "getParameter() takes 1 argument.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetShaderPrecisionFormat(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getShaderPrecisionFormat() takes 2 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    int shadertype = info[0].As<Napi::Number>().Int32Value();
    int precisiontype = info[1].As<Napi::Number>().Int32Value();

    auto req = GetShaderPrecisionFormatCommandBufferRequest(shadertype, precisiontype);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderPrecisionFormatCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_RES);
    if (resp == nullptr)
    {
      Napi::TypeError::New(env, "getShaderPrecisionFormat() failed to get the precision format.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("rangeMin", Napi::Number::New(env, resp->rangeMin));
    obj.Set("rangeMax", Napi::Number::New(env, resp->rangeMax));
    obj.Set("precision", Napi::Number::New(env, resp->precision));
    delete resp;
    return obj;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetError(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto req = GetErrorCommandBufferRequest();
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetErrorCommandBufferResponse>(COMMAND_BUFFER_GET_ERROR_RES);
    if (resp == nullptr)
    {
      Napi::TypeError::New(env, "getError() failed to get the error code.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto r = Napi::Number::New(env, resp->error);
    delete resp;
    return r;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetSupportedExtensions(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto jsThis = info.This().ToObject();
    if (jsThis.Has("_extensions") && jsThis.Get("_extensions").IsArray())
      return jsThis.Get("_extensions");

    auto req = GetExtensionsCommandBufferRequest();
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetExtensionsCommandBufferResponse>(COMMAND_BUFFER_GET_EXTENSIONS_RES);
    if (resp == nullptr)
    {
      Napi::TypeError::New(env, "getSupportedExtensions() failed to get the extensions.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Array extensionsArray = Napi::Array::New(env);
    for (size_t i = 0; i < resp->extensions.size(); i++)
    {
      // remove GL_ prefix
      std::string extension = resp->extensions[i];
      Napi::String jsExtensionName;
      if (extension.find("GL_") == 0)
        jsExtensionName = Napi::String::New(env, extension.substr(3));
      else
        jsExtensionName = Napi::String::New(env, extension);
      extensionsArray.Set(i, jsExtensionName);
    }
    delete resp;

    // Update the extensions array to the context object.
    jsThis.Set("_extensions", extensionsArray);
    return extensionsArray;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DrawingBufferWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, getDrawingBufferWidth());
  }

  template <typename T>
  void WebGLBaseRenderingContext<T>::DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferWidth is readonly.")
        .ThrowAsJavaScriptException();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DrawingBufferHeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, getDrawingBufferHeight());
  }

  template <typename T>
  void WebGLBaseRenderingContext<T>::DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferHeight is readonly.")
        .ThrowAsJavaScriptException();
  }

  template <typename T>
  int WebGLBaseRenderingContext<T>::getDrawingBufferWidth()
  {
    return viewport.width;
  }

  template <typename T>
  int WebGLBaseRenderingContext<T>::getDrawingBufferHeight()
  {
    return viewport.height;
  }

  template <typename T>
  bool WebGLBaseRenderingContext<T>::isWebGL2Context()
  {
    return isWebGL2;
  }

  Napi::Object WebGLRenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "WebGLRenderingContext",
        {
            WEBGL1_CONSTANTS,
            WEBGL1_METHODS(WebGLRenderingContext),
            WEBGL1_ACCESSORS(WebGLRenderingContext),
        });
    webglConstructor = new Napi::FunctionReference();
    *webglConstructor = Napi::Persistent(tpl);
    env.SetInstanceData(webglConstructor);
    exports.Set("WebGLRenderingContext", tpl);
    return exports;
  }

  WebGLRenderingContext::WebGLRenderingContext(const Napi::CallbackInfo &info) : WebGLBaseRenderingContext<WebGLRenderingContext>(info)
  {
    // TODO: add webgl1-specfic initialization here
  }

  Napi::Object WebGL2RenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "WebGL2RenderingContext",
        {
            WEBGL1_CONSTANTS,
            WEBGL2_CONSTANTS,
            WEBGL1_METHODS(WebGL2RenderingContext),
            WEBGL2_METHODS(WebGL2RenderingContext),
            WEBGL1_ACCESSORS(WebGL2RenderingContext),
        });
    webgl2Constructor = new Napi::FunctionReference();
    *webgl2Constructor = Napi::Persistent(tpl);
    env.SetInstanceData(webgl2Constructor);
    exports.Set("WebGL2RenderingContext", tpl);
    return exports;
  }

  WebGL2RenderingContext::WebGL2RenderingContext(const Napi::CallbackInfo &info) : WebGLBaseRenderingContext<WebGL2RenderingContext>(info)
  {
    // mark the webgl version to "webgl2"
    m_isWebGL2 = true;

    auto initCommand = WebGL2ContextInitCommandBufferRequest();
    sendCommandBufferRequest(initCommand, true);

    // Wait for the context init response
    auto resp = recvCommandBufferResponse<WebGL2ContextInitCommandBufferResponse>(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES);
    if (resp == nullptr)
    {
      Napi::TypeError::New(info.Env(), "Failed to initialize WebGL2RenderingContext.")
          .ThrowAsJavaScriptException();
      return;
    }

    this->max3DTextureSize = resp->max3DTextureSize;
    this->maxArrayTextureLayers = resp->maxArrayTextureLayers;
    this->maxColorAttachments = resp->maxColorAttachments;
    this->maxCombinedUniformBlocks = resp->maxCombinedUniformBlocks;
    this->maxDrawBuffers = resp->maxDrawBuffers;
    this->maxElementsIndices = resp->maxElementsIndices;
    this->maxElementsVertices = resp->maxElementsVertices;
    this->maxFragmentInputComponents = resp->maxFragmentInputComponents;
    this->maxFragmentUniformBlocks = resp->maxFragmentUniformBlocks;
    this->maxFragmentUniformComponents = resp->maxFragmentUniformComponents;
    this->maxProgramTexelOffset = resp->maxProgramTexelOffset;
    this->maxSamples = resp->maxSamples;
    this->maxTransformFeedbackInterleavedComponents = resp->maxTransformFeedbackInterleavedComponents;
    this->maxTransformFeedbackSeparateAttributes = resp->maxTransformFeedbackSeparateAttributes;
    this->maxTransformFeedbackSeparateComponents = resp->maxTransformFeedbackSeparateComponents;
    this->maxUniformBufferBindings = resp->maxUniformBufferBindings;
    this->maxVaryingComponents = resp->maxVaryingComponents;
    this->maxVertexOutputComponents = resp->maxVertexOutputComponents;
    this->maxVertexUniformBlocks = resp->maxVertexUniformBlocks;
    this->maxVertexUniformComponents = resp->maxVertexUniformComponents;
    this->minProgramTexelOffset = resp->minProgramTexelOffset;

    this->maxClientWaitTimeout = resp->maxClientWaitTimeout;
    this->maxCombinedFragmentUniformComponents = resp->maxCombinedFragmentUniformComponents;
    this->maxCombinedVertexUniformComponents = resp->maxCombinedVertexUniformComponents;
    this->maxElementIndex = resp->maxElementIndex;
    this->maxServerWaitTimeout = resp->maxServerWaitTimeout;
    this->maxUniformBlockSize = resp->maxUniformBlockSize;
    this->maxTextureLODBias = resp->maxTextureLODBias;
    delete resp;
  }

  Napi::Value WebGL2RenderingContext::GetParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1)
    {
      uint32_t pname = info[0].ToNumber().Uint32Value();
      if (pname == WEBGL2_MAX_3D_TEXTURE_SIZE)
        return Napi::Number::New(env, max3DTextureSize);
      else if (pname == WEBGL2_MAX_ARRAY_TEXTURE_LAYERS)
        return Napi::Number::New(env, maxArrayTextureLayers);
      else if (pname == WEBGL2_MAX_COLOR_ATTACHMENTS)
        return Napi::Number::New(env, maxColorAttachments);
      else if (pname == WEBGL2_MAX_COMBINED_UNIFORM_BLOCKS)
        return Napi::Number::New(env, maxCombinedUniformBlocks);
      else if (pname == WEBGL2_MAX_DRAW_BUFFERS)
        return Napi::Number::New(env, maxDrawBuffers);
      else if (pname == WEBGL2_MAX_ELEMENTS_INDICES)
        return Napi::Number::New(env, maxElementsIndices);
      else if (pname == WEBGL2_MAX_ELEMENTS_VERTICES)
        return Napi::Number::New(env, maxElementsVertices);
      else if (pname == WEBGL2_MAX_FRAGMENT_INPUT_COMPONENTS)
        return Napi::Number::New(env, maxFragmentInputComponents);
      else if (pname == WEBGL2_MAX_FRAGMENT_UNIFORM_BLOCKS)
        return Napi::Number::New(env, maxFragmentUniformBlocks);
      else if (pname == WEBGL2_MAX_FRAGMENT_UNIFORM_COMPONENTS)
        return Napi::Number::New(env, maxFragmentUniformComponents);
      else if (pname == WEBGL2_MAX_PROGRAM_TEXEL_OFFSET)
        return Napi::Number::New(env, maxProgramTexelOffset);
      else if (pname == WEBGL2_MAX_SAMPLES)
        return Napi::Number::New(env, maxSamples);
      else if (pname == WEBGL2_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS)
        return Napi::Number::New(env, maxTransformFeedbackInterleavedComponents);
      else if (pname == WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS)
        return Napi::Number::New(env, maxTransformFeedbackSeparateAttributes);
      else if (pname == WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS)
        return Napi::Number::New(env, maxTransformFeedbackSeparateComponents);
      else if (pname == WEBGL2_MAX_UNIFORM_BUFFER_BINDINGS)
        return Napi::Number::New(env, maxUniformBufferBindings);
      else if (pname == WEBGL2_MAX_VARYING_COMPONENTS)
        return Napi::Number::New(env, maxVaryingComponents);
      else if (pname == WEBGL2_MAX_VERTEX_OUTPUT_COMPONENTS)
        return Napi::Number::New(env, maxVertexOutputComponents);
      else if (pname == WEBGL2_MAX_VERTEX_UNIFORM_BLOCKS)
        return Napi::Number::New(env, maxVertexUniformBlocks);
      else if (pname == WEBGL2_MAX_VERTEX_UNIFORM_COMPONENTS)
        return Napi::Number::New(env, maxVertexUniformComponents);
      else if (pname == WEBGL2_MIN_PROGRAM_TEXEL_OFFSET)
        return Napi::Number::New(env, minProgramTexelOffset);
      else if (pname == WEBGL2_MAX_CLIENT_WAIT_TIMEOUT_WEBGL)
        return Napi::Number::New(env, maxClientWaitTimeout);
      else if (pname == WEBGL2_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS)
        return Napi::Number::New(env, maxCombinedFragmentUniformComponents);
      else if (pname == WEBGL2_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS)
        return Napi::Number::New(env, maxCombinedVertexUniformComponents);
      else if (pname == WEBGL2_MAX_ELEMENT_INDEX)
        return Napi::Number::New(env, maxElementIndex);
      else if (pname == WEBGL2_MAX_SERVER_WAIT_TIMEOUT)
        return Napi::Number::New(env, maxServerWaitTimeout);
      else if (pname == WEBGL2_MAX_UNIFORM_BLOCK_SIZE)
        return Napi::Number::New(env, maxUniformBlockSize);
      else if (pname == WEBGL2_MAX_TEXTURE_LOD_BIAS)
        return Napi::Number::New(env, maxTextureLODBias);
    }
    return WebGLBaseRenderingContext<WebGL2RenderingContext>::GetParameter(info);
  }

  Napi::Value WebGL2RenderingContext::ReadBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "readBuffer() takes 1 argument.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "readBuffer() 1st argument(src) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t src = info[0].As<Napi::Number>().Uint32Value();
    auto commandBuffer = ReadBufferCommandBufferRequest(src);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::BindBufferBase(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "bindBufferBase() takes 3 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferBase() 1st argument(target) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferBase() 2nd argument(index) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferBase() 3rd argument(buffer) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t target = info[0].As<Napi::Number>().Uint32Value();
    uint32_t index = info[1].As<Napi::Number>().Uint32Value();
    uint32_t buffer = 0;
    if (info[2].IsNumber())
      buffer = info[2].As<Napi::Number>().Uint32Value();

    auto commandBuffer = BindBufferBaseCommandBufferRequest(target, index, buffer);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::BindBufferRange(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "bindBufferRange() takes 5 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferRange() 1st argument(target) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferRange() 2nd argument(index) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferRange() 3rd argument(buffer) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[3].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferRange() 4th argument(offset) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[4].IsNumber())
    {
      Napi::TypeError::New(env, "bindBufferRange() 5th argument(size) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t target = info[0].As<Napi::Number>().Uint32Value();
    uint32_t index = info[1].As<Napi::Number>().Uint32Value();
    uint32_t buffer = info[2].As<Napi::Number>().Uint32Value();
    uint32_t offset = info[3].As<Napi::Number>().Uint32Value();
    uint32_t size = info[4].As<Napi::Number>().Uint32Value();

    auto commandBuffer = BindBufferRangeCommandBufferRequest(target, index, buffer, offset, size);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::BlitFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 10)
    {
      Napi::TypeError::New(env, "blitFramebuffer() takes 10 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t srcX0 = info[0].ToNumber().Uint32Value();
    uint32_t srcY0 = info[1].ToNumber().Uint32Value();
    uint32_t srcX1 = info[2].ToNumber().Uint32Value();
    uint32_t srcY1 = info[3].ToNumber().Uint32Value();
    uint32_t dstX0 = info[4].ToNumber().Uint32Value();
    uint32_t dstY0 = info[5].ToNumber().Uint32Value();
    uint32_t dstX1 = info[6].ToNumber().Uint32Value();
    uint32_t dstY1 = info[7].ToNumber().Uint32Value();
    uint32_t mask = info[8].ToNumber().Uint32Value();
    uint32_t filter = info[9].ToNumber().Uint32Value();

    auto commandBuffer = commandbuffers::BlitFramebufferCommandBufferRequest(
        srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::RenderbufferStorageMultisample(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "renderbufferStorageMultisample() takes 5 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t target = info[0].ToNumber().Uint32Value();
    uint32_t samples = info[1].ToNumber().Uint32Value();
    uint32_t internalformat = info[2].ToNumber().Uint32Value();
    uint32_t width = info[3].ToNumber().Uint32Value();
    uint32_t height = info[4].ToNumber().Uint32Value();

    auto commandBuffer = RenderbufferStorageMultisampleCommandBufferRequest(
        target, samples, internalformat, width, height);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::CreateVertexArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto clientId = vaoIdGen.get();
    auto commandBuffer = CreateVertexArrayCommandBufferRequest(clientId);
    sendCommandBufferRequest(commandBuffer);
    return Napi::Number::New(env, commandBuffer.clientId);
  }

  Napi::Value WebGL2RenderingContext::DeleteVertexArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "deleteVertexArray() takes 1 argument.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t vao = info[0].As<Napi::Number>().Uint32Value();
    auto commandBuffer = DeleteVertexArrayCommandBufferRequest(vao);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::BindVertexArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (TR_UNLIKELY(info.Length() <= 0))
    {
      Napi::TypeError::New(env, "bindVertexArray() takes 1 argument.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t vao = 0;
    if (info[0].IsNumber())
      vao = info[0].As<Napi::Number>().Uint32Value();

    BindVertexArrayCommandBufferRequest req(vao);
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::TexImage3D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 10)
    {
      string msg = "texImage3D() takes 10 arguments, but got " + to_string(info.Length()) + ".";
      Napi::TypeError::New(env, msg)
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    TextureImage3DCommandBufferRequest req;
    req.target = info[0].ToNumber().Uint32Value();
    req.level = info[1].ToNumber().Uint32Value();
    req.internalformat = info[2].ToNumber().Uint32Value();
    req.width = info[3].ToNumber().Uint32Value();
    req.height = info[4].ToNumber().Uint32Value();
    req.depth = info[5].ToNumber().Uint32Value();
    req.border = info[6].ToNumber().Uint32Value();
    req.format = info[7].ToNumber().Uint32Value();
    req.pixelType = info[8].ToNumber().Uint32Value();

    if (info[9].IsNumber())
    {
      Napi::TypeError::New(env, "texImage3D() with `offset` is not supported.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsSourceOrData = info[9];
    if (jsSourceOrData.IsNull() || jsSourceOrData.IsUndefined())
    {
      req.setPixels(nullptr);
    }
    else if (jsSourceOrData.IsTypedArray())
    {
      auto typedArray = jsSourceOrData.As<Napi::TypedArray>();
      auto pixels = static_cast<char *>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset();
      req.setPixels(pixels);
    }
    else if (jsSourceOrData.IsDataView())
    {
      auto dataView = jsSourceOrData.As<Napi::DataView>();
      auto pixels = static_cast<char *>(dataView.ArrayBuffer().Data()) + dataView.ByteOffset();
      req.setPixels(pixels);
    }
    else
    {
      Napi::TypeError::New(env, "texImage3D() with source is not supported.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    sendCommandBufferRequest(req);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::TexSubImage3D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 11)
    {
      Napi::TypeError::New(env, "texSubImage3D() takes 11 or 12 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto target = info[0].ToNumber().Uint32Value();
    auto level = info[1].ToNumber().Int32Value();
    auto xoffset = info[2].ToNumber().Int32Value();
    auto yoffset = info[3].ToNumber().Int32Value();
    auto zoffset = info[4].ToNumber().Int32Value();
    auto width = info[5].ToNumber().Int32Value();
    auto height = info[6].ToNumber().Int32Value();
    auto depth = info[7].ToNumber().Int32Value();
    auto format = info[8].ToNumber().Int32Value();
    auto type = info[9].ToNumber().Int32Value();

    auto jsSourceData = info[10];
    if (jsSourceData.IsNumber())
    {
      Napi::TypeError::New(env, "texSubImage3D() 11th argument(offset) is not supported.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    char *pixels = nullptr;
    size_t lenOfPixels = 0;

    if (jsSourceData.IsNull() || jsSourceData.IsUndefined())
    {
      // Do nothing
    }
    else if (jsSourceData.IsArrayBuffer())
    {
      auto arrayBuffer = jsSourceData.As<Napi::ArrayBuffer>();
      pixels = reinterpret_cast<char *>(arrayBuffer.Data());
      lenOfPixels = arrayBuffer.ByteLength();
    }
    else if (jsSourceData.IsTypedArray())
    {
      auto typedArray = jsSourceData.As<Napi::TypedArray>();
      pixels = reinterpret_cast<char *>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset();
      lenOfPixels = typedArray.ByteLength();
    }
    else
    {
      Napi::TypeError::New(env, "texSubImage3D() 11th argument(pixels) must be an ArrayBuffer or TypedArray.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto commandBuffer = commandbuffers::TextureSubImage3DCommandBufferRequest(
        target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::GetUniformBlockIndex(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "getUniformBlockIndex() takes 2 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "getUniformBlockIndex() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "getUniformBlockIndex() 2nd argument(name) must be a string.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();

    if (!program->HasUniformBlockIndex(name))
      return Napi::Number::New(env, -1);
    else
      return Napi::Number::New(env, program->GetUniformBlockIndex(name));
  }

  Napi::Value WebGL2RenderingContext::UniformBlockBinding(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniformBlockBinding() takes 3 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLProgram::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniformBlockBinding() 1st argument(program) must be a WebGLProgram.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "uniformBlockBinding() 2nd argument(uniformBlockIndex) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "uniformBlockBinding() 3rd argument(uniformBlockBinding) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    uint32_t uniformBlockIndex = info[1].As<Napi::Number>().Uint32Value();
    uint32_t uniformBlockBinding = info[2].As<Napi::Number>().Uint32Value();
    if (uniformBlockIndex < UINT32_MAX)
    {
      auto commandBuffer = commandbuffers::UniformBlockBindingCommandBufferRequest(
          program->GetId(), uniformBlockIndex, uniformBlockBinding);
      sendCommandBufferRequest(commandBuffer);
    }
    else
    {
      // Out of range.
      // TODO: make a warning
    }
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::VertexAttribIPointer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "vertexAttribIPointer() takes 5 arguments: index, size, type, stride, offset.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "vertexAttribIPointer() 1st argument(index) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "vertexAttribIPointer() 2nd argument(size) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "vertexAttribIPointer() 3rd argument(type) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[3].IsBoolean())
    {
      Napi::TypeError::New(env, "vertexAttribIPointer() 4th argument(stride) must be a boolean.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[4].IsNumber())
    {
      Napi::TypeError::New(env, "vertexAttribIPointer() 5th argument(offset) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t index = info[0].As<Napi::Number>().Uint32Value();
    uint32_t size = info[1].As<Napi::Number>().Uint32Value();
    uint32_t type = info[2].As<Napi::Number>().Uint32Value();
    uint32_t stride = info[3].As<Napi::Number>().Uint32Value();
    uint32_t offset = info[4].As<Napi::Number>().Uint32Value();

    auto commandBuffer = VertexAttribIPointerCommandBufferRequest(index, size, type, stride, offset);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::VertexAttribDivisor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "vertexAttribDivisor() takes 2 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "vertexAttribDivisor() 1st argument(index) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "vertexAttribDivisor() 2nd argument(divisor) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t index = info[0].As<Napi::Number>().Uint32Value();
    uint32_t divisor = info[1].As<Napi::Number>().Uint32Value();

    auto commandBuffer = VertexAttribDivisorCommandBufferRequest(index, divisor);
    sendCommandBufferRequest(commandBuffer);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::DrawBuffers(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "drawBuffers() takes 1 argument.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsArray())
    {
      Napi::TypeError::New(env, "drawBuffers() 1st argument(buffers) must be an array.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::vector<uint32_t> buffers;
    auto jsBuffers = info[0].As<Napi::Array>();
    for (size_t i = 0; i < jsBuffers.Length(); i++)
    {
      if (!jsBuffers.Get(i).IsNumber())
      {
        Napi::TypeError::New(env, "drawBuffers() 1st argument(buffers) must be an array of numbers.")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      buffers.push_back(jsBuffers.Get(i).As<Napi::Number>().Uint32Value());
    }
    auto commandBuffer = DrawBuffersCommandBufferRequest(buffers.size(), buffers.data());
    sendCommandBufferRequest(commandBuffer);
    sendFirstContentfulPaintMetrics();
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::DrawArraysInstanced(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 4)
    {
      Napi::TypeError::New(env, "drawArraysInstanced() takes 4 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "drawArraysInstanced() 1st argument(mode) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "drawArraysInstanced() 2nd argument(first) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "drawArraysInstanced() 3rd argument(count) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[3].IsNumber())
    {
      Napi::TypeError::New(env, "drawArraysInstanced() 4th argument(instanceCount) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int mode = info[0].As<Napi::Number>().Int32Value();
    int first = info[1].As<Napi::Number>().Int32Value();
    int count = info[2].As<Napi::Number>().Int32Value();
    int instanceCount = info[3].As<Napi::Number>().Int32Value();

    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawArraysInstanced()");
    auto commandBuffer = DrawArraysInstancedCommandBufferRequest(mode, first, count, instanceCount);
    sendCommandBufferRequest(commandBuffer);
    sendFirstContentfulPaintMetrics();
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::DrawElementsInstanced(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "drawElementsInstanced() takes 5 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "drawElementsInstanced() 1st argument(mode) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "drawElementsInstanced() 2nd argument(count) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "drawElementsInstanced() 3rd argument(type) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[3].IsNumber())
    {
      Napi::TypeError::New(env, "drawElementsInstanced() 4th argument(offset) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[4].IsNumber())
    {
      Napi::TypeError::New(env, "drawElementsInstanced() 5th argument(instanceCount) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t mode = info[0].As<Napi::Number>().Uint32Value();
    uint32_t count = info[1].As<Napi::Number>().Uint32Value();
    uint32_t type = info[2].As<Napi::Number>().Uint32Value();
    uint32_t offset = info[3].As<Napi::Number>().Uint32Value();
    uint32_t instanceCount = info[4].As<Napi::Number>().Uint32Value();

    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawElementsInstanced()");
    auto commandBuffer = DrawElementsInstancedCommandBufferRequest(mode, count, type, offset, instanceCount);
    sendCommandBufferRequest(commandBuffer);
    sendFirstContentfulPaintMetrics();
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::DrawRangeElements(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "drawRangeElements() takes 6 arguments.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "drawRangeElements() 1st argument(mode) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsNumber())
    {
      Napi::TypeError::New(env, "drawRangeElements() 2nd argument(start) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsNumber())
    {
      Napi::TypeError::New(env, "drawRangeElements() 3rd argument(end) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[3].IsNumber())
    {
      Napi::TypeError::New(env, "drawRangeElements() 4th argument(count) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[4].IsNumber())
    {
      Napi::TypeError::New(env, "drawRangeElements() 5th argument(type) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[5].IsNumber())
    {
      Napi::TypeError::New(env, "drawRangeElements() 6th argument(offset) must be a number.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    uint32_t mode = info[0].As<Napi::Number>().Uint32Value();
    uint32_t start = info[1].As<Napi::Number>().Uint32Value();
    uint32_t end = info[2].As<Napi::Number>().Uint32Value();
    uint32_t count = info[3].As<Napi::Number>().Uint32Value();
    uint32_t type = info[4].As<Napi::Number>().Uint32Value();
    uint32_t offset = info[5].As<Napi::Number>().Uint32Value();

    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawRangeElements()");
    auto commandBuffer = DrawRangeElementsCommandBufferRequest(mode, start, end, count, type, offset);
    sendCommandBufferRequest(commandBuffer);
    sendFirstContentfulPaintMetrics();
    return env.Undefined();
  }

  template class WebGLBaseRenderingContext<WebGLRenderingContext>;
  template class WebGLBaseRenderingContext<WebGL2RenderingContext>;
}
