#include <idgen.hpp>
#include <client/canvas/image_source.hpp>
#include <crates/bindings.hpp>

#include "./rendering_context-inl.hpp"
#include "./vertex_array.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLRenderingContext::constructor;
  thread_local Napi::FunctionReference *WebGL2RenderingContext::constructor;

#define TBASE(ObjectType, ContextType) WebGLBaseRenderingContext<ObjectType, ContextType>

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

#define WEBGL1_ACCESSORS(T)                                                                           \
  InstanceAccessor("drawingBufferWidth", &T::DrawingBufferWidthGetter, &T::DrawingBufferWidthSetter), \
    InstanceAccessor("drawingBufferHeight", &T::DrawingBufferHeightGetter, &T::DrawingBufferHeightSetter)

#define WEBGL1_METHODS(T)                                                       \
  InstanceMethod("makeXRCompatible", &T::MakeXRCompatible),                     \
    InstanceMethod("setDefaultCoordHandedness", &T::SetDefaultCoordHandedness), \
    InstanceMethod("isContextLost", &T::IsContextLost),                         \
    InstanceMethod("getContextAttributes", &T::GetContextAttributes),           \
    InstanceMethod("createProgram", &T::CreateProgram),                         \
    InstanceMethod("deleteProgram", &T::DeleteProgram),                         \
    InstanceMethod("linkProgram", &T::LinkProgram),                             \
    InstanceMethod("useProgram", &T::UseProgram),                               \
    InstanceMethod("bindAttribLocation", &T::BindAttribLocation),               \
    InstanceMethod("getProgramParameter", &T::GetProgramParameter),             \
    InstanceMethod("getProgramInfoLog", &T::GetProgramInfoLog),                 \
    InstanceMethod("attachShader", &T::AttachShader),                           \
    InstanceMethod("detachShader", &T::DetachShader),                           \
    InstanceMethod("createShader", &T::CreateShader),                           \
    InstanceMethod("deleteShader", &T::DeleteShader),                           \
    InstanceMethod("shaderSource", &T::ShaderSource, napi_default_jsproperty),  \
    InstanceMethod("compileShader", &T::CompileShader),                         \
    InstanceMethod("getShaderSource", &T::GetShaderSource),                     \
    InstanceMethod("getShaderParameter", &T::GetShaderParameter),               \
    InstanceMethod("getShaderInfoLog", &T::GetShaderInfoLog),                   \
    InstanceMethod("createBuffer", &T::CreateBuffer),                           \
    InstanceMethod("deleteBuffer", &T::DeleteBuffer),                           \
    InstanceMethod("bindBuffer", &T::BindBuffer),                               \
    InstanceMethod("bufferData", &T::BufferData, napi_default_jsproperty),      \
    InstanceMethod("bufferSubData", &T::BufferSubData),                         \
    InstanceMethod("createFramebuffer", &T::CreateFramebuffer),                 \
    InstanceMethod("deleteFramebuffer", &T::DeleteFramebuffer),                 \
    InstanceMethod("bindFramebuffer", &T::BindFramebuffer),                     \
    InstanceMethod("framebufferRenderbuffer", &T::FramebufferRenderbuffer),     \
    InstanceMethod("framebufferTexture2D", &T::FramebufferTexture2D),           \
    InstanceMethod("checkFramebufferStatus", &T::CheckFramebufferStatus),       \
    InstanceMethod("createRenderbuffer", &T::CreateRenderbuffer),               \
    InstanceMethod("deleteRenderbuffer", &T::DeleteRenderbuffer),               \
    InstanceMethod("bindRenderbuffer", &T::BindRenderbuffer),                   \
    InstanceMethod("renderbufferStorage", &T::RenderbufferStorage),             \
    InstanceMethod("createTexture", &T::CreateTexture),                         \
    InstanceMethod("deleteTexture", &T::DeleteTexture),                         \
    InstanceMethod("bindTexture", &T::BindTexture),                             \
    InstanceMethod("texImage2D", &T::TexImage2D),                               \
    InstanceMethod("texSubImage2D", &T::TexSubImage2D),                         \
    InstanceMethod("copyTexImage2D", &T::CopyTexImage2D),                       \
    InstanceMethod("copyTexSubImage2D", &T::CopyTexSubImage2D),                 \
    InstanceMethod("texParameteri", &T::TexParameteri),                         \
    InstanceMethod("texParameterf", &T::TexParameterf),                         \
    InstanceMethod("activeTexture", &T::ActiveTexture),                         \
    InstanceMethod("generateMipmap", &T::GenerateMipmap),                       \
    InstanceMethod("enableVertexAttribArray", &T::EnableVertexAttribArray),     \
    InstanceMethod("disableVertexAttribArray", &T::DisableVertexAttribArray),   \
    InstanceMethod("vertexAttribPointer", &T::VertexAttribPointer),             \
    InstanceMethod("getActiveAttrib", &T::GetActiveAttrib),                     \
    InstanceMethod("getActiveUniform", &T::GetActiveUniform),                   \
    InstanceMethod("getAttribLocation", &T::GetAttribLocation),                 \
    InstanceMethod("getUniformLocation", &T::GetUniformLocation),               \
    InstanceMethod("uniform1f", &T::Uniform1f),                                 \
    InstanceMethod("uniform1fv", &T::Uniform1fv),                               \
    InstanceMethod("uniform1i", &T::Uniform1i),                                 \
    InstanceMethod("uniform1iv", &T::Uniform1iv),                               \
    InstanceMethod("uniform2f", &T::Uniform2f),                                 \
    InstanceMethod("uniform2fv", &T::Uniform2fv),                               \
    InstanceMethod("uniform2i", &T::Uniform2i),                                 \
    InstanceMethod("uniform2iv", &T::Uniform2iv),                               \
    InstanceMethod("uniform3f", &T::Uniform3f),                                 \
    InstanceMethod("uniform3fv", &T::Uniform3fv),                               \
    InstanceMethod("uniform3i", &T::Uniform3i),                                 \
    InstanceMethod("uniform3iv", &T::Uniform3iv),                               \
    InstanceMethod("uniform4f", &T::Uniform4f),                                 \
    InstanceMethod("uniform4fv", &T::Uniform4fv),                               \
    InstanceMethod("uniform4i", &T::Uniform4i),                                 \
    InstanceMethod("uniform4iv", &T::Uniform4iv),                               \
    InstanceMethod("uniformMatrix2fv", &T::UniformMatrix2fv),                   \
    InstanceMethod("uniformMatrix3fv", &T::UniformMatrix3fv),                   \
    InstanceMethod("uniformMatrix4fv", &T::UniformMatrix4fv),                   \
    InstanceMethod("drawArrays", &T::DrawArrays),                               \
    InstanceMethod("drawElements", &T::DrawElements),                           \
    InstanceMethod("hint", &T::Hint),                                           \
    InstanceMethod("lineWidth", &T::LineWidth),                                 \
    InstanceMethod("pixelStorei", &T::PixelStorei),                             \
    InstanceMethod("polygonOffset", &T::PolygonOffset),                         \
    InstanceMethod("viewport", &T::Viewport),                                   \
    InstanceMethod("scissor", &T::Scissor),                                     \
    InstanceMethod("clearColor", &T::ClearColor),                               \
    InstanceMethod("clearDepth", &T::ClearDepth),                               \
    InstanceMethod("clearStencil", &T::ClearStencil),                           \
    InstanceMethod("clear", &T::Clear),                                         \
    InstanceMethod("depthMask", &T::DepthMask),                                 \
    InstanceMethod("depthFunc", &T::DepthFunc),                                 \
    InstanceMethod("depthRange", &T::DepthRange),                               \
    InstanceMethod("stencilFunc", &T::StencilFunc),                             \
    InstanceMethod("stencilFuncSeparate", &T::StencilFuncSeparate),             \
    InstanceMethod("stencilMask", &T::StencilMask),                             \
    InstanceMethod("stencilMaskSeparate", &T::StencilMaskSeparate),             \
    InstanceMethod("stencilOp", &T::StencilOp),                                 \
    InstanceMethod("stencilOpSeparate", &T::StencilOpSeparate),                 \
    InstanceMethod("blendColor", &T::BlendColor),                               \
    InstanceMethod("blendEquation", &T::BlendEquation),                         \
    InstanceMethod("blendEquationSeparate", &T::BlendEquationSeparate),         \
    InstanceMethod("blendFunc", &T::BlendFunc),                                 \
    InstanceMethod("blendFuncSeparate", &T::BlendFuncSeparate),                 \
    InstanceMethod("colorMask", &T::ColorMask),                                 \
    InstanceMethod("cullFace", &T::CullFace),                                   \
    InstanceMethod("frontFace", &T::FrontFace),                                 \
    InstanceMethod("enable", &T::Enable),                                       \
    InstanceMethod("disable", &T::Disable),                                     \
    InstanceMethod("getParameter", &T::GetParameter),                           \
    InstanceMethod("getShaderPrecisionFormat", &T::GetShaderPrecisionFormat),   \
    InstanceMethod("getError", &T::GetError),                                   \
    InstanceMethod("getSupportedExtensions", &T::GetSupportedExtensions)

#define WEBGL2_METHODS(T)                                                                 \
  InstanceMethod("readBuffer", &T::ReadBuffer),                                           \
    InstanceMethod("bindBufferBase", &T::BindBufferBase),                                 \
    InstanceMethod("bindBufferRange", &T::BindBufferRange),                               \
    InstanceMethod("blitFramebuffer", &T::BlitFramebuffer),                               \
    InstanceMethod("renderbufferStorageMultisample", &T::RenderbufferStorageMultisample), \
    InstanceMethod("createVertexArray", &T::CreateVertexArray),                           \
    InstanceMethod("deleteVertexArray", &T::DeleteVertexArray),                           \
    InstanceMethod("bindVertexArray", &T::BindVertexArray),                               \
    InstanceMethod("texImage3D", &T::TexImage3D),                                         \
    InstanceMethod("texSubImage3D", &T::TexSubImage3D),                                   \
    InstanceMethod("texStorage2D", &T::TexStorage2D),                                     \
    InstanceMethod("texStorage3D", &T::TexStorage3D),                                     \
    InstanceMethod("getUniformBlockIndex", &T::GetUniformBlockIndex),                     \
    InstanceMethod("uniformBlockBinding", &T::UniformBlockBinding),                       \
    InstanceMethod("vertexAttribIPointer", &T::VertexAttribIPointer),                     \
    InstanceMethod("vertexAttribDivisor", &T::VertexAttribDivisor),                       \
    InstanceMethod("drawBuffers", &T::DrawBuffers),                                       \
    InstanceMethod("drawArraysInstanced", &T::DrawArraysInstanced),                       \
    InstanceMethod("drawElementsInstanced", &T::DrawElementsInstanced),                   \
    InstanceMethod("drawRangeElements", &T::DrawRangeElements)

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
    WebGLRenderingContext::constructor = new Napi::FunctionReference();
    *WebGLRenderingContext::constructor = Napi::Persistent(tpl);
    env.SetInstanceData(WebGLRenderingContext::constructor);
    exports.Set("WebGLRenderingContext", tpl);
    return exports;
  }

  WebGLRenderingContext::WebGLRenderingContext(const Napi::CallbackInfo &info)
      : WebGLBaseRenderingContext<WebGLRenderingContext, client_graphics::WebGLContext>(info)
  {
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
    WebGL2RenderingContext::constructor = new Napi::FunctionReference();
    *WebGL2RenderingContext::constructor = Napi::Persistent(tpl);
    env.SetInstanceData(WebGL2RenderingContext::constructor);
    exports.Set("WebGL2RenderingContext", tpl);
    return exports;
  }

  WebGL2RenderingContext::WebGL2RenderingContext(const Napi::CallbackInfo &info)
      : WebGLBaseRenderingContext<WebGL2RenderingContext, client_graphics::WebGL2Context>(info)
  {
  }

  Napi::Value WebGL2RenderingContext::GetParameter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "getParameter() takes 1 argument.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Value value = env.Undefined();
    auto pname = info[0].ToNumber().Uint32Value();
    switch (pname)
    {
    case WEBGL2_MAX_ARRAY_TEXTURE_LAYERS:
    case WEBGL2_MAX_COLOR_ATTACHMENTS:
    case WEBGL2_MAX_COMBINED_UNIFORM_BLOCKS:
    case WEBGL2_MAX_DRAW_BUFFERS:
    case WEBGL2_MAX_ELEMENTS_INDICES:
    case WEBGL2_MAX_ELEMENTS_VERTICES:
    case WEBGL2_MAX_FRAGMENT_INPUT_COMPONENTS:
    case WEBGL2_MAX_FRAGMENT_UNIFORM_BLOCKS:
    case WEBGL2_MAX_FRAGMENT_UNIFORM_COMPONENTS:
    case WEBGL2_MAX_PROGRAM_TEXEL_OFFSET:
    case WEBGL2_MAX_SAMPLES:
    case WEBGL2_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS:
    case WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS:
    case WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS:
    case WEBGL2_MAX_UNIFORM_BUFFER_BINDINGS:
    case WEBGL2_MAX_VARYING_COMPONENTS:
    case WEBGL2_MAX_VERTEX_OUTPUT_COMPONENTS:
    case WEBGL2_MAX_VERTEX_UNIFORM_BLOCKS:
    case WEBGL2_MAX_VERTEX_UNIFORM_COMPONENTS:
    case WEBGL2_MIN_PROGRAM_TEXEL_OFFSET:
    case WEBGL2_MAX_CLIENT_WAIT_TIMEOUT_WEBGL:
    case WEBGL2_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
    case WEBGL2_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
    case WEBGL2_MAX_ELEMENT_INDEX:
    case WEBGL2_MAX_SERVER_WAIT_TIMEOUT:
    case WEBGL2_MAX_UNIFORM_BLOCK_SIZE:
    case WEBGL2_MAX_TEXTURE_LOD_BIAS:
    case WEBGL2_EXT_MAX_VIEWS_OVR:
      value = Napi::Number::New(env, glContext_->getParameterV2(static_cast<client_graphics::WebGL2IntegerParameterName>(pname)));
      break;
    default:
      break;
    }

    if (value.IsUndefined())
      return WebGLBaseRenderingContext::GetParameter(info);
    else
      return value;
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

    auto src = info[0].As<Napi::Number>().Int32Value();
    glContext_->readBuffer(src);
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
    if (!info[2].IsObject() || WebGLBuffer::IsInstanceOf(info[2]))
    {
      Napi::TypeError::New(env, "bindBufferBase() 3rd argument(buffer) must be an `WebGLBuffer` object.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto targetInt = info[0].As<Napi::Number>().Uint32Value();
    auto target = static_cast<client_graphics::WebGLBufferBindingTarget>(targetInt);
    auto index = info[1].As<Napi::Number>().Uint32Value();
    if (info[2].IsNumber())
    {
      auto buffer = WebGLBuffer::Unwrap(info[2].ToObject());
      glContext_->bindBufferBase(target, index, buffer->handle());
    }
    else
    {
      glContext_->bindBufferBase(target, index, nullptr);
    }
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
    if (!info[2].IsObject() || WebGLBuffer::IsInstanceOf(info[2]))
    {
      Napi::TypeError::New(env, "bindBufferRange() 3rd argument(buffer) must be an `WebGLBuffer` object.")
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

    auto target = info[0].As<Napi::Number>().Uint32Value();
    auto index = info[1].As<Napi::Number>().Uint32Value();
    auto buffer = WebGLBuffer::Unwrap(info[2].ToObject());
    auto offset = info[3].As<Napi::Number>().Int32Value();
    auto size = info[4].As<Napi::Number>().Uint32Value();

    glContext_->bindBufferRange(static_cast<client_graphics::WebGLBufferBindingTarget>(target),
                                index,
                                buffer->handle(),
                                offset,
                                size);
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

    auto srcX0 = info[0].ToNumber().Int32Value();
    auto srcY0 = info[1].ToNumber().Int32Value();
    auto srcX1 = info[2].ToNumber().Int32Value();
    auto srcY1 = info[3].ToNumber().Int32Value();
    auto dstX0 = info[4].ToNumber().Int32Value();
    auto dstY0 = info[5].ToNumber().Int32Value();
    auto dstX1 = info[6].ToNumber().Int32Value();
    auto dstY1 = info[7].ToNumber().Int32Value();
    auto mask = info[8].ToNumber().Int32Value();
    auto filter = info[9].ToNumber().Int32Value();

    glContext_->blitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
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
    auto samples = info[1].ToNumber().Int32Value();
    auto internalformat = info[2].ToNumber().Int32Value();
    auto width = info[3].ToNumber().Int32Value();
    auto height = info[4].ToNumber().Int32Value();

    glContext_->renderbufferStorageMultisample(static_cast<client_graphics::WebGLRenderbufferBindingTarget>(target),
                                               samples,
                                               internalformat,
                                               width,
                                               height);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::CreateVertexArray(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return WebGLVertexArray::NewInstance(env, glContext_->createVertexArray());
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
    if (!info[0].IsObject() || !WebGLVertexArray::IsInstanceOf(info[0]))
    {
      Napi::TypeError::New(env, "deleteVertexArray() 1st argument(vao) must be an `WebGLVertexArray` object.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto vao = WebGLVertexArray::Unwrap(info[0].ToObject());
    glContext_->deleteVertexArray(vao->handle());
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

    if (!info[0].IsObject() || !WebGLVertexArray::IsInstanceOf(info[0]))
    {
      glContext_->bindVertexArray(nullptr);
    }
    else
    {
      auto vao = WebGLVertexArray::Unwrap(info[0].ToObject());
      glContext_->bindVertexArray(vao->handle());
    }
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

    auto target = info[0].ToNumber().Uint32Value();
    auto level = info[1].ToNumber().Uint32Value();
    auto internalformat = info[2].ToNumber().Uint32Value();
    auto width = info[3].ToNumber().Uint32Value();
    auto height = info[4].ToNumber().Uint32Value();
    auto depth = info[5].ToNumber().Uint32Value();
    auto border = info[6].ToNumber().Uint32Value();
    auto format = info[7].ToNumber().Uint32Value();
    auto pixelType = info[8].ToNumber().Uint32Value();
    unsigned char *pixels = nullptr;

    if (info[9].IsNumber())
    {
      Napi::TypeError::New(env, "texImage3D() with `offset` is not supported.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsSourceOrData = info[9];
    if (jsSourceOrData.IsNull() || jsSourceOrData.IsUndefined())
    {
      pixels = nullptr;
    }
    else if (jsSourceOrData.IsTypedArray())
    {
      auto typedArray = jsSourceOrData.As<Napi::TypedArray>();
      pixels = static_cast<unsigned char *>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset();
    }
    else if (jsSourceOrData.IsDataView())
    {
      auto dataView = jsSourceOrData.As<Napi::DataView>();
      pixels = static_cast<unsigned char *>(dataView.ArrayBuffer().Data()) + dataView.ByteOffset();
    }
    else
    {
      Napi::TypeError::New(env, "texImage3D() with source is not supported.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    glContext_->texImage3D(static_cast<client_graphics::WebGLTexture3DTarget>(target),
                           level,
                           internalformat,
                           width,
                           height,
                           depth,
                           border,
                           static_cast<client_graphics::WebGLTextureFormat>(format),
                           static_cast<client_graphics::WebGLPixelType>(pixelType),
                           pixels);
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

    unsigned char *pixels = nullptr;
    size_t lenOfPixels = 0;

    if (jsSourceData.IsNull() || jsSourceData.IsUndefined())
    {
      // Do nothing
    }
    else if (jsSourceData.IsArrayBuffer())
    {
      auto arrayBuffer = jsSourceData.As<Napi::ArrayBuffer>();
      pixels = reinterpret_cast<unsigned char *>(arrayBuffer.Data());
      lenOfPixels = arrayBuffer.ByteLength();
    }
    else if (jsSourceData.IsTypedArray())
    {
      auto typedArray = jsSourceData.As<Napi::TypedArray>();
      pixels = reinterpret_cast<unsigned char *>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset();
      lenOfPixels = typedArray.ByteLength();
    }
    else
    {
      Napi::TypeError::New(env, "texSubImage3D() 11th argument(pixels) must be an ArrayBuffer or TypedArray.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    glContext_->texSubImage3D(static_cast<client_graphics::WebGLTexture3DTarget>(target),
                              level,
                              xoffset,
                              yoffset,
                              zoffset,
                              width,
                              height,
                              depth,
                              static_cast<client_graphics::WebGLTextureFormat>(format),
                              static_cast<client_graphics::WebGLPixelType>(type),
                              pixels);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::TexStorage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 5)
    {
      Napi::TypeError::New(env, "texStorage2D() takes 5 arguments.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto target = info[0].ToNumber().Uint32Value();
    auto levels = info[1].ToNumber().Uint32Value();
    auto internalformat = info[2].ToNumber().Uint32Value();
    auto width = info[3].ToNumber().Uint32Value();
    auto height = info[4].ToNumber().Uint32Value();

    glContext_->texStorage2D(static_cast<client_graphics::WebGLTexture2DTarget>(target),
                             levels,
                             internalformat,
                             width,
                             height);
    return env.Undefined();
  }

  Napi::Value WebGL2RenderingContext::TexStorage3D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 6)
    {
      Napi::TypeError::New(env, "texStorage3D() takes 6 arguments.")
        .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto target = info[0].ToNumber().Uint32Value();
    auto levels = info[1].ToNumber().Uint32Value();
    auto internalformat = info[2].ToNumber().Uint32Value();
    auto width = info[3].ToNumber().Uint32Value();
    auto height = info[4].ToNumber().Uint32Value();
    auto depth = info[5].ToNumber().Uint32Value();

    glContext_->texStorage3D(static_cast<client_graphics::WebGLTexture3DTarget>(target),
                             levels,
                             internalformat,
                             width,
                             height,
                             depth);
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
    if (!WebGLProgram::IsInstanceOf(info[0]))
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
    return Napi::Number::New(env, glContext_->getUniformBlockIndex(program->handle(), name));
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
    if (!WebGLProgram::IsInstanceOf(info[0]))
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
    auto uniformBlockIndex = info[1].As<Napi::Number>().Int32Value();
    auto uniformBlockBinding = info[2].As<Napi::Number>().Uint32Value();
    glContext_->uniformBlockBinding(program->handle(), uniformBlockIndex, uniformBlockBinding);
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

    auto index = info[0].As<Napi::Number>().Uint32Value();
    auto size = info[1].As<Napi::Number>().Uint32Value();
    auto type = info[2].As<Napi::Number>().Uint32Value();
    auto stride = info[3].As<Napi::Number>().Uint32Value();
    auto offset = info[4].As<Napi::Number>().Uint32Value();

    glContext_->vertexAttribIPointer(index, size, type, stride, offset);
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

    auto index = info[0].As<Napi::Number>().Uint32Value();
    auto divisor = info[1].As<Napi::Number>().Uint32Value();

    glContext_->vertexAttribDivisor(index, divisor);
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
    glContext_->drawBuffers(buffers);
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

    auto mode = info[0].As<Napi::Number>().Int32Value();
    auto first = info[1].As<Napi::Number>().Int32Value();
    auto count = info[2].As<Napi::Number>().Int32Value();
    auto instanceCount = info[3].As<Napi::Number>().Int32Value();
    glContext_->drawArraysInstanced(static_cast<client_graphics::WebGLDrawMode>(mode), first, count, instanceCount);
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

    auto mode = info[0].As<Napi::Number>().Uint32Value();
    auto count = info[1].As<Napi::Number>().Uint32Value();
    auto type = info[2].As<Napi::Number>().Uint32Value();
    auto offset = info[3].As<Napi::Number>().Uint32Value();
    auto instanceCount = info[4].As<Napi::Number>().Uint32Value();

    glContext_->drawElementsInstanced(static_cast<client_graphics::WebGLDrawMode>(mode), count, type, offset, instanceCount);
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

    auto mode = info[0].As<Napi::Number>().Uint32Value();
    auto start = info[1].As<Napi::Number>().Uint32Value();
    auto end = info[2].As<Napi::Number>().Uint32Value();
    auto count = info[3].As<Napi::Number>().Uint32Value();
    auto type = info[4].As<Napi::Number>().Uint32Value();
    auto offset = info[5].As<Napi::Number>().Uint32Value();

    glContext_->drawRangeElements(static_cast<client_graphics::WebGLDrawMode>(mode), start, end, count, type, offset);
    return env.Undefined();
  }
}
