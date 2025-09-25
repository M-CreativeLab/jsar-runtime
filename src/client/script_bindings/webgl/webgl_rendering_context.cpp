#include "./webgl_rendering_context.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLRenderingContext::SetupConstants(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // WebGL 1.0 constants as per MDN spec
      auto prototype = tpl->PrototypeTemplate();

#define ADD_WEBGL_CONSTANT(name) \
  prototype->Set(String::NewFromUtf8(isolate, #name).ToLocalChecked(), Integer::New(isolate, WEBGL_##name));

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

#define WEBGL1_CONSTANTS                   \
  WEBGL1_CONSTANTS_CLEARING_BUFFERS        \
  WEBGL1_CONSTANTS_RENDERING_PRIMITIVES    \
  WEBGL1_CONSTANTS_BLENDING                \
  WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS \
  WEBGL1_CONSTANTS_CULLING                 \
  WEBGL1_CONSTANTS_ENABLE_AND_DISABLE      \
  WEBGL1_CONSTANTS_ERRORS                  \
  WEBGL1_CONSTANTS_FRONTFACE               \
  WEBGL1_CONSTANTS_HINTS                   \
  WEBGL1_CONSTANTS_DATATYPES               \
  WEBGL1_CONSTANTS_PIXELS                  \
  WEBGL1_CONSTANTS_SHADERS                 \
  WEBGL1_CONSTANTS_DEPTH_AND_STENCIL       \
  WEBGL1_CONSTANTS_TEXTURE                 \
  WEBGL1_CONSTANTS_UNIFORM_TYPES           \
  WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER

      prototype->Set(String::NewFromUtf8(isolate, "__webgl1_constants__").ToLocalChecked(),
                     Boolean::New(isolate, true));
    }

    void WebGLRenderingContext::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // Set up the WebGL 1.0 API methods and properties

      // Buffer operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createBuffer").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateBuffer));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteBuffer").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteBuffer));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "bindBuffer").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, BindBuffer));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "bufferData").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, BufferData));

      // Shader operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateShader));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteShader));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "shaderSource").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, ShaderSource));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "compileShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CompileShader));

      // Program operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateProgram));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteProgram));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "attachShader").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, AttachShader));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "linkProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, LinkProgram));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "useProgram").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, UseProgram));

      // Texture operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "createTexture").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, CreateTexture));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "deleteTexture").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DeleteTexture));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "bindTexture").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, BindTexture));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "texImage2D").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, TexImage2D));

      // Drawing operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "clear").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Clear));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "clearColor").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, ClearColor));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "viewport").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Viewport));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "drawArrays").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DrawArrays));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "drawElements").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, DrawElements));

      // Uniform operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "getUniformLocation").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, GetUniformLocation));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "uniform1f").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Uniform1f));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "uniform1i").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Uniform1i));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "uniformMatrix4fv").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, UniformMatrix4fv));

      // Error and state operations
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "getError").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, GetError));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "enable").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Enable));
      tpl->InstanceTemplate()->Set(String::NewFromUtf8(isolate, "disable").ToLocalChecked(),
                                   FunctionTemplate::New(isolate, Disable));

      // Canvas property
      tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "canvas").ToLocalChecked(),
                                           CanvasGetter);

      // WebGL constants
      SetupConstants(isolate, tpl);
    }

    Local<Object> WebGLRenderingContext::NewInstance(Isolate *isolate,
                                                     shared_ptr<client_graphics::WebGLContext> nativeContext)
    {
      EscapableHandleScope scope(isolate);
      if (nativeContext == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(WebGLRenderingContextBase::NewInstance(isolate, nativeContext).As<Object>());
      }
    }

    WebGLRenderingContext::WebGLRenderingContext(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLRenderingContextBase(isolate, args)
    {
    }

    // Method implementations - stubs for now, would need native implementation
    void WebGLRenderingContext::CreateBuffer(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL buffer creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::CreateShader(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL shader creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::CreateProgram(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL program creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::CreateTexture(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL texture creation
      args.GetReturnValue().SetNull();
    }

    void WebGLRenderingContext::Clear(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL clear
    }

    void WebGLRenderingContext::Viewport(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL viewport
    }

    void WebGLRenderingContext::DrawArrays(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      // TODO: Implement native WebGL draw arrays
    }

    void WebGLRenderingContext::CanvasGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      // TODO: Return associated canvas object
      info.GetReturnValue().SetNull();
    }

    // Stub implementations for other methods
    void WebGLRenderingContext::DeleteBuffer(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }

    void WebGLRenderingContext::BindBuffer(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::BufferData(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DeleteShader(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::ShaderSource(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::CompileShader(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DeleteProgram(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::AttachShader(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::LinkProgram(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::UseProgram(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DeleteTexture(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::BindTexture(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::TexImage2D(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::ClearColor(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::DrawElements(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::GetUniformLocation(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Uniform1f(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Uniform1i(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::UniformMatrix4fv(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::GetError(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Enable(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
    void WebGLRenderingContext::Disable(const FunctionCallbackInfo<Value> &args)
    { /* TODO */
    }
  }
}