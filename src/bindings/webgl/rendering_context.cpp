#include "rendering_context.hpp"
#include "renderer/command_buffer.hpp"
#include "renderer/render_api.hpp"
#include "renderer/constants.hpp"

#include "program.hpp"
#include "texture.hpp"
#include "uniform_location.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLRenderingContext::constructor;
  Napi::FunctionReference *WebGL2RenderingContext::constructor;

#define WEBGL1_CONSTANTS_CLEARING_BUFFERS                                                    \
  InstanceValue("DEPTH_BUFFER_BIT", Napi::Number::New(env, WEBGL_DEPTH_BUFFER_BIT)),         \
      InstanceValue("STENCIL_BUFFER_BIT", Napi::Number::New(env, WEBGL_STENCIL_BUFFER_BIT)), \
      InstanceValue("COLOR_BUFFER_BIT", Napi::Number::New(env, WEBGL_COLOR_BUFFER_BIT))

#define WEBGL1_CONSTANTS_RENDERING_PRIMITIVES                                        \
  InstanceValue("POINTS", Napi::Number::New(env, WEBGL_POINTS)),                     \
      InstanceValue("LINES", Napi::Number::New(env, WEBGL_LINES)),                   \
      InstanceValue("LINE_LOOP", Napi::Number::New(env, WEBGL_LINE_LOOP)),           \
      InstanceValue("LINE_STRIP", Napi::Number::New(env, WEBGL_LINE_STRIP)),         \
      InstanceValue("TRIANGLES", Napi::Number::New(env, WEBGL_TRIANGLES)),           \
      InstanceValue("TRIANGLE_STRIP", Napi::Number::New(env, WEBGL_TRIANGLE_STRIP)), \
      InstanceValue("TRIANGLE_FAN", Napi::Number::New(env, WEBGL_TRIANGLE_FAN))

#define WEBGL1_CONSTANTS_BLENDING                                                                                        \
  InstanceValue("ZERO", Napi::Number::New(env, WEBGL_ZERO)),                                                             \
      InstanceValue("ONE", Napi::Number::New(env, WEBGL_ONE)),                                                           \
      InstanceValue("SRC_COLOR", Napi::Number::New(env, WEBGL_SRC_COLOR)),                                               \
      InstanceValue("ONE_MINUS_SRC_COLOR", Napi::Number::New(env, WEBGL_ONE_MINUS_SRC_COLOR)),                           \
      InstanceValue("SRC_ALPHA", Napi::Number::New(env, WEBGL_SRC_ALPHA)),                                               \
      InstanceValue("ONE_MINUS_SRC_ALPHA", Napi::Number::New(env, WEBGL_ONE_MINUS_SRC_ALPHA)),                           \
      InstanceValue("DST_ALPHA", Napi::Number::New(env, WEBGL_DST_ALPHA)),                                               \
      InstanceValue("ONE_MINUS_DST_ALPHA", Napi::Number::New(env, WEBGL_ONE_MINUS_DST_ALPHA)),                           \
      InstanceValue("DST_COLOR", Napi::Number::New(env, WEBGL_DST_COLOR)),                                               \
      InstanceValue("ONE_MINUS_DST_COLOR", Napi::Number::New(env, WEBGL_ONE_MINUS_DST_COLOR)),                           \
      InstanceValue("SRC_ALPHA_SATURATE", Napi::Number::New(env, WEBGL_SRC_ALPHA_SATURATE)),                             \
      InstanceValue("CONSTANT_COLOR", Napi::Number::New(env, WEBGL_CONSTANT_COLOR)),                                     \
      InstanceValue("ONE_MINUS_CONSTANT_COLOR", Napi::Number::New(env, WEBGL_ONE_MINUS_CONSTANT_COLOR)),                 \
      InstanceValue("CONSTANT_ALPHA", Napi::Number::New(env, WEBGL_CONSTANT_ALPHA)),                                     \
      InstanceValue("ONE_MINUS_CONSTANT_ALPHA", Napi::Number::New(env, WEBGL_ONE_MINUS_CONSTANT_ALPHA)),                 \
      InstanceValue("FUNC_ADD", Napi::Number::New(env, WEBGL_FUNC_ADD)),                                                 \
      InstanceValue("FUNC_SUBTRACT", Napi::Number::New(env, WEBGL_FUNC_SUBTRACT)),                                       \
      InstanceValue("FUNC_REVERSE_SUBTRACT", Napi::Number::New(env, WEBGL_FUNC_REVERSE_SUBTRACT)),                       \
      InstanceValue("BLEND_EQUATION", Napi::Number::New(env, WEBGL_BLEND_EQUATION)),                                     \
      InstanceValue("BLEND_EQUATION_RGB", Napi::Number::New(env, WEBGL_BLEND_EQUATION_RGB)),                             \
      InstanceValue("BLEND_EQUATION_ALPHA", Napi::Number::New(env, WEBGL_BLEND_EQUATION_ALPHA)),                         \
      InstanceValue("BLEND_DST_RGB", Napi::Number::New(env, WEBGL_BLEND_DST_RGB)),                                       \
      InstanceValue("BLEND_SRC_RGB", Napi::Number::New(env, WEBGL_BLEND_SRC_RGB)),                                       \
      InstanceValue("BLEND_DST_ALPHA", Napi::Number::New(env, WEBGL_BLEND_DST_ALPHA)),                                   \
      InstanceValue("BLEND_SRC_ALPHA", Napi::Number::New(env, WEBGL_BLEND_SRC_ALPHA)),                                   \
      InstanceValue("BLEND_COLOR", Napi::Number::New(env, WEBGL_BLEND_COLOR)),                                           \
      InstanceValue("ARRAY_BUFFER_BINDING", Napi::Number::New(env, WEBGL_ARRAY_BUFFER_BINDING)),                         \
      InstanceValue("ELEMENT_ARRAY_BUFFER_BINDING", Napi::Number::New(env, WEBGL_ELEMENT_ARRAY_BUFFER_BINDING)),         \
      InstanceValue("LINE_WIDTH", Napi::Number::New(env, WEBGL_LINE_WIDTH)),                                             \
      InstanceValue("ALIASED_POINT_SIZE_RANGE", Napi::Number::New(env, WEBGL_ALIASED_POINT_SIZE_RANGE)),                 \
      InstanceValue("ALIASED_LINE_WIDTH_RANGE", Napi::Number::New(env, WEBGL_ALIASED_LINE_WIDTH_RANGE)),                 \
      InstanceValue("CULL_FACE_MODE", Napi::Number::New(env, WEBGL_CULL_FACE_MODE)),                                     \
      InstanceValue("FRONT_FACE", Napi::Number::New(env, WEBGL_FRONT_FACE)),                                             \
      InstanceValue("DEPTH_RANGE", Napi::Number::New(env, WEBGL_DEPTH_RANGE)),                                           \
      InstanceValue("DEPTH_WRITEMASK", Napi::Number::New(env, WEBGL_DEPTH_WRITEMASK)),                                   \
      InstanceValue("DEPTH_CLEAR_VALUE", Napi::Number::New(env, WEBGL_DEPTH_CLEAR_VALUE)),                               \
      InstanceValue("DEPTH_FUNC", Napi::Number::New(env, WEBGL_DEPTH_FUNC)),                                             \
      InstanceValue("STENCIL_CLEAR_VALUE", Napi::Number::New(env, WEBGL_STENCIL_CLEAR_VALUE)),                           \
      InstanceValue("STENCIL_FUNC", Napi::Number::New(env, WEBGL_STENCIL_FUNC)),                                         \
      InstanceValue("STENCIL_FAIL", Napi::Number::New(env, WEBGL_STENCIL_FAIL)),                                         \
      InstanceValue("STENCIL_PASS_DEPTH_FAIL", Napi::Number::New(env, WEBGL_STENCIL_PASS_DEPTH_FAIL)),                   \
      InstanceValue("STENCIL_PASS_DEPTH_PASS", Napi::Number::New(env, WEBGL_STENCIL_PASS_DEPTH_PASS)),                   \
      InstanceValue("STENCIL_REF", Napi::Number::New(env, WEBGL_STENCIL_REF)),                                           \
      InstanceValue("STENCIL_VALUE_MASK", Napi::Number::New(env, WEBGL_STENCIL_VALUE_MASK)),                             \
      InstanceValue("STENCIL_WRITEMASK", Napi::Number::New(env, WEBGL_STENCIL_WRITEMASK)),                               \
      InstanceValue("STENCIL_BACK_FUNC", Napi::Number::New(env, WEBGL_STENCIL_BACK_FUNC)),                               \
      InstanceValue("STENCIL_BACK_FAIL", Napi::Number::New(env, WEBGL_STENCIL_BACK_FAIL)),                               \
      InstanceValue("STENCIL_BACK_PASS_DEPTH_FAIL", Napi::Number::New(env, WEBGL_STENCIL_BACK_PASS_DEPTH_FAIL)),         \
      InstanceValue("STENCIL_BACK_PASS_DEPTH_PASS", Napi::Number::New(env, WEBGL_STENCIL_BACK_PASS_DEPTH_PASS)),         \
      InstanceValue("STENCIL_BACK_REF", Napi::Number::New(env, WEBGL_STENCIL_BACK_REF)),                                 \
      InstanceValue("STENCIL_BACK_VALUE_MASK", Napi::Number::New(env, WEBGL_STENCIL_BACK_VALUE_MASK)),                   \
      InstanceValue("STENCIL_BACK_WRITEMASK", Napi::Number::New(env, WEBGL_STENCIL_BACK_WRITEMASK)),                     \
      InstanceValue("VIEWPORT", Napi::Number::New(env, WEBGL_VIEWPORT)),                                                 \
      InstanceValue("SCISSOR_BOX", Napi::Number::New(env, WEBGL_SCISSOR_BOX)),                                           \
      InstanceValue("COLOR_CLEAR_VALUE", Napi::Number::New(env, WEBGL_COLOR_CLEAR_VALUE)),                               \
      InstanceValue("COLOR_WRITEMASK", Napi::Number::New(env, WEBGL_COLOR_WRITEMASK)),                                   \
      InstanceValue("UNPACK_ALIGNMENT", Napi::Number::New(env, WEBGL_UNPACK_ALIGNMENT)),                                 \
      InstanceValue("PACK_ALIGNMENT", Napi::Number::New(env, WEBGL_PACK_ALIGNMENT)),                                     \
      InstanceValue("MAX_TEXTURE_SIZE", Napi::Number::New(env, WEBGL_MAX_TEXTURE_SIZE)),                                 \
      InstanceValue("MAX_VIEWPORT_DIMS", Napi::Number::New(env, WEBGL_MAX_VIEWPORT_DIMS)),                               \
      InstanceValue("SUBPIXEL_BITS", Napi::Number::New(env, WEBGL_SUBPIXEL_BITS)),                                       \
      InstanceValue("RED_BITS", Napi::Number::New(env, WEBGL_RED_BITS)),                                                 \
      InstanceValue("GREEN_BITS", Napi::Number::New(env, WEBGL_GREEN_BITS)),                                             \
      InstanceValue("BLUE_BITS", Napi::Number::New(env, WEBGL_BLUE_BITS)),                                               \
      InstanceValue("ALPHA_BITS", Napi::Number::New(env, WEBGL_ALPHA_BITS)),                                             \
      InstanceValue("DEPTH_BITS", Napi::Number::New(env, WEBGL_DEPTH_BITS)),                                             \
      InstanceValue("STENCIL_BITS", Napi::Number::New(env, WEBGL_STENCIL_BITS)),                                         \
      InstanceValue("POLYGON_OFFSET_UNITS", Napi::Number::New(env, WEBGL_POLYGON_OFFSET_UNITS)),                         \
      InstanceValue("POLYGON_OFFSET_FACTOR", Napi::Number::New(env, WEBGL_POLYGON_OFFSET_FACTOR)),                       \
      InstanceValue("TEXTURE_BINDING_2D", Napi::Number::New(env, WEBGL_TEXTURE_BINDING_2D)),                             \
      InstanceValue("SAMPLE_BUFFERS", Napi::Number::New(env, WEBGL_SAMPLE_BUFFERS)),                                     \
      InstanceValue("SAMPLES", Napi::Number::New(env, WEBGL_SAMPLES)),                                                   \
      InstanceValue("SAMPLE_COVERAGE_VALUE", Napi::Number::New(env, WEBGL_SAMPLE_COVERAGE_VALUE)),                       \
      InstanceValue("SAMPLE_COVERAGE_INVERT", Napi::Number::New(env, WEBGL_SAMPLE_COVERAGE_INVERT)),                     \
      InstanceValue("COMPRESSED_TEXTURE_FORMATS", Napi::Number::New(env, WEBGL_COMPRESSED_TEXTURE_FORMATS)),             \
      InstanceValue("VENDOR", Napi::Number::New(env, WEBGL_VENDOR)),                                                     \
      InstanceValue("RENDERER", Napi::Number::New(env, WEBGL_RENDERER)),                                                 \
      InstanceValue("VERSION", Napi::Number::New(env, WEBGL_VERSION)),                                                   \
      InstanceValue("IMPLEMENTATION_COLOR_READ_TYPE", Napi::Number::New(env, WEBGL_IMPLEMENTATION_COLOR_READ_TYPE)),     \
      InstanceValue("IMPLEMENTATION_COLOR_READ_FORMAT", Napi::Number::New(env, WEBGL_IMPLEMENTATION_COLOR_READ_FORMAT)), \
      InstanceValue("BROWSER_DEFAULT_WEBGL", Napi::Number::New(env, WEBGL_BROWSER_DEFAULT_WEBGL))

#define WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS                                                                     \
  InstanceValue("STATIC_DRAW", Napi::Number::New(env, WEBGL_STATIC_DRAW)),                                           \
      InstanceValue("STREAM_DRAW", Napi::Number::New(env, WEBGL_STREAM_DRAW)),                                       \
      InstanceValue("DYNAMIC_DRAW", Napi::Number::New(env, WEBGL_DYNAMIC_DRAW)),                                     \
      InstanceValue("ARRAY_BUFFER", Napi::Number::New(env, WEBGL_ARRAY_BUFFER)),                                     \
      InstanceValue("ELEMENT_ARRAY_BUFFER", Napi::Number::New(env, WEBGL_ELEMENT_ARRAY_BUFFER)),                     \
      InstanceValue("BUFFER_SIZE", Napi::Number::New(env, WEBGL_BUFFER_SIZE)),                                       \
      InstanceValue("BUFFER_USAGE", Napi::Number::New(env, WEBGL_BUFFER_USAGE)),                                     \
      InstanceValue("CURRENT_VERTEX_ATTRIB", Napi::Number::New(env, WEBGL_CURRENT_VERTEX_ATTRIB)),                   \
      InstanceValue("VERTEX_ATTRIB_ARRAY_ENABLED", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_ENABLED)),       \
      InstanceValue("VERTEX_ATTRIB_ARRAY_SIZE", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_SIZE)),             \
      InstanceValue("VERTEX_ATTRIB_ARRAY_STRIDE", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_STRIDE)),         \
      InstanceValue("VERTEX_ATTRIB_ARRAY_TYPE", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_TYPE)),             \
      InstanceValue("VERTEX_ATTRIB_ARRAY_NORMALIZED", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_NORMALIZED)), \
      InstanceValue("VERTEX_ATTRIB_ARRAY_POINTER", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_POINTER)),       \
      InstanceValue("VERTEX_ATTRIB_ARRAY_BUFFER_BINDING", Napi::Number::New(env, WEBGL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING))

#define WEBGL1_CONSTANTS_CULLING                                       \
  InstanceValue("CULL_FACE", Napi::Number::New(env, WEBGL_CULL_FACE)), \
      InstanceValue("FRONT", Napi::Number::New(env, WEBGL_FRONT)),     \
      InstanceValue("BACK", Napi::Number::New(env, WEBGL_BACK)),       \
      InstanceValue("FRONT_AND_BACK", Napi::Number::New(env, WEBGL_FRONT_AND_BACK))

#define WEBGL1_CONSTANTS_ENABLE_AND_DISABLE                                                              \
  InstanceValue("BLEND", Napi::Number::New(env, WEBGL_BLEND)),                                           \
      InstanceValue("DEPTH_TEST", Napi::Number::New(env, WEBGL_DEPTH_TEST)),                             \
      InstanceValue("DITHER", Napi::Number::New(env, WEBGL_DITHER)),                                     \
      InstanceValue("POLYGON_OFFSET_FILL", Napi::Number::New(env, WEBGL_POLYGON_OFFSET_FILL)),           \
      InstanceValue("SAMPLE_ALPHA_TO_COVERAGE", Napi::Number::New(env, WEBGL_SAMPLE_ALPHA_TO_COVERAGE)), \
      InstanceValue("SAMPLE_COVERAGE", Napi::Number::New(env, WEBGL_SAMPLE_COVERAGE)),                   \
      InstanceValue("SCISSOR_TEST", Napi::Number::New(env, WEBGL_SCISSOR_TEST)),                         \
      InstanceValue("STENCIL_TEST", Napi::Number::New(env, WEBGL_STENCIL_TEST))

#define WEBGL1_CONSTANTS_ERRORS                                                            \
  InstanceValue("NO_ERROR", Napi::Number::New(env, WEBGL_NO_ERROR)),                       \
      InstanceValue("INVALID_ENUM", Napi::Number::New(env, WEBGL_INVALID_ENUM)),           \
      InstanceValue("INVALID_VALUE", Napi::Number::New(env, WEBGL_INVALID_VALUE)),         \
      InstanceValue("INVALID_OPERATION", Napi::Number::New(env, WEBGL_INVALID_OPERATION)), \
      InstanceValue("OUT_OF_MEMORY", Napi::Number::New(env, WEBGL_OUT_OF_MEMORY)),         \
      InstanceValue("CONTEXT_LOST_WEBGL", Napi::Number::New(env, WEBGL_CONTEXT_LOST_WEBGL))

#define WEBGL1_CONSTANTS_FRONTFACE                       \
  InstanceValue("CW", Napi::Number::New(env, WEBGL_CW)), \
      InstanceValue("CCW", Napi::Number::New(env, WEBGL_CCW))

#define WEBGL1_CONSTANTS_HINTS                                         \
  InstanceValue("DONT_CARE", Napi::Number::New(env, WEBGL_DONT_CARE)), \
      InstanceValue("FASTEST", Napi::Number::New(env, WEBGL_FASTEST)), \
      InstanceValue("NICEST", Napi::Number::New(env, WEBGL_NICEST))

#define WEBGL1_CONSTANTS_DATATYPES                                                   \
  InstanceValue("BYTE", Napi::Number::New(env, WEBGL_BYTE)),                         \
      InstanceValue("UNSIGNED_BYTE", Napi::Number::New(env, WEBGL_UNSIGNED_BYTE)),   \
      InstanceValue("SHORT", Napi::Number::New(env, WEBGL_SHORT)),                   \
      InstanceValue("UNSIGNED_SHORT", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT)), \
      InstanceValue("INT", Napi::Number::New(env, WEBGL_INT)),                       \
      InstanceValue("UNSIGNED_INT", Napi::Number::New(env, WEBGL_UNSIGNED_INT)),     \
      InstanceValue("FLOAT", Napi::Number::New(env, WEBGL_FLOAT))

#define WEBGL1_CONSTANTS_PIXELS                                                                                      \
  InstanceValue("DEPTH_COMPONENT", Napi::Number::New(env, WEBGL_DEPTH_COMPONENT)),                                   \
      InstanceValue("ALPHA", Napi::Number::New(env, WEBGL_ALPHA)),                                                   \
      InstanceValue("RGB", Napi::Number::New(env, WEBGL_RGB)),                                                       \
      InstanceValue("RGBA", Napi::Number::New(env, WEBGL_RGBA)),                                                     \
      InstanceValue("LUMINANCE", Napi::Number::New(env, WEBGL_LUMINANCE)),                                           \
      InstanceValue("LUMINANCE_ALPHA", Napi::Number::New(env, WEBGL_LUMINANCE_ALPHA)),                               \
      InstanceValue("UNSIGNED_SHORT_4_4_4_4", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT_4_4_4_4)),                 \
      InstanceValue("UNSIGNED_SHORT_5_5_5_1", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT_5_5_5_1)),                 \
      InstanceValue("UNSIGNED_SHORT_5_6_5", Napi::Number::New(env, WEBGL_UNSIGNED_SHORT_5_6_5)),                     \
      InstanceValue("UNPACK_FLIP_Y_WEBGL", Napi::Number::New(env, WEBGL_UNPACK_FLIP_Y_WEBGL)),                       \
      InstanceValue("UNPACK_PREMULTIPLY_ALPHA_WEBGL", Napi::Number::New(env, WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL)), \
      InstanceValue("UNPACK_COLORSPACE_CONVERSION_WEBGL", Napi::Number::New(env, WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL))

#define WEBGL1_CONSTANTS_SHADERS                                                                                         \
  InstanceValue("FRAGMENT_SHADER", Napi::Number::New(env, WEBGL_FRAGMENT_SHADER)),                                       \
      InstanceValue("VERTEX_SHADER", Napi::Number::New(env, WEBGL_VERTEX_SHADER)),                                       \
      InstanceValue("COMPILE_STATUS", Napi::Number::New(env, WEBGL_COMPILE_STATUS)),                                     \
      InstanceValue("DELETE_STATUS", Napi::Number::New(env, WEBGL_DELETE_STATUS)),                                       \
      InstanceValue("LINK_STATUS", Napi::Number::New(env, WEBGL_LINK_STATUS)),                                           \
      InstanceValue("VALIDATE_STATUS", Napi::Number::New(env, WEBGL_VALIDATE_STATUS)),                                   \
      InstanceValue("ATTACHED_SHADERS", Napi::Number::New(env, WEBGL_ATTACHED_SHADERS)),                                 \
      InstanceValue("ACTIVE_ATTRIBUTES", Napi::Number::New(env, WEBGL_ACTIVE_ATTRIBUTES)),                               \
      InstanceValue("ACTIVE_UNIFORMS", Napi::Number::New(env, WEBGL_ACTIVE_UNIFORMS)),                                   \
      InstanceValue("MAX_VERTEX_ATTRIBS", Napi::Number::New(env, WEBGL_MAX_VERTEX_ATTRIBS)),                             \
      InstanceValue("MAX_VERTEX_UNIFORM_VECTORS", Napi::Number::New(env, WEBGL_MAX_VERTEX_UNIFORM_VECTORS)),             \
      InstanceValue("MAX_VARYING_VECTORS", Napi::Number::New(env, WEBGL_MAX_VARYING_VECTORS)),                           \
      InstanceValue("MAX_COMBINED_TEXTURE_IMAGE_UNITS", Napi::Number::New(env, WEBGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)), \
      InstanceValue("MAX_VERTEX_TEXTURE_IMAGE_UNITS", Napi::Number::New(env, WEBGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS)),     \
      InstanceValue("MAX_TEXTURE_IMAGE_UNITS", Napi::Number::New(env, WEBGL_MAX_TEXTURE_IMAGE_UNITS)),                   \
      InstanceValue("MAX_FRAGMENT_UNIFORM_VECTORS", Napi::Number::New(env, WEBGL_MAX_FRAGMENT_UNIFORM_VECTORS)),         \
      InstanceValue("SHADER_TYPE", Napi::Number::New(env, WEBGL_SHADER_TYPE)),                                           \
      InstanceValue("SHADING_LANGUAGE_VERSION", Napi::Number::New(env, WEBGL_SHADING_LANGUAGE_VERSION)),                 \
      InstanceValue("CURRENT_PROGRAM", Napi::Number::New(env, WEBGL_CURRENT_PROGRAM)),                                   \
      InstanceValue("LOW_FLOAT", Napi::Number::New(env, WEBGL_LOW_FLOAT)),                                               \
      InstanceValue("MEDIUM_FLOAT", Napi::Number::New(env, WEBGL_MEDIUM_FLOAT)),                                         \
      InstanceValue("HIGH_FLOAT", Napi::Number::New(env, WEBGL_HIGH_FLOAT)),                                             \
      InstanceValue("LOW_INT", Napi::Number::New(env, WEBGL_LOW_INT)),                                                   \
      InstanceValue("MEDIUM_INT", Napi::Number::New(env, WEBGL_MEDIUM_INT)),                                             \
      InstanceValue("HIGH_INT", Napi::Number::New(env, WEBGL_HIGH_INT))

#define WEBGL1_CONSTANTS_DEPTH_AND_STENCIL                                 \
  InstanceValue("NEVER", Napi::Number::New(env, WEBGL_NEVER)),             \
      InstanceValue("LESS", Napi::Number::New(env, WEBGL_LESS)),           \
      InstanceValue("EQUAL", Napi::Number::New(env, WEBGL_EQUAL)),         \
      InstanceValue("LEQUAL", Napi::Number::New(env, WEBGL_LEQUAL)),       \
      InstanceValue("GREATER", Napi::Number::New(env, WEBGL_GREATER)),     \
      InstanceValue("NOTEQUAL", Napi::Number::New(env, WEBGL_NOTEQUAL)),   \
      InstanceValue("GEQUAL", Napi::Number::New(env, WEBGL_GEQUAL)),       \
      InstanceValue("ALWAYS", Napi::Number::New(env, WEBGL_ALWAYS)),       \
      InstanceValue("KEEP", Napi::Number::New(env, WEBGL_KEEP)),           \
      InstanceValue("REPLACE", Napi::Number::New(env, WEBGL_REPLACE)),     \
      InstanceValue("INCR", Napi::Number::New(env, WEBGL_INCR)),           \
      InstanceValue("DECR", Napi::Number::New(env, WEBGL_DECR)),           \
      InstanceValue("INVERT", Napi::Number::New(env, WEBGL_INVERT)),       \
      InstanceValue("INCR_WRAP", Napi::Number::New(env, WEBGL_INCR_WRAP)), \
      InstanceValue("DECR_WRAP", Napi::Number::New(env, WEBGL_DECR_WRAP))

#define WEBGL1_CONSTANTS_TEXTURE                                                                               \
  InstanceValue("NEAREST", Napi::Number::New(env, WEBGL_NEAREST)),                                             \
      InstanceValue("LINEAR", Napi::Number::New(env, WEBGL_LINEAR)),                                           \
      InstanceValue("NEAREST_MIPMAP_NEAREST", Napi::Number::New(env, WEBGL_NEAREST_MIPMAP_NEAREST)),           \
      InstanceValue("LINEAR_MIPMAP_NEAREST", Napi::Number::New(env, WEBGL_LINEAR_MIPMAP_NEAREST)),             \
      InstanceValue("NEAREST_MIPMAP_LINEAR", Napi::Number::New(env, WEBGL_NEAREST_MIPMAP_LINEAR)),             \
      InstanceValue("LINEAR_MIPMAP_LINEAR", Napi::Number::New(env, WEBGL_LINEAR_MIPMAP_LINEAR)),               \
      InstanceValue("TEXTURE_MAG_FILTER", Napi::Number::New(env, WEBGL_TEXTURE_MAG_FILTER)),                   \
      InstanceValue("TEXTURE_MIN_FILTER", Napi::Number::New(env, WEBGL_TEXTURE_MIN_FILTER)),                   \
      InstanceValue("TEXTURE_WRAP_S", Napi::Number::New(env, WEBGL_TEXTURE_WRAP_S)),                           \
      InstanceValue("TEXTURE_WRAP_T", Napi::Number::New(env, WEBGL_TEXTURE_WRAP_T)),                           \
      InstanceValue("TEXTURE_2D", Napi::Number::New(env, WEBGL_TEXTURE_2D)),                                   \
      InstanceValue("TEXTURE", Napi::Number::New(env, WEBGL_TEXTURE)),                                         \
      InstanceValue("TEXTURE_CUBE_MAP", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP)),                       \
      InstanceValue("TEXTURE_BINDING_CUBE_MAP", Napi::Number::New(env, WEBGL_TEXTURE_BINDING_CUBE_MAP)),       \
      InstanceValue("TEXTURE_CUBE_MAP_POSITIVE_X", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_POSITIVE_X)), \
      InstanceValue("TEXTURE_CUBE_MAP_NEGATIVE_X", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_X)), \
      InstanceValue("TEXTURE_CUBE_MAP_POSITIVE_Y", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Y)), \
      InstanceValue("TEXTURE_CUBE_MAP_NEGATIVE_Y", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Y)), \
      InstanceValue("TEXTURE_CUBE_MAP_POSITIVE_Z", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Z)), \
      InstanceValue("TEXTURE_CUBE_MAP_NEGATIVE_Z", Napi::Number::New(env, WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Z)), \
      InstanceValue("MAX_CUBE_MAP_TEXTURE_SIZE", Napi::Number::New(env, WEBGL_MAX_CUBE_MAP_TEXTURE_SIZE)),     \
      InstanceValue("TEXTURE0", Napi::Number::New(env, WEBGL_TEXTURE0)),                                       \
      InstanceValue("TEXTURE1", Napi::Number::New(env, WEBGL_TEXTURE1)),                                       \
      InstanceValue("TEXTURE2", Napi::Number::New(env, WEBGL_TEXTURE2)),                                       \
      InstanceValue("TEXTURE3", Napi::Number::New(env, WEBGL_TEXTURE3)),                                       \
      InstanceValue("TEXTURE4", Napi::Number::New(env, WEBGL_TEXTURE4)),                                       \
      InstanceValue("TEXTURE5", Napi::Number::New(env, WEBGL_TEXTURE5)),                                       \
      InstanceValue("TEXTURE6", Napi::Number::New(env, WEBGL_TEXTURE6)),                                       \
      InstanceValue("TEXTURE7", Napi::Number::New(env, WEBGL_TEXTURE7)),                                       \
      InstanceValue("TEXTURE8", Napi::Number::New(env, WEBGL_TEXTURE8)),                                       \
      InstanceValue("TEXTURE9", Napi::Number::New(env, WEBGL_TEXTURE9)),                                       \
      InstanceValue("TEXTURE10", Napi::Number::New(env, WEBGL_TEXTURE10)),                                     \
      InstanceValue("TEXTURE11", Napi::Number::New(env, WEBGL_TEXTURE11)),                                     \
      InstanceValue("TEXTURE12", Napi::Number::New(env, WEBGL_TEXTURE12)),                                     \
      InstanceValue("TEXTURE13", Napi::Number::New(env, WEBGL_TEXTURE13)),                                     \
      InstanceValue("TEXTURE14", Napi::Number::New(env, WEBGL_TEXTURE14)),                                     \
      InstanceValue("TEXTURE15", Napi::Number::New(env, WEBGL_TEXTURE15)),                                     \
      InstanceValue("TEXTURE16", Napi::Number::New(env, WEBGL_TEXTURE16)),                                     \
      InstanceValue("TEXTURE17", Napi::Number::New(env, WEBGL_TEXTURE17)),                                     \
      InstanceValue("TEXTURE18", Napi::Number::New(env, WEBGL_TEXTURE18)),                                     \
      InstanceValue("TEXTURE19", Napi::Number::New(env, WEBGL_TEXTURE19)),                                     \
      InstanceValue("TEXTURE20", Napi::Number::New(env, WEBGL_TEXTURE20)),                                     \
      InstanceValue("TEXTURE21", Napi::Number::New(env, WEBGL_TEXTURE21)),                                     \
      InstanceValue("TEXTURE22", Napi::Number::New(env, WEBGL_TEXTURE22)),                                     \
      InstanceValue("TEXTURE23", Napi::Number::New(env, WEBGL_TEXTURE23)),                                     \
      InstanceValue("TEXTURE24", Napi::Number::New(env, WEBGL_TEXTURE24)),                                     \
      InstanceValue("TEXTURE25", Napi::Number::New(env, WEBGL_TEXTURE25)),                                     \
      InstanceValue("TEXTURE26", Napi::Number::New(env, WEBGL_TEXTURE26)),                                     \
      InstanceValue("TEXTURE27", Napi::Number::New(env, WEBGL_TEXTURE27)),                                     \
      InstanceValue("TEXTURE28", Napi::Number::New(env, WEBGL_TEXTURE28)),                                     \
      InstanceValue("TEXTURE29", Napi::Number::New(env, WEBGL_TEXTURE29)),                                     \
      InstanceValue("TEXTURE30", Napi::Number::New(env, WEBGL_TEXTURE30)),                                     \
      InstanceValue("TEXTURE31", Napi::Number::New(env, WEBGL_TEXTURE31)),                                     \
      InstanceValue("ACTIVE_TEXTURE", Napi::Number::New(env, WEBGL_ACTIVE_TEXTURE)),                           \
      InstanceValue("REPEAT", Napi::Number::New(env, WEBGL_REPEAT)),                                           \
      InstanceValue("CLAMP_TO_EDGE", Napi::Number::New(env, WEBGL_CLAMP_TO_EDGE)),                             \
      InstanceValue("MIRRORED_REPEAT", Napi::Number::New(env, WEBGL_MIRRORED_REPEAT))

#define WEBGL1_CONSTANTS_UNIFORM_TYPES                                       \
  InstanceValue("FLOAT_VEC2", Napi::Number::New(env, WEBGL_FLOAT_VEC2)),     \
      InstanceValue("FLOAT_VEC3", Napi::Number::New(env, WEBGL_FLOAT_VEC3)), \
      InstanceValue("FLOAT_VEC4", Napi::Number::New(env, WEBGL_FLOAT_VEC4)), \
      InstanceValue("INT_VEC2", Napi::Number::New(env, WEBGL_INT_VEC2)),     \
      InstanceValue("INT_VEC3", Napi::Number::New(env, WEBGL_INT_VEC3)),     \
      InstanceValue("INT_VEC4", Napi::Number::New(env, WEBGL_INT_VEC4)),     \
      InstanceValue("BOOL", Napi::Number::New(env, WEBGL_BOOL)),             \
      InstanceValue("BOOL_VEC2", Napi::Number::New(env, WEBGL_BOOL_VEC2)),   \
      InstanceValue("BOOL_VEC3", Napi::Number::New(env, WEBGL_BOOL_VEC3)),   \
      InstanceValue("BOOL_VEC4", Napi::Number::New(env, WEBGL_BOOL_VEC4)),   \
      InstanceValue("FLOAT_MAT2", Napi::Number::New(env, WEBGL_FLOAT_MAT2)), \
      InstanceValue("FLOAT_MAT3", Napi::Number::New(env, WEBGL_FLOAT_MAT3)), \
      InstanceValue("FLOAT_MAT4", Napi::Number::New(env, WEBGL_FLOAT_MAT4)), \
      InstanceValue("SAMPLER_2D", Napi::Number::New(env, WEBGL_SAMPLER_2D)), \
      InstanceValue("SAMPLER_CUBE", Napi::Number::New(env, WEBGL_SAMPLER_CUBE))

#define WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER                                                                                            \
  InstanceValue("FRAMEBUFFER", Napi::Number::New(env, WEBGL_FRAMEBUFFER)),                                                                       \
      InstanceValue("RENDERBUFFER", Napi::Number::New(env, WEBGL_RENDERBUFFER)),                                                                 \
      InstanceValue("RGBA4", Napi::Number::New(env, WEBGL_RGBA4)),                                                                               \
      InstanceValue("RGB5_A1", Napi::Number::New(env, WEBGL_RGB5_A1)),                                                                           \
      InstanceValue("RGB565", Napi::Number::New(env, WEBGL_RGB565)),                                                                             \
      InstanceValue("DEPTH_COMPONENT16", Napi::Number::New(env, WEBGL_DEPTH_COMPONENT16)),                                                       \
      InstanceValue("STENCIL_INDEX8", Napi::Number::New(env, WEBGL_STENCIL_INDEX8)),                                                             \
      InstanceValue("DEPTH_STENCIL", Napi::Number::New(env, WEBGL_DEPTH_STENCIL)),                                                               \
      InstanceValue("RENDERBUFFER_WIDTH", Napi::Number::New(env, WEBGL_RENDERBUFFER_WIDTH)),                                                     \
      InstanceValue("RENDERBUFFER_HEIGHT", Napi::Number::New(env, WEBGL_RENDERBUFFER_HEIGHT)),                                                   \
      InstanceValue("RENDERBUFFER_INTERNAL_FORMAT", Napi::Number::New(env, WEBGL_RENDERBUFFER_INTERNAL_FORMAT)),                                 \
      InstanceValue("RENDERBUFFER_RED_SIZE", Napi::Number::New(env, WEBGL_RENDERBUFFER_RED_SIZE)),                                               \
      InstanceValue("RENDERBUFFER_GREEN_SIZE", Napi::Number::New(env, WEBGL_RENDERBUFFER_GREEN_SIZE)),                                           \
      InstanceValue("RENDERBUFFER_BLUE_SIZE", Napi::Number::New(env, WEBGL_RENDERBUFFER_BLUE_SIZE)),                                             \
      InstanceValue("RENDERBUFFER_ALPHA_SIZE", Napi::Number::New(env, WEBGL_RENDERBUFFER_ALPHA_SIZE)),                                           \
      InstanceValue("RENDERBUFFER_DEPTH_SIZE", Napi::Number::New(env, WEBGL_RENDERBUFFER_DEPTH_SIZE)),                                           \
      InstanceValue("RENDERBUFFER_STENCIL_SIZE", Napi::Number::New(env, WEBGL_RENDERBUFFER_STENCIL_SIZE)),                                       \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE", Napi::Number::New(env, WEBGL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE)),                     \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_OBJECT_NAME", Napi::Number::New(env, WEBGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME)),                     \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL", Napi::Number::New(env, WEBGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL)),                 \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE", Napi::Number::New(env, WEBGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE)), \
      InstanceValue("COLOR_ATTACHMENT0", Napi::Number::New(env, WEBGL_COLOR_ATTACHMENT0)),                                                       \
      InstanceValue("DEPTH_ATTACHMENT", Napi::Number::New(env, WEBGL_DEPTH_ATTACHMENT)),                                                         \
      InstanceValue("STENCIL_ATTACHMENT", Napi::Number::New(env, WEBGL_STENCIL_ATTACHMENT)),                                                     \
      InstanceValue("DEPTH_STENCIL_ATTACHMENT", Napi::Number::New(env, WEBGL_DEPTH_STENCIL_ATTACHMENT)),                                         \
      InstanceValue("NONE", Napi::Number::New(env, WEBGL_NONE)),                                                                                 \
      InstanceValue("FRAMEBUFFER_COMPLETE", Napi::Number::New(env, WEBGL_FRAMEBUFFER_COMPLETE)),                                                 \
      InstanceValue("FRAMEBUFFER_INCOMPLETE_ATTACHMENT", Napi::Number::New(env, WEBGL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)),                       \
      InstanceValue("FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", Napi::Number::New(env, WEBGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)),       \
      InstanceValue("FRAMEBUFFER_INCOMPLETE_DIMENSIONS", Napi::Number::New(env, WEBGL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)),                       \
      InstanceValue("FRAMEBUFFER_UNSUPPORTED", Napi::Number::New(env, WEBGL_FRAMEBUFFER_UNSUPPORTED)),                                           \
      InstanceValue("FRAMEBUFFER_BINDING", Napi::Number::New(env, WEBGL_FRAMEBUFFER_BINDING)),                                                   \
      InstanceValue("RENDERBUFFER_BINDING", Napi::Number::New(env, WEBGL_RENDERBUFFER_BINDING)),                                                 \
      InstanceValue("MAX_RENDERBUFFER_SIZE", Napi::Number::New(env, WEBGL_MAX_RENDERBUFFER_SIZE)),                                               \
      InstanceValue("INVALID_FRAMEBUFFER_OPERATION", Napi::Number::New(env, WEBGL_INVALID_FRAMEBUFFER_OPERATION))

#define WEBGL1_CONSTANTS                        \
  WEBGL1_CONSTANTS_CLEARING_BUFFERS,            \
      WEBGL1_CONSTANTS_RENDERING_PRIMITIVES,    \
      WEBGL1_CONSTANTS_BLENDING,                \
      WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS, \
      WEBGL1_CONSTANTS_CULLING,                 \
      WEBGL1_CONSTANTS_ENABLE_AND_DISABLE,      \
      WEBGL1_CONSTANTS_ERRORS,                  \
      WEBGL1_CONSTANTS_FRONTFACE,               \
      WEBGL1_CONSTANTS_HINTS,                   \
      WEBGL1_CONSTANTS_DATATYPES,               \
      WEBGL1_CONSTANTS_PIXELS,                  \
      WEBGL1_CONSTANTS_SHADERS,                 \
      WEBGL1_CONSTANTS_DEPTH_AND_STENCIL,       \
      WEBGL1_CONSTANTS_TEXTURE,                 \
      WEBGL1_CONSTANTS_UNIFORM_TYPES,           \
      WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER

#define WEBGL2_CONSTANTS_PARAMETERS                                                                                     \
  InstanceValue("READ_BUFFER", Napi::Number::New(env, WEBGL2_READ_BUFFER)),                                             \
      InstanceValue("UNPACK_ROW_LENGTH", Napi::Number::New(env, WEBGL2_UNPACK_ROW_LENGTH)),                             \
      InstanceValue("UNPACK_SKIP_ROWS", Napi::Number::New(env, WEBGL2_UNPACK_SKIP_ROWS)),                               \
      InstanceValue("UNPACK_SKIP_PIXELS", Napi::Number::New(env, WEBGL2_UNPACK_SKIP_PIXELS)),                           \
      InstanceValue("PACK_ROW_LENGTH", Napi::Number::New(env, WEBGL2_PACK_ROW_LENGTH)),                                 \
      InstanceValue("PACK_SKIP_ROWS", Napi::Number::New(env, WEBGL2_PACK_SKIP_ROWS)),                                   \
      InstanceValue("PACK_SKIP_PIXELS", Napi::Number::New(env, WEBGL2_PACK_SKIP_PIXELS)),                               \
      InstanceValue("TEXTURE_BINDING_3D", Napi::Number::New(env, WEBGL2_TEXTURE_BINDING_3D)),                           \
      InstanceValue("UNPACK_SKIP_IMAGES", Napi::Number::New(env, WEBGL2_UNPACK_SKIP_IMAGES)),                           \
      InstanceValue("UNPACK_IMAGE_HEIGHT", Napi::Number::New(env, WEBGL2_UNPACK_IMAGE_HEIGHT)),                         \
      InstanceValue("MAX_3D_TEXTURE_SIZE", Napi::Number::New(env, WEBGL2_MAX_3D_TEXTURE_SIZE)),                         \
      InstanceValue("MAX_ELEMENTS_VERTICES", Napi::Number::New(env, WEBGL2_MAX_ELEMENTS_VERTICES)),                     \
      InstanceValue("MAX_ELEMENTS_INDICES", Napi::Number::New(env, WEBGL2_MAX_ELEMENTS_INDICES)),                       \
      InstanceValue("MAX_TEXTURE_LOD_BIAS", Napi::Number::New(env, WEBGL2_MAX_TEXTURE_LOD_BIAS)),                       \
      InstanceValue("MAX_FRAGMENT_UNIFORM_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_FRAGMENT_UNIFORM_COMPONENTS)), \
      InstanceValue("MAX_VERTEX_UNIFORM_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_VERTEX_UNIFORM_COMPONENTS)),     \
      InstanceValue("MAX_ARRAY_TEXTURE_LAYERS", Napi::Number::New(env, WEBGL2_MAX_ARRAY_TEXTURE_LAYERS)),               \
      InstanceValue("MIN_PROGRAM_TEXEL_OFFSET", Napi::Number::New(env, WEBGL2_MIN_PROGRAM_TEXEL_OFFSET)),               \
      InstanceValue("MAX_PROGRAM_TEXEL_OFFSET", Napi::Number::New(env, WEBGL2_MAX_PROGRAM_TEXEL_OFFSET)),               \
      InstanceValue("MAX_VARYING_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_VARYING_COMPONENTS)),                   \
      InstanceValue("FRAGMENT_SHADER_DERIVATIVE_HINT", Napi::Number::New(env, WEBGL2_FRAGMENT_SHADER_DERIVATIVE_HINT)), \
      InstanceValue("RASTERIZER_DISCARD", Napi::Number::New(env, WEBGL2_RASTERIZER_DISCARD)),                           \
      InstanceValue("VERTEX_ARRAY_BINDING", Napi::Number::New(env, WEBGL2_VERTEX_ARRAY_BINDING)),                       \
      InstanceValue("MAX_VERTEX_OUTPUT_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_VERTEX_OUTPUT_COMPONENTS)),       \
      InstanceValue("MAX_FRAGMENT_INPUT_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_FRAGMENT_INPUT_COMPONENTS)),     \
      InstanceValue("MAX_SERVER_WAIT_TIMEOUT", Napi::Number::New(env, WEBGL2_MAX_SERVER_WAIT_TIMEOUT)),                 \
      InstanceValue("MAX_ELEMENT_INDEX", Napi::Number::New(env, WEBGL2_MAX_ELEMENT_INDEX))

#define WEBGL2_CONSTANTS_TEXTURES                                                                         \
  InstanceValue("RED", Napi::Number::New(env, WEBGL2_RED)),                                               \
      InstanceValue("RGB8", Napi::Number::New(env, WEBGL2_RGB8)),                                         \
      InstanceValue("RGBA8", Napi::Number::New(env, WEBGL2_RGBA8)),                                       \
      InstanceValue("RGB10_A2", Napi::Number::New(env, WEBGL2_RGB10_A2)),                                 \
      InstanceValue("TEXTURE_3D", Napi::Number::New(env, WEBGL2_TEXTURE_3D)),                             \
      InstanceValue("TEXTURE_WRAP_R", Napi::Number::New(env, WEBGL2_TEXTURE_WRAP_R)),                     \
      InstanceValue("TEXTURE_MIN_LOD", Napi::Number::New(env, WEBGL2_TEXTURE_MIN_LOD)),                   \
      InstanceValue("TEXTURE_MAX_LOD", Napi::Number::New(env, WEBGL2_TEXTURE_MAX_LOD)),                   \
      InstanceValue("TEXTURE_BASE_LEVEL", Napi::Number::New(env, WEBGL2_TEXTURE_BASE_LEVEL)),             \
      InstanceValue("TEXTURE_MAX_LEVEL", Napi::Number::New(env, WEBGL2_TEXTURE_MAX_LEVEL)),               \
      InstanceValue("TEXTURE_COMPARE_MODE", Napi::Number::New(env, WEBGL2_TEXTURE_COMPARE_MODE)),         \
      InstanceValue("TEXTURE_COMPARE_FUNC", Napi::Number::New(env, WEBGL2_TEXTURE_COMPARE_FUNC)),         \
      InstanceValue("SRGB", Napi::Number::New(env, WEBGL2_SRGB)),                                         \
      InstanceValue("SRGB8", Napi::Number::New(env, WEBGL2_SRGB8)),                                       \
      InstanceValue("SRGB8_ALPHA8", Napi::Number::New(env, WEBGL2_SRGB8_ALPHA8)),                         \
      InstanceValue("COMPARE_REF_TO_TEXTURE", Napi::Number::New(env, WEBGL2_COMPARE_REF_TO_TEXTURE)),     \
      InstanceValue("RGBA32F", Napi::Number::New(env, WEBGL2_RGBA32F)),                                   \
      InstanceValue("RGB32F", Napi::Number::New(env, WEBGL2_RGB32F)),                                     \
      InstanceValue("RGBA16F", Napi::Number::New(env, WEBGL2_RGBA16F)),                                   \
      InstanceValue("RGB16F", Napi::Number::New(env, WEBGL2_RGB16F)),                                     \
      InstanceValue("TEXTURE_2D_ARRAY", Napi::Number::New(env, WEBGL2_TEXTURE_2D_ARRAY)),                 \
      InstanceValue("TEXTURE_BINDING_2D_ARRAY", Napi::Number::New(env, WEBGL2_TEXTURE_BINDING_2D_ARRAY)), \
      InstanceValue("R11F_G11F_B10F", Napi::Number::New(env, WEBGL2_R11F_G11F_B10F)),                     \
      InstanceValue("RGB9_E5", Napi::Number::New(env, WEBGL2_RGB9_E5)),                                   \
      InstanceValue("RGBA32UI", Napi::Number::New(env, WEBGL2_RGBA32UI)),                                 \
      InstanceValue("RGB32UI", Napi::Number::New(env, WEBGL2_RGB32UI)),                                   \
      InstanceValue("RGBA16UI", Napi::Number::New(env, WEBGL2_RGBA16UI)),                                 \
      InstanceValue("RGB16UI", Napi::Number::New(env, WEBGL2_RGB16UI)),                                   \
      InstanceValue("RGBA8UI", Napi::Number::New(env, WEBGL2_RGBA8UI)),                                   \
      InstanceValue("RGB8UI", Napi::Number::New(env, WEBGL2_RGB8UI)),                                     \
      InstanceValue("RGBA32I", Napi::Number::New(env, WEBGL2_RGBA32I)),                                   \
      InstanceValue("RGB32I", Napi::Number::New(env, WEBGL2_RGB32I)),                                     \
      InstanceValue("RGBA16I", Napi::Number::New(env, WEBGL2_RGBA16I)),                                   \
      InstanceValue("RGB16I", Napi::Number::New(env, WEBGL2_RGB16I)),                                     \
      InstanceValue("RGBA8I", Napi::Number::New(env, WEBGL2_RGBA8I)),                                     \
      InstanceValue("RGB8I", Napi::Number::New(env, WEBGL2_RGB8I)),                                       \
      InstanceValue("RED_INTEGER", Napi::Number::New(env, WEBGL2_RED_INTEGER)),                           \
      InstanceValue("RGB_INTEGER", Napi::Number::New(env, WEBGL2_RGB_INTEGER)),                           \
      InstanceValue("RGBA_INTEGER", Napi::Number::New(env, WEBGL2_RGBA_INTEGER)),                         \
      InstanceValue("R8", Napi::Number::New(env, WEBGL2_R8)),                                             \
      InstanceValue("RG8", Napi::Number::New(env, WEBGL2_RG8)),                                           \
      InstanceValue("R16F", Napi::Number::New(env, WEBGL2_R16F)),                                         \
      InstanceValue("RG16F", Napi::Number::New(env, WEBGL2_RG16F)),                                       \
      InstanceValue("R32F", Napi::Number::New(env, WEBGL2_R32F)),                                         \
      InstanceValue("RG32F", Napi::Number::New(env, WEBGL2_RG32F)),                                       \
      InstanceValue("R8I", Napi::Number::New(env, WEBGL2_R8I)),                                           \
      InstanceValue("R8UI", Napi::Number::New(env, WEBGL2_R8UI)),                                         \
      InstanceValue("R16I", Napi::Number::New(env, WEBGL2_R16I)),                                         \
      InstanceValue("R16UI", Napi::Number::New(env, WEBGL2_R16UI)),                                       \
      InstanceValue("R32I", Napi::Number::New(env, WEBGL2_R32I)),                                         \
      InstanceValue("R32UI", Napi::Number::New(env, WEBGL2_R32UI)),                                       \
      InstanceValue("RG8I", Napi::Number::New(env, WEBGL2_RG8I)),                                         \
      InstanceValue("RG8UI", Napi::Number::New(env, WEBGL2_RG8UI)),                                       \
      InstanceValue("RG16I", Napi::Number::New(env, WEBGL2_RG16I)),                                       \
      InstanceValue("RG16UI", Napi::Number::New(env, WEBGL2_RG16UI)),                                     \
      InstanceValue("RG32I", Napi::Number::New(env, WEBGL2_RG32I)),                                       \
      InstanceValue("RG32UI", Napi::Number::New(env, WEBGL2_RG32UI)),                                     \
      InstanceValue("R8_SNORM", Napi::Number::New(env, WEBGL2_R8_SNORM)),                                 \
      InstanceValue("RG8_SNORM", Napi::Number::New(env, WEBGL2_RG8_SNORM)),                               \
      InstanceValue("RGB8_SNORM", Napi::Number::New(env, WEBGL2_RGB8_SNORM)),                             \
      InstanceValue("RGBA8_SNORM", Napi::Number::New(env, WEBGL2_RGBA8_SNORM)),                           \
      InstanceValue("RGB10_A2UI", Napi::Number::New(env, WEBGL2_RGB10_A2UI)),                             \
      InstanceValue("TEXTURE_IMMUTABLE_FORMAT", Napi::Number::New(env, WEBGL2_TEXTURE_IMMUTABLE_FORMAT)), \
      InstanceValue("TEXTURE_IMMUTABLE_LEVELS", Napi::Number::New(env, WEBGL2_TEXTURE_IMMUTABLE_LEVELS))

#define WEBGL2_CONSTANTS_PIXELTYPES                                                                                   \
  InstanceValue("UNSIGNED_INT_2_10_10_10_REV", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_2_10_10_10_REV)),           \
      InstanceValue("UNSIGNED_INT_10F_11F_11F_REV", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_10F_11F_11F_REV)),     \
      InstanceValue("UNSIGNED_INT_5_9_9_9_REV", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_5_9_9_9_REV)),             \
      InstanceValue("FLOAT_32_UNSIGNED_INT_24_8_REV", Napi::Number::New(env, WEBGL2_FLOAT_32_UNSIGNED_INT_24_8_REV)), \
      InstanceValue("UNSIGNED_INT_24_8", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_24_8)),                           \
      InstanceValue("HALF_FLOAT", Napi::Number::New(env, WEBGL2_HALF_FLOAT)),                                         \
      InstanceValue("RG", Napi::Number::New(env, WEBGL2_RG)),                                                         \
      InstanceValue("RG_INTEGER", Napi::Number::New(env, WEBGL2_RG_INTEGER)),                                         \
      InstanceValue("INT_2_10_10_10_REV", Napi::Number::New(env, WEBGL2_INT_2_10_10_10_REV))

#define WEBGL2_CONSTANTS_QUERIES                                                                      \
  InstanceValue("CURRENT_QUERY", Napi::Number::New(env, WEBGL2_CURRENT_QUERY)),                       \
      InstanceValue("QUERY_RESULT", Napi::Number::New(env, WEBGL2_QUERY_RESULT)),                     \
      InstanceValue("QUERY_RESULT_AVAILABLE", Napi::Number::New(env, WEBGL2_QUERY_RESULT_AVAILABLE)), \
      InstanceValue("ANY_SAMPLES_PASSED", Napi::Number::New(env, WEBGL2_ANY_SAMPLES_PASSED)),         \
      InstanceValue("ANY_SAMPLES_PASSED_CONSERVATIVE", Napi::Number::New(env, WEBGL2_ANY_SAMPLES_PASSED_CONSERVATIVE))

#define WEBGL2_CONSTANTS_DRAWBUFFERS                                                                \
  InstanceValue("MAX_DRAW_BUFFERS", Napi::Number::New(env, WEBGL2_MAX_DRAW_BUFFERS)),               \
      InstanceValue("DRAW_BUFFER0", Napi::Number::New(env, WEBGL2_DRAW_BUFFER0)),                   \
      InstanceValue("DRAW_BUFFER1", Napi::Number::New(env, WEBGL2_DRAW_BUFFER1)),                   \
      InstanceValue("DRAW_BUFFER2", Napi::Number::New(env, WEBGL2_DRAW_BUFFER2)),                   \
      InstanceValue("DRAW_BUFFER3", Napi::Number::New(env, WEBGL2_DRAW_BUFFER3)),                   \
      InstanceValue("DRAW_BUFFER4", Napi::Number::New(env, WEBGL2_DRAW_BUFFER4)),                   \
      InstanceValue("DRAW_BUFFER5", Napi::Number::New(env, WEBGL2_DRAW_BUFFER5)),                   \
      InstanceValue("DRAW_BUFFER6", Napi::Number::New(env, WEBGL2_DRAW_BUFFER6)),                   \
      InstanceValue("DRAW_BUFFER7", Napi::Number::New(env, WEBGL2_DRAW_BUFFER7)),                   \
      InstanceValue("DRAW_BUFFER8", Napi::Number::New(env, WEBGL2_DRAW_BUFFER8)),                   \
      InstanceValue("DRAW_BUFFER9", Napi::Number::New(env, WEBGL2_DRAW_BUFFER9)),                   \
      InstanceValue("DRAW_BUFFER10", Napi::Number::New(env, WEBGL2_DRAW_BUFFER10)),                 \
      InstanceValue("DRAW_BUFFER11", Napi::Number::New(env, WEBGL2_DRAW_BUFFER11)),                 \
      InstanceValue("DRAW_BUFFER12", Napi::Number::New(env, WEBGL2_DRAW_BUFFER12)),                 \
      InstanceValue("DRAW_BUFFER13", Napi::Number::New(env, WEBGL2_DRAW_BUFFER13)),                 \
      InstanceValue("DRAW_BUFFER14", Napi::Number::New(env, WEBGL2_DRAW_BUFFER14)),                 \
      InstanceValue("DRAW_BUFFER15", Napi::Number::New(env, WEBGL2_DRAW_BUFFER15)),                 \
      InstanceValue("MAX_COLOR_ATTACHMENTS", Napi::Number::New(env, WEBGL2_MAX_COLOR_ATTACHMENTS)), \
      InstanceValue("COLOR_ATTACHMENT1", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT1)),         \
      InstanceValue("COLOR_ATTACHMENT2", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT2)),         \
      InstanceValue("COLOR_ATTACHMENT3", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT3)),         \
      InstanceValue("COLOR_ATTACHMENT4", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT4)),         \
      InstanceValue("COLOR_ATTACHMENT5", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT5)),         \
      InstanceValue("COLOR_ATTACHMENT6", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT6)),         \
      InstanceValue("COLOR_ATTACHMENT7", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT7)),         \
      InstanceValue("COLOR_ATTACHMENT8", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT8)),         \
      InstanceValue("COLOR_ATTACHMENT9", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT9)),         \
      InstanceValue("COLOR_ATTACHMENT10", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT10)),       \
      InstanceValue("COLOR_ATTACHMENT11", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT11)),       \
      InstanceValue("COLOR_ATTACHMENT12", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT12)),       \
      InstanceValue("COLOR_ATTACHMENT13", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT13)),       \
      InstanceValue("COLOR_ATTACHMENT14", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT14)),       \
      InstanceValue("COLOR_ATTACHMENT15", Napi::Number::New(env, WEBGL2_COLOR_ATTACHMENT15))

#define WEBGL2_CONSTANTS_SAMPLERS                                                                                   \
  InstanceValue("SAMPLER_3D", Napi::Number::New(env, WEBGL2_SAMPLER_3D)),                                           \
      InstanceValue("SAMPLER_2D_SHADOW", Napi::Number::New(env, WEBGL2_SAMPLER_2D_SHADOW)),                         \
      InstanceValue("SAMPLER_2D_ARRAY", Napi::Number::New(env, WEBGL2_SAMPLER_2D_ARRAY)),                           \
      InstanceValue("SAMPLER_2D_ARRAY_SHADOW", Napi::Number::New(env, WEBGL2_SAMPLER_2D_ARRAY_SHADOW)),             \
      InstanceValue("SAMPLER_CUBE_SHADOW", Napi::Number::New(env, WEBGL2_SAMPLER_CUBE_SHADOW)),                     \
      InstanceValue("INT_SAMPLER_2D", Napi::Number::New(env, WEBGL2_INT_SAMPLER_2D)),                               \
      InstanceValue("INT_SAMPLER_3D", Napi::Number::New(env, WEBGL2_INT_SAMPLER_3D)),                               \
      InstanceValue("INT_SAMPLER_CUBE", Napi::Number::New(env, WEBGL2_INT_SAMPLER_CUBE)),                           \
      InstanceValue("INT_SAMPLER_2D_ARRAY", Napi::Number::New(env, WEBGL2_INT_SAMPLER_2D_ARRAY)),                   \
      InstanceValue("UNSIGNED_INT_SAMPLER_2D", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_SAMPLER_2D)),             \
      InstanceValue("UNSIGNED_INT_SAMPLER_3D", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_SAMPLER_3D)),             \
      InstanceValue("UNSIGNED_INT_SAMPLER_CUBE", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_SAMPLER_CUBE)),         \
      InstanceValue("UNSIGNED_INT_SAMPLER_2D_ARRAY", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_SAMPLER_2D_ARRAY)), \
      InstanceValue("MAX_SAMPLES", Napi::Number::New(env, WEBGL2_MAX_SAMPLES)),                                     \
      InstanceValue("SAMPLER_BINDING", Napi::Number::New(env, WEBGL2_SAMPLER_BINDING))

#define WEBGL2_CONSTANTS_BUFFERS                                                                                \
  InstanceValue("PIXEL_PACK_BUFFER", Napi::Number::New(env, WEBGL2_PIXEL_PACK_BUFFER)),                         \
      InstanceValue("PIXEL_UNPACK_BUFFER", Napi::Number::New(env, WEBGL2_PIXEL_UNPACK_BUFFER)),                 \
      InstanceValue("PIXEL_PACK_BUFFER_BINDING", Napi::Number::New(env, WEBGL2_PIXEL_PACK_BUFFER_BINDING)),     \
      InstanceValue("PIXEL_UNPACK_BUFFER_BINDING", Napi::Number::New(env, WEBGL2_PIXEL_UNPACK_BUFFER_BINDING)), \
      InstanceValue("COPY_READ_BUFFER", Napi::Number::New(env, WEBGL2_COPY_READ_BUFFER)),                       \
      InstanceValue("COPY_WRITE_BUFFER", Napi::Number::New(env, WEBGL2_COPY_WRITE_BUFFER)),                     \
      InstanceValue("COPY_READ_BUFFER_BINDING", Napi::Number::New(env, WEBGL2_COPY_READ_BUFFER_BINDING)),       \
      InstanceValue("COPY_WRITE_BUFFER_BINDING", Napi::Number::New(env, WEBGL2_COPY_WRITE_BUFFER_BINDING))

#define WEBGL2_CONSTANTS_DATA_TYPES                                                             \
  InstanceValue("FLOAT_MAT2x3", Napi::Number::New(env, WEBGL2_FLOAT_MAT2x3)),                   \
      InstanceValue("FLOAT_MAT2x4", Napi::Number::New(env, WEBGL2_FLOAT_MAT2x4)),               \
      InstanceValue("FLOAT_MAT3x2", Napi::Number::New(env, WEBGL2_FLOAT_MAT3x2)),               \
      InstanceValue("FLOAT_MAT3x4", Napi::Number::New(env, WEBGL2_FLOAT_MAT3x4)),               \
      InstanceValue("FLOAT_MAT4x2", Napi::Number::New(env, WEBGL2_FLOAT_MAT4x2)),               \
      InstanceValue("FLOAT_MAT4x3", Napi::Number::New(env, WEBGL2_FLOAT_MAT4x3)),               \
      InstanceValue("UNSIGNED_INT_VEC2", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_VEC2)),     \
      InstanceValue("UNSIGNED_INT_VEC3", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_VEC3)),     \
      InstanceValue("UNSIGNED_INT_VEC4", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_VEC4)),     \
      InstanceValue("UNSIGNED_NORMALIZED", Napi::Number::New(env, WEBGL2_UNSIGNED_NORMALIZED)), \
      InstanceValue("SIGNED_NORMALIZED", Napi::Number::New(env, WEBGL2_SIGNED_NORMALIZED))

#define WEBGL2_CONSTANTS_VERTEX_ATTRIBS                                                                     \
  InstanceValue("VERTEX_ATTRIB_ARRAY_INTEGER", Napi::Number::New(env, WEBGL2_VERTEX_ATTRIB_ARRAY_INTEGER)), \
      InstanceValue("VERTEX_ATTRIB_ARRAY_DIVISOR", Napi::Number::New(env, WEBGL2_VERTEX_ATTRIB_ARRAY_DIVISOR))

#define WEBGL2_CONSTANTS_TRANSFORM_FEEDBACK                                                                                                         \
  InstanceValue("TRANSFORM_FEEDBACK_BUFFER_MODE", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_BUFFER_MODE)),                                   \
      InstanceValue("MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS)),       \
      InstanceValue("TRANSFORM_FEEDBACK_VARYINGS", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_VARYINGS)),                                     \
      InstanceValue("TRANSFORM_FEEDBACK_BUFFER_START", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_BUFFER_START)),                             \
      InstanceValue("TRANSFORM_FEEDBACK_BUFFER_SIZE", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_BUFFER_SIZE)),                               \
      InstanceValue("TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)),                 \
      InstanceValue("MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS)), \
      InstanceValue("MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS", Napi::Number::New(env, WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS)),             \
      InstanceValue("INTERLEAVED_ATTRIBS", Napi::Number::New(env, WEBGL2_INTERLEAVED_ATTRIBS)),                                                     \
      InstanceValue("SEPARATE_ATTRIBS", Napi::Number::New(env, WEBGL2_SEPARATE_ATTRIBS)),                                                           \
      InstanceValue("TRANSFORM_FEEDBACK_BUFFER", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_BUFFER)),                                         \
      InstanceValue("TRANSFORM_FEEDBACK_BUFFER_BINDING", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_BUFFER_BINDING)),                         \
      InstanceValue("TRANSFORM_FEEDBACK", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK)),                                                       \
      InstanceValue("TRANSFORM_FEEDBACK_PAUSED", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_PAUSED)),                                         \
      InstanceValue("TRANSFORM_FEEDBACK_ACTIVE", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_ACTIVE)),                                         \
      InstanceValue("TRANSFORM_FEEDBACK_BINDING", Napi::Number::New(env, WEBGL2_TRANSFORM_FEEDBACK_BINDING))

#define WEBGL2_CONSTANTS_FRAMEBUFFERS_AND_RENDERBUFFERS                                                                             \
  InstanceValue("FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING)),     \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE)), \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_RED_SIZE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_RED_SIZE)),             \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_GREEN_SIZE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE)),         \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_BLUE_SIZE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE)),           \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE)),         \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE)),         \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE)),     \
      InstanceValue("FRAMEBUFFER_DEFAULT", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_DEFAULT)),                                     \
      InstanceValue("DEPTH_STENCIL_ATTACHMENT", Napi::Number::New(env, WEBGL2_DEPTH_STENCIL_ATTACHMENT)),                           \
      InstanceValue("DEPTH_STENCIL", Napi::Number::New(env, WEBGL2_DEPTH_STENCIL)),                                                 \
      InstanceValue("DEPTH24_STENCIL8", Napi::Number::New(env, WEBGL2_UNSIGNED_INT_24_8)),                                          \
      InstanceValue("DRAW_FRAMEBUFFER_BINDING", Napi::Number::New(env, WEBGL2_DRAW_FRAMEBUFFER_BINDING)),                           \
      InstanceValue("READ_FRAMEBUFFER", Napi::Number::New(env, WEBGL2_READ_FRAMEBUFFER)),                                           \
      InstanceValue("DRAW_FRAMEBUFFER", Napi::Number::New(env, WEBGL2_DRAW_FRAMEBUFFER)),                                           \
      InstanceValue("READ_FRAMEBUFFER_BINDING", Napi::Number::New(env, WEBGL2_READ_FRAMEBUFFER_BINDING)),                           \
      InstanceValue("RENDERBUFFER_SAMPLES", Napi::Number::New(env, WEBGL2_RENDERBUFFER_SAMPLES)),                                   \
      InstanceValue("FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER)),   \
      InstanceValue("FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", Napi::Number::New(env, WEBGL2_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE))

#define WEBGL2_CONSTANTS_UNIFORMS                                                                                                           \
  InstanceValue("UNIFORM_BUFFER", Napi::Number::New(env, WEBGL2_UNIFORM_BUFFER)),                                                           \
      InstanceValue("UNIFORM_BUFFER_BINDING", Napi::Number::New(env, WEBGL2_UNIFORM_BUFFER_BINDING)),                                       \
      InstanceValue("UNIFORM_BUFFER_START", Napi::Number::New(env, WEBGL2_UNIFORM_BUFFER_START)),                                           \
      InstanceValue("UNIFORM_BUFFER_SIZE", Napi::Number::New(env, WEBGL2_UNIFORM_BUFFER_SIZE)),                                             \
      InstanceValue("MAX_VERTEX_UNIFORM_BLOCKS", Napi::Number::New(env, WEBGL2_MAX_VERTEX_UNIFORM_BLOCKS)),                                 \
      InstanceValue("MAX_FRAGMENT_UNIFORM_BLOCKS", Napi::Number::New(env, WEBGL2_MAX_FRAGMENT_UNIFORM_BLOCKS)),                             \
      InstanceValue("MAX_COMBINED_UNIFORM_BLOCKS", Napi::Number::New(env, WEBGL2_MAX_COMBINED_UNIFORM_BLOCKS)),                             \
      InstanceValue("MAX_UNIFORM_BUFFER_BINDINGS", Napi::Number::New(env, WEBGL2_MAX_UNIFORM_BUFFER_BINDINGS)),                             \
      InstanceValue("MAX_UNIFORM_BLOCK_SIZE", Napi::Number::New(env, WEBGL2_MAX_UNIFORM_BLOCK_SIZE)),                                       \
      InstanceValue("MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS)),       \
      InstanceValue("MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", Napi::Number::New(env, WEBGL2_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS)),   \
      InstanceValue("UNIFORM_BUFFER_OFFSET_ALIGNMENT", Napi::Number::New(env, WEBGL2_UNIFORM_BUFFER_OFFSET_ALIGNMENT)),                     \
      InstanceValue("ACTIVE_UNIFORM_BLOCKS", Napi::Number::New(env, WEBGL2_ACTIVE_UNIFORM_BLOCKS)),                                         \
      InstanceValue("UNIFORM_TYPE", Napi::Number::New(env, WEBGL2_UNIFORM_TYPE)),                                                           \
      InstanceValue("UNIFORM_SIZE", Napi::Number::New(env, WEBGL2_UNIFORM_SIZE)),                                                           \
      InstanceValue("UNIFORM_BLOCK_INDEX", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_INDEX)),                                             \
      InstanceValue("UNIFORM_OFFSET", Napi::Number::New(env, WEBGL2_UNIFORM_OFFSET)),                                                       \
      InstanceValue("UNIFORM_ARRAY_STRIDE", Napi::Number::New(env, WEBGL2_UNIFORM_ARRAY_STRIDE)),                                           \
      InstanceValue("UNIFORM_MATRIX_STRIDE", Napi::Number::New(env, WEBGL2_UNIFORM_MATRIX_STRIDE)),                                         \
      InstanceValue("UNIFORM_IS_ROW_MAJOR", Napi::Number::New(env, WEBGL2_UNIFORM_IS_ROW_MAJOR)),                                           \
      InstanceValue("UNIFORM_BLOCK_BINDING", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_BINDING)),                                         \
      InstanceValue("UNIFORM_BLOCK_DATA_SIZE", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_DATA_SIZE)),                                     \
      InstanceValue("UNIFORM_BLOCK_ACTIVE_UNIFORMS", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_ACTIVE_UNIFORMS)),                         \
      InstanceValue("UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES)),           \
      InstanceValue("UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER)), \
      InstanceValue("UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER", Napi::Number::New(env, WEBGL2_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER))

#define WEBGL2_CONSTANTS_SYNC_OBJECTS                                                                         \
  InstanceValue("OBJECT_TYPE", Napi::Number::New(env, WEBGL2_OBJECT_TYPE)),                                   \
      InstanceValue("SYNC_CONDITION", Napi::Number::New(env, WEBGL2_SYNC_CONDITION)),                         \
      InstanceValue("SYNC_STATUS", Napi::Number::New(env, WEBGL2_SYNC_STATUS)),                               \
      InstanceValue("SYNC_FLAGS", Napi::Number::New(env, WEBGL2_SYNC_FLAGS)),                                 \
      InstanceValue("SYNC_FENCE", Napi::Number::New(env, WEBGL2_SYNC_FENCE)),                                 \
      InstanceValue("SYNC_GPU_COMMANDS_COMPLETE", Napi::Number::New(env, WEBGL2_SYNC_GPU_COMMANDS_COMPLETE)), \
      InstanceValue("UNSIGNALED", Napi::Number::New(env, WEBGL2_UNSIGNALED)),                                 \
      InstanceValue("SIGNALED", Napi::Number::New(env, WEBGL2_SIGNALED)),                                     \
      InstanceValue("ALREADY_SIGNALED", Napi::Number::New(env, WEBGL2_ALREADY_SIGNALED)),                     \
      InstanceValue("TIMEOUT_EXPIRED", Napi::Number::New(env, WEBGL2_TIMEOUT_EXPIRED)),                       \
      InstanceValue("CONDITION_SATISFIED", Napi::Number::New(env, WEBGL2_CONDITION_SATISFIED)),               \
      InstanceValue("WAIT_FAILED", Napi::Number::New(env, WEBGL2_WAIT_FAILED)),                               \
      InstanceValue("SYNC_FLUSH_COMMANDS_BIT", Napi::Number::New(env, WEBGL2_SYNC_FLUSH_COMMANDS_BIT))

#define WEBGL2_CONSTANTS_MISCELLANEOUS                                                        \
  InstanceValue("COLOR", Napi::Number::New(env, WEBGL2_COLOR)),                               \
      InstanceValue("DEPTH", Napi::Number::New(env, WEBGL2_DEPTH)),                           \
      InstanceValue("STENCIL", Napi::Number::New(env, WEBGL2_STENCIL)),                       \
      InstanceValue("MIN", Napi::Number::New(env, WEBGL2_MIN)),                               \
      InstanceValue("MAX", Napi::Number::New(env, WEBGL2_MAX)),                               \
      InstanceValue("DEPTH_COMPONENT24", Napi::Number::New(env, WEBGL2_DEPTH_COMPONENT24)),   \
      InstanceValue("STREAM_READ", Napi::Number::New(env, WEBGL2_STREAM_READ)),               \
      InstanceValue("STREAM_COPY", Napi::Number::New(env, WEBGL2_STREAM_COPY)),               \
      InstanceValue("STATIC_READ", Napi::Number::New(env, WEBGL2_STATIC_READ)),               \
      InstanceValue("STATIC_COPY", Napi::Number::New(env, WEBGL2_STATIC_COPY)),               \
      InstanceValue("DYNAMIC_READ", Napi::Number::New(env, WEBGL2_DYNAMIC_READ)),             \
      InstanceValue("DYNAMIC_COPY", Napi::Number::New(env, WEBGL2_DYNAMIC_COPY)),             \
      InstanceValue("DEPTH_COMPONENT32F", Napi::Number::New(env, WEBGL2_DEPTH_COMPONENT32F)), \
      InstanceValue("DEPTH32F_STENCIL8", Napi::Number::New(env, WEBGL2_DEPTH32F_STENCIL8)),   \
      InstanceValue("INVALID_INDEX", Napi::Number::New(env, WEBGL2_INVALID_INDEX)),           \
      InstanceValue("TIMEOUT_IGNORED", Napi::Number::New(env, WEBGL2_TIMEOUT_IGNORED)),       \
      InstanceValue("MAX_CLIENT_WAIT_TIMEOUT_WEBGL", Napi::Number::New(env, WEBGL2_MAX_CLIENT_WAIT_TIMEOUT_WEBGL))

#define WEBGL2_CONSTANTS                               \
  WEBGL2_CONSTANTS_PARAMETERS,                         \
      WEBGL2_CONSTANTS_TEXTURES,                       \
      WEBGL2_CONSTANTS_PIXELTYPES,                     \
      WEBGL2_CONSTANTS_QUERIES,                        \
      WEBGL2_CONSTANTS_DRAWBUFFERS,                    \
      WEBGL2_CONSTANTS_SAMPLERS,                       \
      WEBGL2_CONSTANTS_BUFFERS,                        \
      WEBGL2_CONSTANTS_DATA_TYPES,                     \
      WEBGL2_CONSTANTS_VERTEX_ATTRIBS,                 \
      WEBGL2_CONSTANTS_TRANSFORM_FEEDBACK,             \
      WEBGL2_CONSTANTS_FRAMEBUFFERS_AND_RENDERBUFFERS, \
      WEBGL2_CONSTANTS_UNIFORMS,                       \
      WEBGL2_CONSTANTS_SYNC_OBJECTS,                   \
      WEBGL2_CONSTANTS_MISCELLANEOUS

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

#define WEBGL1_ACCESSORS(T)                                                                           \
  InstanceAccessor<&T::DrawingBufferWidthGetter, &T::DrawingBufferWidthSetter>("drawingBufferWidth"), \
      InstanceAccessor<&T::DrawingBufferHeightGetter, &T::DrawingBufferHeightSetter>("drawingBufferHeight")

  template <typename T>
  WebGLBaseRenderingContext<T>::WebGLBaseRenderingContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    m_renderAPI = RenderAPI::Get();
    if (m_renderAPI == nullptr)
    {
      Napi::TypeError::New(env, "RenderAPI is not available")
          .ThrowAsJavaScriptException();
      return;
    }

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

    auto initCommandBuffer = new renderer::ContextInitCommandBuffer();
    addCommandBuffer(initCommandBuffer, true, true);

    this->maxCombinedTextureImageUnits = initCommandBuffer->maxCombinedTextureImageUnits;
    this->maxCubeMapTextureSize = initCommandBuffer->maxCubeMapTextureSize;
    this->maxFragmentUniformVectors = initCommandBuffer->maxFragmentUniformVectors;
    this->maxRenderbufferSize = initCommandBuffer->maxRenderbufferSize;
    this->maxTextureImageUnits = initCommandBuffer->maxTextureImageUnits;
    this->maxTextureSize = initCommandBuffer->maxTextureSize;
    this->maxVaryingVectors = initCommandBuffer->maxVaryingVectors;
    this->maxVertexAttribs = initCommandBuffer->maxVertexAttribs;
    this->maxVertexTextureImageUnits = initCommandBuffer->maxVertexTextureImageUnits;
    this->maxVertexUniformVectors = initCommandBuffer->maxVertexUniformVectors;
    this->vendor = initCommandBuffer->vendor;
    this->version = initCommandBuffer->version;
    this->renderer = initCommandBuffer->renderer;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::MakeXRCompatible(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    m_XRCompatible = true;
    contextAttributes.xrCompatible = true;
    return env.Undefined();
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

    auto commandBuffer = new renderer::CreateProgramCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);

    return WebGLProgram::constructor->New({Napi::Number::New(env, commandBuffer->m_ProgramId)});
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
    addCommandBuffer(new renderer::DeleteProgramCommandBuffer(program->GetId()));
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
    auto commandBuffer = new renderer::LinkProgramCommandBuffer(program->GetId());
    addCommandBuffer(commandBuffer, true, true);

    /**
     * See https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getUniformLocation#name
     *
     * When uniforms declared as an array, the valid name might be like the followings:
     *
     * - foo
     * - foo[0]
     * - foo[1]
     */
    auto uniforms = commandBuffer->m_UniformLocations;
    for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
    {
      auto name = it->first;
      auto uniform = it->second;

      if (uniform.size == 1)
      {
        program->SetUniformLocation(name, uniform.location);
      }
      else if (uniform.size > 1)
      {
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
        std::string suffix = "[0]";
        auto end = name.length() - suffix.length();
        if (name.size() < suffix.size() || name.rfind(suffix) != end)
        {
          continue;
        }
        auto arrayName = name.substr(0, end);
        program->SetUniformLocation(arrayName, uniform.location);
        program->SetUniformLocation(name, uniform.location);
        for (int i = 1; i < uniform.size; i++)
          program->SetUniformLocation(arrayName + "[" + std::to_string(i) + "]", uniform.location + i);
      }
      else
      {
        // TODO: warning size is invalid?
        continue;
      }
    }
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
    addCommandBuffer(new renderer::UseProgramCommandBuffer(program->GetId()));
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
    auto commandBuffer = new renderer::GetProgramParameterCommandBuffer(program->GetId(), pname);
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_Value);
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
    auto commandBuffer = new renderer::GetProgramInfoLogCommandBuffer(program->GetId());
    addCommandBuffer(commandBuffer, true, true);
    return Napi::String::New(env, commandBuffer->m_InfoLog);
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
    int shader = info[1].As<Napi::Number>().Int32Value();
    addCommandBuffer(
        new renderer::AttachShaderCommandBuffer(program->GetId(), shader),
        true,
        false);
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
    int shader = info[1].As<Napi::Number>().Int32Value();
    addCommandBuffer(new renderer::DetachShaderCommandBuffer(program->GetId(), shader), true, false);
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

    auto commandBuffer = new renderer::CreateShaderCommandBuffer(type);
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_ShaderId);
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
    auto commandBuffer = new renderer::DeleteShaderCommandBuffer(shader);
    addCommandBuffer(commandBuffer, true, true);
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

    auto commandBuffer = new renderer::ShaderSourceCommandBuffer(shader, source.c_str(), source.length());
    addCommandBuffer(commandBuffer, true, false);
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
    addCommandBuffer(new renderer::CompileShaderCommandBuffer(shader), true, false);
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
    auto commandBuffer = new renderer::GetShaderSourceCommandBuffer(shader);
    addCommandBuffer(commandBuffer, true, true);
    return Napi::String::New(env, commandBuffer->m_Source);
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
    auto commandBuffer = new renderer::GetShaderParameterCommandBuffer(shader, pname);
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_Value);
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
    auto commandBuffer = new renderer::GetShaderInfoLogCommandBuffer(shader);
    addCommandBuffer(commandBuffer, true, true);
    return Napi::String::New(env, commandBuffer->m_InfoLog);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::CreateBufferCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_BufferId);
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
    addCommandBuffer(new renderer::DeleteBufferCommandBuffer(buffer), true, false);
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
      Napi::TypeError::New(env, "the first argument should be a number when calling bindBuffer().")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    int target = info[0].As<Napi::Number>().Int32Value();
    int buffer = 0;
    if (info[1].IsNumber())
      buffer = info[1].As<Napi::Number>().Int32Value();
    addCommandBuffer(new renderer::BindBufferCommandBuffer(target, buffer));
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
    int target = info[0].As<Napi::Number>().Int32Value();
    Napi::Uint8Array buffer = info[1].As<Napi::Uint8Array>();
    int usage = info[2].As<Napi::Number>().Int32Value();

    auto commandBuffer = new renderer::BufferDataCommandBuffer(
        target,
        buffer.ByteLength(),
        buffer.Data(),
        usage);
    addCommandBuffer(commandBuffer);
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
    int target = info[0].As<Napi::Number>().Int32Value();
    int offset = info[1].As<Napi::Number>().Int32Value();
    Napi::ArrayBuffer buffer = info[2].As<Napi::ArrayBuffer>();

    auto commandBuffer = new renderer::BufferSubDataCommandBuffer(
        target,
        offset,
        buffer.ByteLength(),
        buffer.Data());
    addCommandBuffer(commandBuffer);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateFramebuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::CreateFramebufferCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_FramebufferId);
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
    addCommandBuffer(new renderer::DeleteFramebufferCommandBuffer(framebuffer), true, false);
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
    addCommandBuffer(new renderer::BindFramebufferCommandBuffer(target, framebuffer));
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

    addCommandBuffer(new renderer::FramebufferRenderbufferCommandBuffer(
        target,
        attachment,
        renderbuffertarget,
        renderbuffer));
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

    addCommandBuffer(new renderer::FramebufferTexture2DCommandBuffer(
        target,
        attachment,
        textarget,
        texture->GetId(),
        level));
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
    auto commandBuffer = new renderer::CheckFramebufferStatusCommandBuffer(target);
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_Status);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateRenderbuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::CreateRenderbufferCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_RenderbufferId);
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
    addCommandBuffer(new renderer::DeleteRenderbufferCommandBuffer(renderbuffer));
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
    addCommandBuffer(new renderer::BindRenderbufferCommandBuffer(target, renderbuffer));
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

    addCommandBuffer(new renderer::RenderbufferStorageCommandBuffer(
        target,
        internalformat,
        width,
        height));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CreateTexture(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::CreateTextureCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);
    return WebGLTexture::constructor->New({Napi::Number::New(env, commandBuffer->m_TextureId)});
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
    addCommandBuffer(new renderer::DeleteTextureCommandBuffer(texture->GetId()));
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

    addCommandBuffer(new renderer::BindTextureCommandBuffer(target, texture));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::TexImage2D(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    int level = info[1].As<Napi::Number>().Int32Value();
    int internalformat = info[2].As<Napi::Number>().Int32Value();
    int width = info[3].As<Napi::Number>().Int32Value();
    int height = info[4].As<Napi::Number>().Int32Value();
    int border = info[5].As<Napi::Number>().Int32Value();
    int format = info[6].As<Napi::Number>().Int32Value();
    int type = info[7].As<Napi::Number>().Int32Value();

    Napi::Value imageSource = info[8];
    if (imageSource.IsNull())
    {
      /** When the image source is null, just create TexImage2DCommandBuffer with empty mode */
      addCommandBuffer(new renderer::TexImage2DCommandBuffer(
          target,
          level,
          internalformat,
          width,
          height,
          border,
          format,
          type));
      return env.Undefined();
    }

    // Otherwise, the image source should be a TypedArray
    if (!imageSource.IsTypedArray())
    {
      Napi::TypeError::New(env, "the pixels should be a TypedArray.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::TypedArray pixels = info[8].As<Napi::TypedArray>();
    Napi::ArrayBuffer data = pixels.ArrayBuffer();
    renderer::TexImage2DCommandBuffer *commandBuffer;

    if (m_unpackFlipY == true || m_unpackPremultiplyAlpha == true)
    {
      unsigned char *packedPixels = reinterpret_cast<unsigned char *>(data.Data());
      unsigned char *pixels = unpackPixels(type, format, width, height, packedPixels);
      commandBuffer = new renderer::TexImage2DCommandBuffer(
          target,
          level,
          internalformat,
          width,
          height,
          border,
          format,
          type,
          data.ByteLength(),
          pixels);
      delete[] pixels;
    }
    else
    {
      commandBuffer = new renderer::TexImage2DCommandBuffer(
          target,
          level,
          internalformat,
          width,
          height,
          border,
          format,
          type,
          data.ByteLength(),
          data.Data());
    }
    addCommandBuffer(commandBuffer);
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
    renderer::TexSubImage2DCommandBuffer *commandBuffer;

    if (m_unpackFlipY == true || m_unpackPremultiplyAlpha == true)
    {
      unsigned char *packedPixels = reinterpret_cast<unsigned char *>(data.Data());
      unsigned char *pixels = unpackPixels(type, format, width, height, packedPixels);
      commandBuffer = new renderer::TexSubImage2DCommandBuffer(
          target,
          level,
          xoffset,
          yoffset,
          width,
          height,
          format,
          type,
          data.ByteLength(),
          pixels);
      delete[] pixels;
    }
    else
    {
      commandBuffer = new renderer::TexSubImage2DCommandBuffer(
          target,
          level,
          xoffset,
          yoffset,
          width,
          height,
          format,
          type,
          data.ByteLength(),
          data.Data());
    }
    addCommandBuffer(commandBuffer);
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

    addCommandBuffer(new renderer::CopyTexImage2DCommandBuffer(
        target,
        level,
        internalformat,
        x,
        y,
        width,
        height,
        border));
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

    addCommandBuffer(new renderer::CopyTexSubImage2DCommandBuffer(
        target,
        level,
        xoffset,
        yoffset,
        x,
        y,
        width,
        height));
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
    addCommandBuffer(new renderer::TexParameteriCommandBuffer(target, pname, param));
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
    addCommandBuffer(new renderer::ActiveTextureCommandBuffer(textureUnit));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GenerateMipmap(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int target = info[0].As<Napi::Number>().Int32Value();
    addCommandBuffer(new renderer::GenerateMipmapCommandBuffer(target));
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
    addCommandBuffer(new renderer::EnableVertexAttribArrayCommandBuffer(index));
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
    int index = info[0].As<Napi::Number>().Int32Value();
    addCommandBuffer(new renderer::DisableVertexAttribArrayCommandBuffer(index));
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
    int index = info[0].As<Napi::Number>().Int32Value();
    int size = info[1].As<Napi::Number>().Int32Value();
    int type = info[2].As<Napi::Number>().Int32Value();
    bool normalized = info[3].As<Napi::Boolean>().Value();
    int stride = info[4].As<Napi::Number>().Int32Value();
    int offset = info[5].As<Napi::Number>().Int32Value();

    auto commandBuffer = new renderer::VertexAttribPointerCommandBuffer(
        index,
        size,
        type,
        normalized,
        stride,
        (char *)NULL + offset);
    addCommandBuffer(commandBuffer);
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

    auto commandBuffer = new renderer::GetAttribLocationCommandBuffer(program->GetId(), name.c_str());
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_Location);
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
    float x = info[1].As<Napi::Number>().FloatValue();
    addCommandBuffer(new renderer::Uniform1fCommandBuffer(location->GetValue(), x));
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

    auto commandBuffer = new renderer::Uniform1fvCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform1iCommandBuffer(location->GetValue(), x));
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

    auto commandBuffer = new renderer::Uniform1ivCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform2fCommandBuffer(location->GetValue(), x, y));
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

    auto commandBuffer = new renderer::Uniform2fvCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform2iCommandBuffer(location->GetValue(), x, y));
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

    auto commandBuffer = new renderer::Uniform2ivCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform3fCommandBuffer(location->GetValue(), x, y, z));
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

    auto commandBuffer = new renderer::Uniform3fvCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform3iCommandBuffer(location->GetValue(), x, y, z));
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

    auto commandBuffer = new renderer::Uniform3ivCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform4fCommandBuffer(location->GetValue(), x, y, z, w));
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

    auto commandBuffer = new renderer::Uniform4fvCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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
    addCommandBuffer(new renderer::Uniform4iCommandBuffer(location->GetValue(), x, y, z, w));
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

    auto commandBuffer = new renderer::Uniform4ivCommandBuffer(location->GetValue(), data);
    addCommandBuffer(commandBuffer);
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

    auto commandBuffer = new renderer::UniformMatrix2fvCommandBuffer(
        location->GetValue(),
        transpose,
        data);
    addCommandBuffer(commandBuffer);
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

    auto commandBuffer = new renderer::UniformMatrix3fvCommandBuffer(
        location->GetValue(),
        transpose,
        data);
    addCommandBuffer(commandBuffer);
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::UniformMatrix4fv(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() takes 3 arguments.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    else if (info.Length() > 3)
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() don't support the parameters: `srcLength` and `srcOffset`")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsObject() || !info[0].As<Napi::Object>().InstanceOf(WebGLUniformLocation::constructor->Value()))
    {
      Napi::TypeError::New(env, "uniformMatrix4fv() 1st argument(program) must be a WebGLUniformLocation object.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    bool transpose = info[1].As<Napi::Boolean>().Value();
    Napi::Float32Array array = info[2].As<Napi::Float32Array>();

    // Check the array is a "MatrixPlaceholder", if own `_isXRMatrixPlaceholder` property and to be true
    string xrMatrixPlaceholderField = "_isXRMatrixPlaceholder";
    if (array.Has(xrMatrixPlaceholderField) && array.Get(xrMatrixPlaceholderField).ToBoolean().Value() == true)
    {
      auto typeOfMatrixPlaceholder = array.Get("type").ToNumber().Int32Value();
      auto commandBuffer = new renderer::UniformMatrix4fvCommandBuffer(
          location->GetValue(),
          transpose,
          (renderer::MatrixPlaceholderType)typeOfMatrixPlaceholder);

      if (array.Has("xrSessionId"))
      {
        auto xrSessionValue = array.Get("xrSessionId");
        if (xrSessionValue.IsNumber())
          commandBuffer->m_XrSessionId = xrSessionValue.ToNumber().Int32Value();
      }
      addCommandBuffer(commandBuffer);
    }
    else
    {
      size_t length = array.ElementLength();
      if (length % 16 != 0)
      {
        Napi::TypeError::New(env,
                             "uniformMatrix4fv() must take 16x float elements array but accept " + std::to_string(length) + ".")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }

      std::vector<float> data(length);
      for (size_t i = 0; i < length; i++)
        data[i] = array.Get(i).ToNumber().FloatValue();

      auto commandBuffer = new renderer::UniformMatrix4fvCommandBuffer(
          location->GetValue(),
          transpose,
          data);
      addCommandBuffer(commandBuffer);
    }
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

    addCommandBuffer(new renderer::DrawArraysCommandBuffer(mode, first, count));
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
    addCommandBuffer(new renderer::DrawElementsCommandBuffer(mode, count, type, (char *)NULL + offset));
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
    int pname = info[0].As<Napi::Number>().Int32Value();
    int param = info[1].ToNumber().Int32Value();

    if (
        pname == WEBGL_PACK_ALIGNMENT ||
        pname == WEBGL_UNPACK_ALIGNMENT)
    {
      addCommandBuffer(new renderer::PixelStoreiCommandBuffer(pname, param));
    }
    else if (pname == WEBGL_UNPACK_FLIP_Y_WEBGL)
    {
      m_unpackFlipY = param;
    }
    else if (pname == WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL)
    {
      m_unpackPremultiplyAlpha = param;
    }
    else
    {
      // TODO: other pixel store parameters
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
    addCommandBuffer(new renderer::PolygonOffsetCommandBuffer(factor, units));
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

    /**
     * Only the viewport is changed, we need to dispatch a SetViewportCommandBuffer.
     */
    if (m_renderAPI->HasViewportChanged(x, y, width, height) == true)
      addCommandBuffer(new renderer::SetViewportCommandBuffer(x, y, width, height));
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
    addCommandBuffer(new renderer::SetScissorCommandBuffer(x, y, width, height));
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
      addCommandBuffer(new renderer::DepthMaskCommandBuffer(flag));
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
      addCommandBuffer(new renderer::DepthFuncCommandBuffer(func));
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
      addCommandBuffer(new renderer::DepthRangeCommandBuffer(near, far));
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
      addCommandBuffer(new renderer::StencilFuncCommandBuffer(func, ref, mask));
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
      addCommandBuffer(new renderer::StencilFuncSeparateCommandBuffer(face, func, ref, mask));
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
      addCommandBuffer(new renderer::StencilMaskCommandBuffer(mask));
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
      addCommandBuffer(new renderer::StencilMaskSeparateCommandBuffer(face, mask));
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
      addCommandBuffer(new renderer::StencilOpCommandBuffer(fail, zfail, zpass));
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
      addCommandBuffer(new renderer::StencilOpSeparateCommandBuffer(face, fail, zfail, zpass));
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
      addCommandBuffer(new renderer::BlendColorCommandBuffer(r, g, b, a));
    }
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendEquation(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t mode = info[0].ToNumber().Uint32Value();
    addCommandBuffer(new renderer::BlendEquationCommandBuffer(mode));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendEquationSeparate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t modeRGB = info[0].ToNumber().Uint32Value();
    uint32_t modeAlpha = info[1].ToNumber().Uint32Value();
    addCommandBuffer(new renderer::BlendEquationSeparateCommandBuffer(modeRGB, modeAlpha));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::BlendFunc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t sfactor = info[0].ToNumber().Uint32Value();
    uint32_t dfactor = info[1].ToNumber().Uint32Value();
    addCommandBuffer(new renderer::BlendFuncCommandBuffer(sfactor, dfactor));
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
    addCommandBuffer(new renderer::BlendFuncSeparateCommandBuffer(srcRGB, dstRGB, srcAlpha, dstAlpha));
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
    addCommandBuffer(new renderer::ColorMaskCommandBuffer(r, g, b, a));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::CullFace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t mode = info[0].ToNumber().Uint32Value();
    addCommandBuffer(new renderer::CullFaceCommandBuffer(mode));
    return env.Undefined();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::FrontFace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    uint32_t mode = info[0].ToNumber().Uint32Value();
    addCommandBuffer(new renderer::FrontFaceCommandBuffer(mode));
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
      addCommandBuffer(new renderer::EnableCommandBuffer(mask));
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
      addCommandBuffer(new renderer::DisableCommandBuffer(mask));
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
        auto commandBuffer = new renderer::GetIntegervCommandBuffer(pname);
        addCommandBuffer(commandBuffer, true, true);

        return Napi::Number::New(env, commandBuffer->m_Value);
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
        auto commandBuffer = new renderer::GetBooleanvCommandBuffer(pname);
        addCommandBuffer(commandBuffer, true, true);

        return Napi::Boolean::New(env, commandBuffer->m_Value);
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
        auto commandBuffer = new renderer::GetIntegervCommandBuffer(pname);
        addCommandBuffer(commandBuffer, true, true);

        return Napi::Number::New(env, commandBuffer->m_Value);
      }
      // GLubyte/string
      case WEBGL_RENDERER:
      case WEBGL_SHADING_LANGUAGE_VERSION:
      case WEBGL_VENDOR:
      case WEBGL_VERSION:
      {
        auto commandBuffer = new renderer::GetStringCommandBuffer(pname);
        addCommandBuffer(commandBuffer, true, true);
        return Napi::String::New(env, commandBuffer->m_Value);
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

    auto commandBuffer = new renderer::GetShaderPrecisionFormatCommandBuffer(shadertype, precisiontype);
    addCommandBuffer(commandBuffer, true, true);

    Napi::Object obj = Napi::Object::New(env);
    obj.Set("rangeMin", Napi::Number::New(env, commandBuffer->m_RangeMin));
    obj.Set("rangeMax", Napi::Number::New(env, commandBuffer->m_RangeMax));
    obj.Set("precision", Napi::Number::New(env, commandBuffer->m_Precision));
    return obj;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetError(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    auto commandBuffer = new renderer::GetErrorCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);
    return Napi::Number::New(env, commandBuffer->m_Error);
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::GetSupportedExtensions(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto commandBuffer = new renderer::GetSupportedExtensionsCommandBuffer();
    addCommandBuffer(commandBuffer, true, true);

    Napi::Array extensionsArray = Napi::Array::New(env, commandBuffer->m_Extensions.size());
    for (size_t i = 0; i < commandBuffer->m_Extensions.size(); i++)
    {
      // remove GL_ prefix
      std::string extension = commandBuffer->m_Extensions[i];
      Napi::String jsExtensionName;
      if (extension.find("GL_") == 0)
        jsExtensionName = Napi::String::New(env, extension.substr(3));
      else
        jsExtensionName = Napi::String::New(env, extension);
      extensionsArray.Set(i, jsExtensionName);
    }
    return extensionsArray;
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DrawingBufferWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, m_renderAPI->GetDrawingBufferWidth());
  }

  template <typename T>
  void WebGLBaseRenderingContext<T>::DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferWidth is readonly.").ThrowAsJavaScriptException();
  }

  template <typename T>
  Napi::Value WebGLBaseRenderingContext<T>::DrawingBufferHeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, m_renderAPI->GetDrawingBufferHeight());
  }

  template <typename T>
  void WebGLBaseRenderingContext<T>::DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "drawingBufferHeight is readonly.").ThrowAsJavaScriptException();
  }

  template <typename T>
  bool WebGLBaseRenderingContext<T>::addCommandBuffer(
      renderer::CommandBuffer *commandBuffer,
      bool useDefaultQueue,
      bool waitForFinished)
  {
    if (waitForFinished)
    {
      /**
       * Only default queue supports waiting for the command buffer to be finished.
       */
      useDefaultQueue = true;
      /**
       * Set the command buffer to be perserved when finished, then this instance will be responsible for deleting it.
       */
      commandBuffer->PerserveWhenFinished();
    }

    if (m_XRCompatible == true && useDefaultQueue == false)
    {
      auto device = xr::Device::GetInstance();
      assert(device != nullptr);

      if (device->isInFrame())
      {
        device->addCommandBufferToFrame(commandBuffer);
        return true;
      }
      // If not in XR frame, add the command buffer to the default render queue.
    }

    // Otherwise, add the command buffer to the default render queue.
    m_renderAPI->AddCommandBuffer(commandBuffer);

    // Wait for the command buffer to be finished if needed.
    if (waitForFinished)
    {
      commandBuffer->WaitFinished();
      // TODO: release the command buffer?
    }
    return true;
  }

  /**
   * Source from https://github.com/stackgl/headless-gl/blob/v8.0.2/src/native/webgl.cc#L722
   */
  template <typename T>
  unsigned char *WebGLBaseRenderingContext<T>::unpackPixels(int type, int format, int width, int height, unsigned char *pixels)
  {
    // Compute the pixel size
    int pixelSize = 1;
    if (type == WEBGL_UNSIGNED_BYTE || type == WEBGL_FLOAT)
    {
      if (type == WEBGL_FLOAT)
        pixelSize = 4;
      switch (format)
      {
      case WEBGL_ALPHA:
      case WEBGL_LUMINANCE:
        break;
      case WEBGL_LUMINANCE_ALPHA:
        pixelSize *= 2;
        break;
      case WEBGL_RGB:
        pixelSize *= 3;
        break;
      case WEBGL_RGBA:
        pixelSize *= 4;
        break;
      default:
        break;
      }
    }
    else
    {
      pixelSize = 2;
    }

    // Compute the row stride
    int rowStride = width * pixelSize;
    if ((rowStride % m_unpackAlignment) != 0)
    {
      rowStride += m_unpackAlignment - (rowStride % m_unpackAlignment);
    }

    int imageSize = rowStride * height;
    unsigned char *unpacked = new unsigned char[imageSize];

    if (m_unpackFlipY)
    {
      for (int i = 0, j = height - 1; j >= 0; ++i, --j)
      {
        memcpy(
            reinterpret_cast<void *>(unpacked + j * rowStride),
            reinterpret_cast<void *>(pixels + i * rowStride),
            width * pixelSize);
      }
    }
    else
    {
      memcpy(
          reinterpret_cast<void *>(unpacked),
          reinterpret_cast<void *>(pixels),
          imageSize);
    }

    if (m_unpackPremultiplyAlpha && (format == WEBGL_LUMINANCE_ALPHA || format == WEBGL_RGBA))
    {
      for (int row = 0; row < height; ++row)
      {
        for (int col = 0; col < width; ++col)
        {
          unsigned char *pixel = unpacked + (row + rowStride) + (col * pixelSize);
          if (format == WEBGL_LUMINANCE_ALPHA)
          {
            pixel[0] *= pixel[1] / 255.0f;
          }
          else if (type == WEBGL_UNSIGNED_BYTE)
          {
            float scale = pixel[3] / 255.0f;
            pixel[0] *= scale;
            pixel[1] *= scale;
            pixel[2] *= scale;
          }
          else if (type == WEBGL_UNSIGNED_SHORT_4_4_4_4)
          {
            int r = pixel[0] & 0x0f;
            int g = pixel[0] >> 4;
            int b = pixel[1] & 0x0f;
            int a = pixel[1] >> 4;
            float scale = a / 15.0f;
            r *= scale;
            g *= scale;
            b *= scale;

            pixel[0] = r | (g << 4);
            pixel[1] = b | (a << 4);
          }
        }
      }
    }
    return unpacked;
  }

  Napi::Object WebGLRenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "WebGLRenderingContext",
        {WEBGL1_CONSTANTS,
         WEBGL1_METHODS(WebGLRenderingContext),
         WEBGL1_ACCESSORS(WebGLRenderingContext)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
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
        {WEBGL1_CONSTANTS,
         WEBGL2_CONSTANTS,
         WEBGL1_METHODS(WebGL2RenderingContext),
         WEBGL1_ACCESSORS(WebGL2RenderingContext)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("WebGL2RenderingContext", tpl);
    return exports;
  }

  WebGL2RenderingContext::WebGL2RenderingContext(const Napi::CallbackInfo &info) : WebGLBaseRenderingContext<WebGL2RenderingContext>(info)
  {
    auto initCommand = new renderer::Context2InitCommandBuffer();
    addCommandBuffer(initCommand, true, true);

    this->max3DTextureSize = initCommand->max3DTextureSize;
    this->maxArrayTextureLayers = initCommand->maxArrayTextureLayers;
    this->maxColorAttachments = initCommand->maxColorAttachments;
    this->maxCombinedUniformBlocks = initCommand->maxCombinedUniformBlocks;
    this->maxDrawBuffers = initCommand->maxDrawBuffers;
    this->maxElementsIndices = initCommand->maxElementsIndices;
    this->maxElementsVertices = initCommand->maxElementsVertices;
    this->maxFragmentInputComponents = initCommand->maxFragmentInputComponents;
    this->maxFragmentUniformBlocks = initCommand->maxFragmentUniformBlocks;
    this->maxFragmentUniformComponents = initCommand->maxFragmentUniformComponents;
    this->maxProgramTexelOffset = initCommand->maxProgramTexelOffset;
    this->maxSamples = initCommand->maxSamples;
    this->maxTransformFeedbackInterleavedComponents = initCommand->maxTransformFeedbackInterleavedComponents;
    this->maxTransformFeedbackSeparateAttributes = initCommand->maxTransformFeedbackSeparateAttributes;
    this->maxTransformFeedbackSeparateComponents = initCommand->maxTransformFeedbackSeparateComponents;
    this->maxUniformBufferBindings = initCommand->maxUniformBufferBindings;
    this->maxVaryingComponents = initCommand->maxVaryingComponents;
    this->maxVertexOutputComponents = initCommand->maxVertexOutputComponents;
    this->maxVertexUniformBlocks = initCommand->maxVertexUniformBlocks;
    this->maxVertexUniformComponents = initCommand->maxVertexUniformComponents;
    this->minProgramTexelOffset = initCommand->minProgramTexelOffset;

    this->maxClientWaitTimeout = initCommand->maxClientWaitTimeout;
    this->maxCombinedFragmentUniformComponents = initCommand->maxCombinedFragmentUniformComponents;
    this->maxCombinedVertexUniformComponents = initCommand->maxCombinedVertexUniformComponents;
    this->maxElementIndex = initCommand->maxElementIndex;
    this->maxServerWaitTimeout = initCommand->maxServerWaitTimeout;
    this->maxUniformBlockSize = initCommand->maxUniformBlockSize;
    this->maxTextureLODBias = initCommand->maxTextureLODBias;
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
}