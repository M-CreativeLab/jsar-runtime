#include "./webgl_rendering_context.hpp"
#include "./shader.hpp"
#include "./program.hpp"
#include "./buffer.hpp"
#include "./texture.hpp"
#include "./renderbuffer.hpp"
#include "./framebuffer.hpp"
#include "./extensions/all.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLRenderingContext::SetupConstants(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // WebGL 1.0 constants as per MDN spec
#define ADD_WEBGL_CONSTANT(NAME) IntegerConstant(isolate, tpl, #NAME, WEBGL_##NAME);

#define WEBGL1_CONSTANTS_CLEARING_BUFFERS() \
  ADD_WEBGL_CONSTANT(DEPTH_BUFFER_BIT)      \
  ADD_WEBGL_CONSTANT(STENCIL_BUFFER_BIT)    \
  ADD_WEBGL_CONSTANT(COLOR_BUFFER_BIT)

#define WEBGL1_CONSTANTS_RENDERING_PRIMITIVES() \
  ADD_WEBGL_CONSTANT(POINTS)                    \
  ADD_WEBGL_CONSTANT(LINES)                     \
  ADD_WEBGL_CONSTANT(LINE_LOOP)                 \
  ADD_WEBGL_CONSTANT(LINE_STRIP)                \
  ADD_WEBGL_CONSTANT(TRIANGLES)                 \
  ADD_WEBGL_CONSTANT(TRIANGLE_STRIP)            \
  ADD_WEBGL_CONSTANT(TRIANGLE_FAN)

#define WEBGL1_CONSTANTS_BLENDING()                    \
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

#define WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS()   \
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

#define WEBGL1_CONSTANTS_CULLING() \
  ADD_WEBGL_CONSTANT(CULL_FACE)    \
  ADD_WEBGL_CONSTANT(FRONT)        \
  ADD_WEBGL_CONSTANT(BACK)         \
  ADD_WEBGL_CONSTANT(FRONT_AND_BACK)

#define WEBGL1_CONSTANTS_ENABLE_AND_DISABLE()  \
  ADD_WEBGL_CONSTANT(BLEND)                    \
  ADD_WEBGL_CONSTANT(DEPTH_TEST)               \
  ADD_WEBGL_CONSTANT(DITHER)                   \
  ADD_WEBGL_CONSTANT(POLYGON_OFFSET_FILL)      \
  ADD_WEBGL_CONSTANT(SAMPLE_ALPHA_TO_COVERAGE) \
  ADD_WEBGL_CONSTANT(SAMPLE_COVERAGE)          \
  ADD_WEBGL_CONSTANT(SCISSOR_TEST)             \
  ADD_WEBGL_CONSTANT(STENCIL_TEST)

#define WEBGL1_CONSTANTS_ERRORS()       \
  ADD_WEBGL_CONSTANT(NO_ERROR)          \
  ADD_WEBGL_CONSTANT(INVALID_ENUM)      \
  ADD_WEBGL_CONSTANT(INVALID_VALUE)     \
  ADD_WEBGL_CONSTANT(INVALID_OPERATION) \
  ADD_WEBGL_CONSTANT(OUT_OF_MEMORY)     \
  ADD_WEBGL_CONSTANT(CONTEXT_LOST_WEBGL)

#define WEBGL1_CONSTANTS_FRONTFACE() \
  ADD_WEBGL_CONSTANT(CW)             \
  ADD_WEBGL_CONSTANT(CCW)

#define WEBGL1_CONSTANTS_HINTS() \
  ADD_WEBGL_CONSTANT(DONT_CARE)  \
  ADD_WEBGL_CONSTANT(FASTEST)    \
  ADD_WEBGL_CONSTANT(NICEST)

#define WEBGL1_CONSTANTS_DATATYPES() \
  ADD_WEBGL_CONSTANT(BYTE)           \
  ADD_WEBGL_CONSTANT(UNSIGNED_BYTE)  \
  ADD_WEBGL_CONSTANT(SHORT)          \
  ADD_WEBGL_CONSTANT(UNSIGNED_SHORT) \
  ADD_WEBGL_CONSTANT(INT)            \
  ADD_WEBGL_CONSTANT(UNSIGNED_INT)   \
  ADD_WEBGL_CONSTANT(FLOAT)

#define WEBGL1_CONSTANTS_PIXELS()                    \
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

#define WEBGL1_CONSTANTS_SHADERS()                     \
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

#define WEBGL1_CONSTANTS_DEPTH_AND_STENCIL() \
  ADD_WEBGL_CONSTANT(NEVER)                  \
  ADD_WEBGL_CONSTANT(LESS)                   \
  ADD_WEBGL_CONSTANT(EQUAL)                  \
  ADD_WEBGL_CONSTANT(LEQUAL)                 \
  ADD_WEBGL_CONSTANT(GREATER)                \
  ADD_WEBGL_CONSTANT(NOTEQUAL)               \
  ADD_WEBGL_CONSTANT(GEQUAL)                 \
  ADD_WEBGL_CONSTANT(ALWAYS)                 \
  ADD_WEBGL_CONSTANT(KEEP)                   \
  ADD_WEBGL_CONSTANT(REPLACE)                \
  ADD_WEBGL_CONSTANT(INCR)                   \
  ADD_WEBGL_CONSTANT(DECR)                   \
  ADD_WEBGL_CONSTANT(INVERT)                 \
  ADD_WEBGL_CONSTANT(INCR_WRAP)              \
  ADD_WEBGL_CONSTANT(DECR_WRAP)

#define WEBGL1_CONSTANTS_TEXTURE()                \
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

#define WEBGL1_CONSTANTS_UNIFORM_TYPES() \
  ADD_WEBGL_CONSTANT(FLOAT_VEC2)         \
  ADD_WEBGL_CONSTANT(FLOAT_VEC3)         \
  ADD_WEBGL_CONSTANT(FLOAT_VEC4)         \
  ADD_WEBGL_CONSTANT(INT_VEC2)           \
  ADD_WEBGL_CONSTANT(INT_VEC3)           \
  ADD_WEBGL_CONSTANT(INT_VEC4)           \
  ADD_WEBGL_CONSTANT(BOOL)               \
  ADD_WEBGL_CONSTANT(BOOL_VEC2)          \
  ADD_WEBGL_CONSTANT(BOOL_VEC3)          \
  ADD_WEBGL_CONSTANT(BOOL_VEC4)          \
  ADD_WEBGL_CONSTANT(FLOAT_MAT2)         \
  ADD_WEBGL_CONSTANT(FLOAT_MAT3)         \
  ADD_WEBGL_CONSTANT(FLOAT_MAT4)         \
  ADD_WEBGL_CONSTANT(SAMPLER_2D)         \
  ADD_WEBGL_CONSTANT(SAMPLER_CUBE)

#define WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER()            \
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

      WEBGL1_CONSTANTS_CLEARING_BUFFERS()
      WEBGL1_CONSTANTS_RENDERING_PRIMITIVES()
      WEBGL1_CONSTANTS_BLENDING()
      WEBGL1_CONSTANTS_BUFFERS_AND_VERTEXATTRS()
      WEBGL1_CONSTANTS_CULLING()
      WEBGL1_CONSTANTS_ENABLE_AND_DISABLE()
      WEBGL1_CONSTANTS_ERRORS()
      WEBGL1_CONSTANTS_FRONTFACE()
      WEBGL1_CONSTANTS_HINTS()
      WEBGL1_CONSTANTS_DATATYPES()
      WEBGL1_CONSTANTS_PIXELS()
      WEBGL1_CONSTANTS_SHADERS()
      WEBGL1_CONSTANTS_DEPTH_AND_STENCIL()
      WEBGL1_CONSTANTS_TEXTURE()
      WEBGL1_CONSTANTS_UNIFORM_TYPES()
      WEBGL1_CONSTANTS_FRAMEBUFFER_AND_RENDERBUFFER()
#undef ADD_WEBGL_CONSTANT
    }

    void WebGLRenderingContext::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // WebGL constants
      SetupConstants(isolate, tpl);

      // Set up the WebGL 1.0 API methods and properties
      auto prototype = tpl->PrototypeTemplate();

#define ADD_WEBGL1_METHOD(NAME, CALLBACK) \
  InstanceMethod(isolate,                 \
                 prototype,               \
                 NAME,                    \
                 &WebGLRenderingContext::CALLBACK);

      // Getters and setters
      InstanceReadonlyAccessor(isolate, prototype, "canvas", &WebGLRenderingContext::CanvasGetter);
      InstanceReadonlyAccessor(isolate,
                               prototype,
                               "drawingBufferWidth",
                               &WebGLRenderingContext::DrawingBufferWidthGetter);
      InstanceReadonlyAccessor(isolate,
                               prototype,
                               "drawingBufferHeight",
                               &WebGLRenderingContext::DrawingBufferHeightGetter);

      // Methods
      ADD_WEBGL1_METHOD("getContextAttributes", GetContextAttributes)
      ADD_WEBGL1_METHOD("isContextLost", IsContextLost)
      ADD_WEBGL1_METHOD("makeXRCompatible", MakeXRCompatible)
      ADD_WEBGL1_METHOD("setDefaultCoordHandedness", SetDefaultCoordHandedness)
      ADD_WEBGL1_METHOD("createProgram", CreateProgram)
      ADD_WEBGL1_METHOD("deleteProgram", DeleteProgram)
      ADD_WEBGL1_METHOD("linkProgram", LinkProgram)
      ADD_WEBGL1_METHOD("validateProgram", ValidateProgram)
      ADD_WEBGL1_METHOD("useProgram", UseProgram)
      ADD_WEBGL1_METHOD("bindAttribLocation", BindAttribLocation)
      ADD_WEBGL1_METHOD("getProgramParameter", GetProgramParameter)
      ADD_WEBGL1_METHOD("getProgramInfoLog", GetProgramInfoLog)
      ADD_WEBGL1_METHOD("attachShader", AttachShader)
      ADD_WEBGL1_METHOD("detachShader", DetachShader)
      ADD_WEBGL1_METHOD("createShader", CreateShader)
      ADD_WEBGL1_METHOD("deleteShader", DeleteShader)
      ADD_WEBGL1_METHOD("shaderSource", ShaderSource)
      ADD_WEBGL1_METHOD("compileShader", CompileShader)
      ADD_WEBGL1_METHOD("getShaderSource", GetShaderSource)
      ADD_WEBGL1_METHOD("getShaderParameter", GetShaderParameter)
      ADD_WEBGL1_METHOD("getShaderInfoLog", GetShaderInfoLog)
      ADD_WEBGL1_METHOD("createBuffer", CreateBuffer)
      ADD_WEBGL1_METHOD("deleteBuffer", DeleteBuffer)
      ADD_WEBGL1_METHOD("bindBuffer", BindBuffer)
      ADD_WEBGL1_METHOD("bufferData", BufferData)
      ADD_WEBGL1_METHOD("bufferSubData", BufferSubData)
      ADD_WEBGL1_METHOD("createFramebuffer", CreateFramebuffer)
      ADD_WEBGL1_METHOD("deleteFramebuffer", DeleteFramebuffer)
      ADD_WEBGL1_METHOD("bindFramebuffer", BindFramebuffer)
      ADD_WEBGL1_METHOD("framebufferRenderbuffer", FramebufferRenderbuffer)
      ADD_WEBGL1_METHOD("framebufferTexture2D", FramebufferTexture2D)
      ADD_WEBGL1_METHOD("checkFramebufferStatus", CheckFramebufferStatus)
      ADD_WEBGL1_METHOD("createRenderbuffer", CreateRenderbuffer)
      ADD_WEBGL1_METHOD("deleteRenderbuffer", DeleteRenderbuffer)
      ADD_WEBGL1_METHOD("bindRenderbuffer", BindRenderbuffer)
      ADD_WEBGL1_METHOD("renderbufferStorage", RenderbufferStorage)
      ADD_WEBGL1_METHOD("createTexture", CreateTexture)
      ADD_WEBGL1_METHOD("deleteTexture", DeleteTexture)
      ADD_WEBGL1_METHOD("bindTexture", BindTexture)
      ADD_WEBGL1_METHOD("texImage2D", TexImage2D)
      ADD_WEBGL1_METHOD("texSubImage2D", TexSubImage2D)
      ADD_WEBGL1_METHOD("copyTexImage2D", CopyTexImage2D)
      ADD_WEBGL1_METHOD("copyTexSubImage2D", CopyTexSubImage2D)
      ADD_WEBGL1_METHOD("texParameteri", TexParameteri)
      ADD_WEBGL1_METHOD("texParameterf", TexParameterf)
      ADD_WEBGL1_METHOD("activeTexture", ActiveTexture)
      ADD_WEBGL1_METHOD("generateMipmap", GenerateMipmap)
      ADD_WEBGL1_METHOD("enableVertexAttribArray", EnableVertexAttribArray)
      ADD_WEBGL1_METHOD("disableVertexAttribArray", DisableVertexAttribArray)
      ADD_WEBGL1_METHOD("vertexAttribPointer", VertexAttribPointer)
      ADD_WEBGL1_METHOD("vertexAttrib1f", VertexAttrib1f)
      ADD_WEBGL1_METHOD("vertexAttrib2f", VertexAttrib2f)
      ADD_WEBGL1_METHOD("vertexAttrib3f", VertexAttrib3f)
      ADD_WEBGL1_METHOD("vertexAttrib4f", VertexAttrib4f)
      ADD_WEBGL1_METHOD("getActiveAttrib", GetActiveAttrib)
      ADD_WEBGL1_METHOD("getActiveUniform", GetActiveUniform)
      ADD_WEBGL1_METHOD("getAttribLocation", GetAttribLocation)
      ADD_WEBGL1_METHOD("getUniformLocation", GetUniformLocation)
      ADD_WEBGL1_METHOD("uniform1f", Uniform1f)
      ADD_WEBGL1_METHOD("uniform1fv", Uniform1fv)
      ADD_WEBGL1_METHOD("uniform1i", Uniform1i)
      ADD_WEBGL1_METHOD("uniform1iv", Uniform1iv)
      ADD_WEBGL1_METHOD("uniform2f", Uniform2f)
      ADD_WEBGL1_METHOD("uniform2fv", Uniform2fv)
      ADD_WEBGL1_METHOD("uniform2i", Uniform2i)
      ADD_WEBGL1_METHOD("uniform2iv", Uniform2iv)
      ADD_WEBGL1_METHOD("uniform3f", Uniform3f)
      ADD_WEBGL1_METHOD("uniform3fv", Uniform3fv)
      ADD_WEBGL1_METHOD("uniform3i", Uniform3i)
      ADD_WEBGL1_METHOD("uniform3iv", Uniform3iv)
      ADD_WEBGL1_METHOD("uniform4f", Uniform4f)
      ADD_WEBGL1_METHOD("uniform4fv", Uniform4fv)
      ADD_WEBGL1_METHOD("uniform4i", Uniform4i)
      ADD_WEBGL1_METHOD("uniform4iv", Uniform4iv)
      ADD_WEBGL1_METHOD("uniformMatrix2fv", UniformMatrix2fv)
      ADD_WEBGL1_METHOD("uniformMatrix3fv", UniformMatrix3fv)
      ADD_WEBGL1_METHOD("uniformMatrix4fv", UniformMatrix4fv)
      ADD_WEBGL1_METHOD("drawArrays", DrawArrays)
      ADD_WEBGL1_METHOD("drawElements", DrawElements)
      ADD_WEBGL1_METHOD("hint", Hint)
      ADD_WEBGL1_METHOD("lineWidth", LineWidth)
      ADD_WEBGL1_METHOD("pixelStorei", PixelStorei)
      ADD_WEBGL1_METHOD("polygonOffset", PolygonOffset)
      ADD_WEBGL1_METHOD("viewport", Viewport)
      ADD_WEBGL1_METHOD("scissor", Scissor)
      ADD_WEBGL1_METHOD("clearColor", ClearColor)
      ADD_WEBGL1_METHOD("clearDepth", ClearDepth)
      ADD_WEBGL1_METHOD("clearStencil", ClearStencil)
      ADD_WEBGL1_METHOD("clear", Clear)
      ADD_WEBGL1_METHOD("depthMask", DepthMask)
      ADD_WEBGL1_METHOD("depthFunc", DepthFunc)
      ADD_WEBGL1_METHOD("depthRange", DepthRange)
      ADD_WEBGL1_METHOD("stencilFunc", StencilFunc)
      ADD_WEBGL1_METHOD("stencilFuncSeparate", StencilFuncSeparate)
      ADD_WEBGL1_METHOD("stencilMask", StencilMask)
      ADD_WEBGL1_METHOD("stencilMaskSeparate", StencilMaskSeparate)
      ADD_WEBGL1_METHOD("stencilOp", StencilOp)
      ADD_WEBGL1_METHOD("stencilOpSeparate", StencilOpSeparate)
      ADD_WEBGL1_METHOD("blendColor", BlendColor)
      ADD_WEBGL1_METHOD("blendEquation", BlendEquation)
      ADD_WEBGL1_METHOD("blendEquationSeparate", BlendEquationSeparate)
      ADD_WEBGL1_METHOD("blendFunc", BlendFunc)
      ADD_WEBGL1_METHOD("blendFuncSeparate", BlendFuncSeparate)
      ADD_WEBGL1_METHOD("colorMask", ColorMask)
      ADD_WEBGL1_METHOD("cullFace", CullFace)
      ADD_WEBGL1_METHOD("frontFace", FrontFace)
      ADD_WEBGL1_METHOD("enable", Enable)
      ADD_WEBGL1_METHOD("disable", Disable)
      ADD_WEBGL1_METHOD("getParameter", GetParameter)
      ADD_WEBGL1_METHOD("getShaderPrecisionFormat", GetShaderPrecisionFormat)
      ADD_WEBGL1_METHOD("getError", GetError)
      ADD_WEBGL1_METHOD("getExtension", GetExtension)
      ADD_WEBGL1_METHOD("getSupportedExtensions", GetSupportedExtensions)
    }

    // Method implementations - stubs for now, would need native implementation
    void WebGLRenderingContext::CanvasGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // TODO(yorkie): Return the actual canvas associated with this WebGL context.
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::DrawingBufferWidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto width = handle()->drawingBufferWidth();
      info.GetReturnValue().Set(Integer::New(isolate, width));
    }

    void WebGLRenderingContext::DrawingBufferHeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto height = handle()->drawingBufferHeight();
      info.GetReturnValue().Set(Integer::New(isolate, height));
    }

    void WebGLRenderingContext::GetContextAttributes(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      const auto &context_attribs = handle()->contextAttributes;
      auto obj = Object::New(isolate);

#define SET_CONTEXT_ATTRIB_BOOL(NAME)                            \
  obj->Set(context,                                              \
           String::NewFromUtf8(isolate, #NAME).ToLocalChecked(), \
           Boolean::New(isolate, context_attribs.NAME))          \
    .Check();

      SET_CONTEXT_ATTRIB_BOOL(alpha)
      SET_CONTEXT_ATTRIB_BOOL(antialias)
      SET_CONTEXT_ATTRIB_BOOL(depth)
      SET_CONTEXT_ATTRIB_BOOL(stencil)
      SET_CONTEXT_ATTRIB_BOOL(failIfMajorPerformanceCaveat)
      SET_CONTEXT_ATTRIB_BOOL(premultipliedAlpha)
      SET_CONTEXT_ATTRIB_BOOL(preserveDrawingBuffer)
      SET_CONTEXT_ATTRIB_BOOL(xrCompatible)
#undef SET_CONTEXT_ATTRIB_BOOL

      obj->Set(context,
               String::NewFromUtf8(isolate, "powerPreference").ToLocalChecked(),
               String::NewFromUtf8(isolate, context_attribs.powerPreference.c_str()).ToLocalChecked())
        .Check();
      info.GetReturnValue().Set(obj);
    }

    void WebGLRenderingContext::IsContextLost(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      bool is_lost = handle()->isContextLost();
      info.GetReturnValue().Set(Boolean::New(isolate, is_lost));
    }

    void WebGLRenderingContext::MakeXRCompatible(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      handle()->makeXRCompatible();
      auto deferred = Promise::Resolver::New(context).ToLocalChecked();
      deferred->Resolve(context, Undefined(isolate)).Check();
      info.GetReturnValue().Set(deferred->GetPromise());
    }

    void WebGLRenderingContext::SetDefaultCoordHandedness(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "setDefaultCoordHandedness", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "setDefaultCoordHandedness", "first argument must be a string")));
        return;
      }

      auto handedness_utf8 = String::Utf8Value(isolate, info[0]);
      string handedness = *handedness_utf8;
      if (handedness == "left")
        handle()->defaultCoordHandedness = commandbuffers::MatrixHandedness::MATRIX_LEFT_HANDED;
      else if (handedness == "right")
        handle()->defaultCoordHandedness = commandbuffers::MatrixHandedness::MATRIX_RIGHT_HANDED;
      else
      {
        isolate->ThrowException(Exception::TypeError(MakeMethodError(
          isolate, "setDefaultCoordHandedness", "first argument must be 'left' or 'right'")));
        return;
      }

      info.GetReturnValue().SetUndefined();
    }

    // State management
    void WebGLRenderingContext::Hint(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "hint", "2 arguments required, but only fewer present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "hint", "first argument must be a number")));
        return;
      }
      if (!info[1]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "hint", "second argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int target = info[0]->Int32Value(context).FromMaybe(0);
      int mode = info[1]->Int32Value(context).FromMaybe(0);

      handle()->hint(static_cast<client_graphics::WebGLHintTargetBehavior>(target),
                     static_cast<client_graphics::WebGLHintBehaviorMode>(mode));
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::LineWidth(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "lineWidth", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "lineWidth", "first argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      float width = static_cast<float>(info[0]->NumberValue(context).FromMaybe(1.0));
      handle()->lineWidth(width);
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::PixelStorei(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "pixelStorei", "2 arguments required, but only fewer present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "pixelStorei", "first argument must be a number")));
        return;
      }
      if (!info[1]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "pixelStorei", "second argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int pname = info[0]->Int32Value(context).FromMaybe(0);
      int param = info[1]->Int32Value(context).FromMaybe(0);
      handle()->pixelStorei(static_cast<client_graphics::WebGLPixelStorageParameterName>(pname), param);
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::PolygonOffset(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "polygonOffset", "2 arguments required, but only fewer present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "polygonOffset", "first argument must be a number")));
        return;
      }
      if (!info[1]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "polygonOffset", "second argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      float factor = static_cast<float>(info[0]->NumberValue(context).FromMaybe(0.0));
      float units = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
      handle()->polygonOffset(factor, units);
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::CullFace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "cullFace", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "cullFace", "first argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int mode = info[0]->Int32Value(context).FromMaybe(0);
      handle()->cullFace(mode);
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::FrontFace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "frontFace", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "frontFace", "first argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int mode = info[0]->Int32Value(context).FromMaybe(0);
      handle()->frontFace(mode);
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::ActiveTexture(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "activeTexture", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "activeTexture", "first argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int texture = info[0]->Int32Value(context).FromMaybe(0);
      handle()->activeTexture(client_graphics::WebGLTextureUnit(texture));
    }

    void WebGLRenderingContext::AttachShader(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "attachShader", "not implemented")));
    }

    void WebGLRenderingContext::DetachShader(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "detachShader", "not implemented")));
    }

    void WebGLRenderingContext::BindAttribLocation(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "bindAttribLocation", "not implemented")));
    }

    void WebGLRenderingContext::BindBuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "bindBuffer", "not implemented")));
    }

    void WebGLRenderingContext::BindFramebuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "bindFramebuffer", "not implemented")));
    }

    void WebGLRenderingContext::BindRenderbuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "bindRenderbuffer", "not implemented")));
    }

    void WebGLRenderingContext::BindTexture(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "bindTexture", "2 arguments required, but only fewer present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "bindTexture", "first argument must be a number")));
        return;
      }

      client_graphics::WebGLTextureTarget textureTarget;
      {
        int intValue = info[0]->Int32Value(context).FromMaybe(0);
        textureTarget = static_cast<client_graphics::WebGLTextureTarget>(intValue);
      }

      if (info[1]->IsNull())
      {
        handle()->bindTexture(textureTarget, nullptr);
      }
      else if (WebGLTexture::IsInstanceOf(isolate, info[1]))
      {
        auto textureObj = info[1]->ToObject(context).ToLocalChecked();
        auto textureBinding = WebGLTexture::Unwrap(isolate, textureObj);
        if (textureBinding == nullptr || !textureBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindTexture", "invalid WebGLTexture object")));
          return;
        }
        else
        {
          handle()->bindTexture(textureTarget, textureBinding->handle());
        }
      }
      else
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "bindTexture", "second argument must be a WebGLTexture or null")));
        return;
      }

      info.GetReturnValue().SetUndefined();
    }

    // Buffer operations
    void WebGLRenderingContext::BufferData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "bufferData", "not implemented")));
    }

    void WebGLRenderingContext::BufferSubData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "bufferSubData", "not implemented")));
    }

    // Framebuffer operations
    void WebGLRenderingContext::FramebufferRenderbuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "framebufferRenderbuffer", "not implemented")));
    }

    void WebGLRenderingContext::FramebufferTexture2D(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "framebufferTexture2D", "not implemented")));
    }

    void WebGLRenderingContext::CheckFramebufferStatus(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "checkFramebufferStatus", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "checkFramebufferStatus", "first argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int target = info[0]->Uint32Value(context).FromMaybe(0);
      int status = handle()->checkFramebufferStatus(static_cast<client_graphics::WebGLFramebufferBindingTarget>(target));
      info.GetReturnValue().Set(Integer::New(isolate, status));
    }

    void WebGLRenderingContext::Clear(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int mask = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->clear(mask);
      }
    }

    void WebGLRenderingContext::ClearColor(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4)
      {
        float red = static_cast<float>(info[0]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float green = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float blue = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float alpha = static_cast<float>(info[3]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->clearColor(red, green, blue, alpha);
      }
    }

    void WebGLRenderingContext::ClearDepth(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        float depth = static_cast<float>(info[0]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->clearDepth(depth);
      }
    }

    void WebGLRenderingContext::ClearStencil(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int s = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->clearStencil(s);
      }
    }

    void WebGLRenderingContext::ColorMask(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4)
      {
        bool red = info[0]->BooleanValue(isolate);
        bool green = info[1]->BooleanValue(isolate);
        bool blue = info[2]->BooleanValue(isolate);
        bool alpha = info[3]->BooleanValue(isolate);
        self->handle()->colorMask(red, green, blue, alpha);
      }
    }

    void WebGLRenderingContext::DepthMask(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1)
      {
        bool flag = info[0]->BooleanValue(isolate);
        self->handle()->depthMask(flag);
      }
    }

    void WebGLRenderingContext::DepthFunc(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int func = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->depthFunc(func);
      }
    }

    void WebGLRenderingContext::DepthRange(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2)
      {
        float zNear = static_cast<float>(info[0]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float zFar = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->depthRange(zNear, zFar);
      }
    }

    void WebGLRenderingContext::StencilFunc(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3)
      {
        int func = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int ref = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int mask = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->stencilFunc(func, ref, mask);
      }
    }

    void WebGLRenderingContext::StencilFuncSeparate(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4)
      {
        int face = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int func = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int ref = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int mask = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->stencilFuncSeparate(face, func, ref, mask);
      }
    }

    void WebGLRenderingContext::StencilMask(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int mask = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->stencilMask(mask);
      }
    }

    void WebGLRenderingContext::StencilMaskSeparate(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2)
      {
        int face = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int mask = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->stencilMaskSeparate(face, mask);
      }
    }

    void WebGLRenderingContext::StencilOp(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3)
      {
        int fail = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int zfail = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int zpass = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->stencilOp(fail, zfail, zpass);
      }
    }

    void WebGLRenderingContext::StencilOpSeparate(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4)
      {
        int face = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int fail = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int zfail = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int zpass = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->stencilOpSeparate(face, fail, zfail, zpass);
      }
    }

    void WebGLRenderingContext::BlendColor(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4)
      {
        float red = static_cast<float>(info[0]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float green = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float blue = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float alpha = static_cast<float>(info[3]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->blendColor(red, green, blue, alpha);
      }
    }

    void WebGLRenderingContext::BlendEquation(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int mode = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->blendEquation(mode);
      }
    }

    void WebGLRenderingContext::BlendEquationSeparate(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsNumber() && info[1]->IsNumber())
      {
        int modeRGB = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int modeAlpha = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->blendEquationSeparate(modeRGB, modeAlpha);
      }
    }

    void WebGLRenderingContext::BlendFunc(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsNumber() && info[1]->IsNumber())
      {
        int sfactor = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int dfactor = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->blendFunc(sfactor, dfactor);
      }
    }

    void WebGLRenderingContext::BlendFuncSeparate(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
      {
        int srcRGB = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int dstRGB = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int srcAlpha = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int dstAlpha = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
      }
    }

    void WebGLRenderingContext::RenderbufferStorage(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int internalformat = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int height = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        // self->handle()->renderbufferStorage(target, internalformat, width, height);
      }
    }

    void WebGLRenderingContext::CompileShader(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "compileShader", "1 argument required, but only 0 present")));
        return;
      }
      if (!WebGLShader::IsInstanceOf(isolate, info[0]))
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "compileShader", "first argument must be a WebGLShader")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      auto shaderObj = info[0]->ToObject(context).ToLocalChecked();
      auto shader = WebGLShader::Unwrap(isolate, shaderObj);
      if (shader == nullptr || !shader->hasData())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "compileShader", "invalid WebGLShader object")));
        return;
      }

      handle()->compileShader(shader->handle());
      info.GetReturnValue().SetUndefined();
    }

    void WebGLRenderingContext::CreateProgram(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto newProgram = handle()->createProgram();
      info.GetReturnValue().Set(WebGLProgram::NewInstance(isolate, newProgram));
    }

    void WebGLRenderingContext::CreateShader(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "createShader", "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "createShader", "first argument must be a number")));
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      int type = info[0]->Int32Value(context).FromMaybe(0);
      auto newShader = handle()->createShader(static_cast<client_graphics::WebGLShaderType>(type));
      info.GetReturnValue().Set(WebGLShader::NewInstance(isolate, newShader));
    }

    void WebGLRenderingContext::CreateBuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto newBuffer = handle()->createBuffer();
      info.GetReturnValue().Set(WebGLBuffer::NewInstance(isolate, newBuffer));
    }

    void WebGLRenderingContext::CreateFramebuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto newFramebuffer = handle()->createFramebuffer();
      info.GetReturnValue().Set(WebGLFramebuffer::NewInstance(isolate, newFramebuffer));
    }

    void WebGLRenderingContext::CreateRenderbuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto newRenderbuffer = handle()->createRenderbuffer();
      info.GetReturnValue().Set(WebGLRenderbuffer::NewInstance(isolate, newRenderbuffer));
    }

    void WebGLRenderingContext::CreateTexture(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto newTexture = handle()->createTexture();
      info.GetReturnValue().Set(WebGLTexture::NewInstance(isolate, newTexture));
    }

    void WebGLRenderingContext::DeleteBuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto buffer = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (buffer)
        {
          // self->handle()->deleteBuffer(buffer);
        }
      }
    }

    void WebGLRenderingContext::DeleteFramebuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto framebuffer = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (framebuffer)
        {
          // self->handle()->deleteFramebuffer(framebuffer);
        }
      }
    }

    void WebGLRenderingContext::DeleteProgram(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (program)
        {
          // self->handle()->deleteProgram(program);
        }
      }
    }

    void WebGLRenderingContext::DeleteRenderbuffer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto renderbuffer = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (renderbuffer)
        {
          // self->handle()->deleteRenderbuffer(renderbuffer);
        }
      }
    }

    void WebGLRenderingContext::DeleteShader(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto shader = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (shader)
        {
          // self->handle()->deleteShader(shader);
        }
      }
    }

    void WebGLRenderingContext::DeleteTexture(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto texture = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (texture)
        {
          // self->handle()->deleteTexture(texture);
        }
      }
    }

    void WebGLRenderingContext::DrawArrays(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        int mode = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int first = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int count = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        // self->handle()->drawArrays(mode, first, count);
      }
    }

    void WebGLRenderingContext::DrawElements(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsObject())
      {
        int mode = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int count = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int type = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        auto indices = info[3]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
        // self->handle()->drawElements(mode, count, type, indices);
      }
    }

    void WebGLRenderingContext::Enable(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int cap = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->enable(cap);
      }
    }

    void WebGLRenderingContext::Disable(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int cap = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->disable(cap);
      }
    }

    void WebGLRenderingContext::EnableVertexAttribArray(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->enableVertexAttribArray(index);
      }
    }

    void WebGLRenderingContext::DisableVertexAttribArray(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->disableVertexAttribArray(index);
      }
    }

    void WebGLRenderingContext::GetActiveAttrib(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsNumber())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int index = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (program)
        {
          // auto attrib = self->handle()->getActiveAttrib(program, index);
          // Wrap and return the attribute
        }
      }
    }

    void WebGLRenderingContext::GetActiveUniform(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsNumber())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int index = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (program)
        {
          // auto uniform = self->handle()->getActiveUniform(program, index);
          // Wrap and return the uniform
        }
      }
    }

    void WebGLRenderingContext::GetAttribLocation(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsString())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        String::Utf8Value name(isolate, info[1]);
        if (program && *name)
        {
          // int location = self->handle()->getAttribLocation(program, *name);
          // info.GetReturnValue().Set(Integer::New(isolate, location));
        }
      }
    }

    void WebGLRenderingContext::GetUniformLocation(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsString())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        String::Utf8Value name(isolate, info[1]);
        if (program && *name)
        {
          // auto location = self->handle()->getUniformLocation(program, *name);
          // Wrap and return the location
        }
      }
    }

    void WebGLRenderingContext::LinkProgram(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (program)
        {
          // self->handle()->linkProgram(program);
        }
      }
    }

    void WebGLRenderingContext::UseProgram(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (program)
        {
          // self->handle()->useProgram(program);
        }
      }
    }

    void WebGLRenderingContext::ValidateProgram(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (program)
        {
          // self->handle()->validateProgram(program);
        }
      }
    }

    void WebGLRenderingContext::ShaderSource(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsString())
      {
        auto shader = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        String::Utf8Value source(isolate, info[1]);
        if (shader && *source)
        {
          // self->handle()->shaderSource(shader, *source);
        }
      }
    }

    void WebGLRenderingContext::GetShaderSource(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto shader = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (shader)
        {
          // std::string source = self->handle()->getShaderSource(shader);
          // info.GetReturnValue().Set(String::NewFromUtf8(isolate, source.c_str()).ToLocalChecked());
        }
      }
    }

    void WebGLRenderingContext::TexImage2D(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 6 && info[0]->IsNumber() && info[1]->IsNumber() &&
          info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int level = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int internalformat = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int height = info[4]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int border = info[5]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);

        // if (info.Length() >= 7 && (info[6]->IsTypedArray() || info[6]->IsArrayBuffer()))
        // {
        //   // Handle pixel data
        //   self->handle()->texImage2D(target, level, internalformat, width, height, border, info[6]);
        // }
        // else
        // {
        //   self->handle()->texImage2D(target, level, internalformat, width, height, border);
        // }
      }
    }

    void WebGLRenderingContext::TexSubImage2D(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 7 && info[0]->IsNumber() && info[1]->IsNumber() &&
          info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() &&
          info[6]->IsTypedArray())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int level = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int xoffset = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int yoffset = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[4]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int height = info[5]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);

        // self->handle()->texSubImage2D(target, level, xoffset, yoffset, width, height, info[6]);
      }
    }

    void WebGLRenderingContext::TexParameterf(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int pname = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        float param = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        // self->handle()->texParameterf(target, pname, param);
      }
    }

    void WebGLRenderingContext::TexParameteri(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int pname = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int param = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        // self->handle()->texParameteri(target, pname, param);
      }
    }

    void WebGLRenderingContext::CopyTexImage2D(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 8 && info[0]->IsNumber() && info[1]->IsNumber() &&
          info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() &&
          info[6]->IsNumber() && info[7]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int level = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int internalformat = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int x = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[4]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[5]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int height = info[6]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int border = info[7]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        // self->handle()->copyTexImage2D(target, level, internalformat, x, y, width, height, border);
      }
    }

    void WebGLRenderingContext::CopyTexSubImage2D(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 7 && info[0]->IsNumber() && info[1]->IsNumber() &&
          info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() &&
          info[6]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int level = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int xoffset = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int yoffset = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int x = info[4]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[5]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[6]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        // self->handle()->copyTexSubImage2D(target, level, xoffset, yoffset, x, y, width);
      }
    }

    void WebGLRenderingContext::GenerateMipmap(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsNumber())
      {
        int target = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        // self->handle()->generateMipmap(target);
      }
    }

    void WebGLRenderingContext::Uniform1f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        if (location)
        {
          // self->handle()->uniform1f(location, x);
        }
      }
    }

    void WebGLRenderingContext::Uniform1i(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int x = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (location)
        {
          // self->handle()->uniform1i(location, x);
        }
      }
    }

    void WebGLRenderingContext::Uniform2f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        if (location)
        {
          // self->handle()->uniform2f(location, x, y);
        }
      }
    }

    void WebGLRenderingContext::Uniform2i(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int x = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (location)
        {
          // self->handle()->uniform2i(location, x, y);
        }
      }
    }

    void WebGLRenderingContext::Uniform3f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float z = static_cast<float>(info[3]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        if (location)
        {
          // self->handle()->uniform3f(location, x, y, z);
        }
      }
    }

    void WebGLRenderingContext::Uniform3i(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int x = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int z = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (location)
        {
          // self->handle()->uniform3i(location, x, y, z);
        }
      }
    }

    void WebGLRenderingContext::Uniform4f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 5 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float z = static_cast<float>(info[3]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float w = static_cast<float>(info[4]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        if (location)
        {
          // self->handle()->uniform4f(location, x, y, z, w);
        }
      }
    }

    void WebGLRenderingContext::Uniform4i(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 5 && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int x = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int z = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int w = info[4]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (location)
        {
          // self->handle()->uniform4i(location, x, y, z, w);
        }
      }
    }

    void WebGLRenderingContext::Uniform1fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform1fv(location, static_cast<float *>(contents.Data()), data->Length());
        }
      }
    }

    void WebGLRenderingContext::Uniform1iv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Int32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform1iv(location, static_cast<int *>(contents.Data()), data->Length());
        }
      }
    }

    void WebGLRenderingContext::Uniform2fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform2fv(location, static_cast<float *>(contents.Data()), data->Length() / 2);
        }
      }
    }

    void WebGLRenderingContext::Uniform2iv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Int32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform2iv(location, static_cast<int *>(contents.Data()), data->Length() / 2);
        }
      }
    }

    void WebGLRenderingContext::Uniform3fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform3fv(location, static_cast<float *>(contents.Data()), data->Length() / 3);
        }
      }
    }

    void WebGLRenderingContext::Uniform3iv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Int32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform3iv(location, static_cast<int *>(contents.Data()), data->Length() / 3);
        }
      }
    }

    void WebGLRenderingContext::Uniform4fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform4fv(location, static_cast<float *>(contents.Data()), data->Length() / 4);
        }
      }
    }

    void WebGLRenderingContext::Uniform4iv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        auto data = info[1].As<Int32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniform4iv(location, static_cast<int *>(contents.Data()), data->Length() / 4);
        }
      }
    }

    void WebGLRenderingContext::UniformMatrix2fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsObject() && info[1]->IsBoolean() && info[2]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        bool transpose = info[1]->BooleanValue(isolate);
        auto data = info[2].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniformMatrix2fv(location, transpose, static_cast<float *>(contents.Data()), data->Length() / 4);
        }
      }
    }

    void WebGLRenderingContext::UniformMatrix3fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsObject() && info[1]->IsBoolean() && info[2]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        bool transpose = info[1]->BooleanValue(isolate);
        auto data = info[2].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniformMatrix3fv(location, transpose, static_cast<float *>(contents.Data()), data->Length() / 9);
        }
      }
    }

    void WebGLRenderingContext::UniformMatrix4fv(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsObject() && info[1]->IsBoolean() && info[2]->IsTypedArray())
      {
        auto location = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        bool transpose = info[1]->BooleanValue(isolate);
        auto data = info[2].As<Float32Array>();
        if (location && !data.IsEmpty())
        {
          // ArrayBuffer::Contents contents = data->Buffer()->GetContents();
          // self->handle()->uniformMatrix4fv(location, transpose, static_cast<float *>(contents.Data()), data->Length() / 16);
        }
      }
    }

    void WebGLRenderingContext::VertexAttrib1f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsNumber() && info[1]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->vertexAttrib1f(index, x);
      }
    }

    void WebGLRenderingContext::VertexAttrib2f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 3 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->vertexAttrib2f(index, x, y);
      }
    }

    void WebGLRenderingContext::VertexAttrib3f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float z = static_cast<float>(info[3]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->vertexAttrib3f(index, x, y, z);
      }
    }

    void WebGLRenderingContext::VertexAttrib4f(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 5 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float z = static_cast<float>(info[3]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        float w = static_cast<float>(info[4]->NumberValue(isolate->GetCurrentContext()).FromMaybe(0.0));
        self->handle()->vertexAttrib4f(index, x, y, z, w);
      }
    }

    void WebGLRenderingContext::VertexAttribPointer(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 6 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsBoolean() && info[4]->IsNumber() && info[5]->IsNumber())
      {
        int index = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int size = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int type = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        bool normalized = info[3]->BooleanValue(isolate);
        int stride = info[4]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int offset = info[5]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->vertexAttribPointer(index, size, type, normalized, stride, offset);
      }
    }

    void WebGLRenderingContext::Viewport(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
      {
        int x = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int height = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->viewport(x, y, width, height);
      }
    }

    void WebGLRenderingContext::Scissor(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 4 && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
      {
        int x = info[0]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int y = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int width = info[2]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        int height = info[3]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        self->handle()->scissor(x, y, width, height);
      }
    }

    void WebGLRenderingContext::GetError(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle())
      {
        int error = self->handle()->getError();
        info.GetReturnValue().Set(Integer::New(isolate, error));
      }
    }

    void WebGLRenderingContext::GetParameter(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1 || !info[0]->IsNumber())
      {
        string msg = "Requires 1 argument, but only " + to_string(info.Length()) + " present.";
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getParameter", msg.c_str())));
        info.GetReturnValue().SetUndefined();
        return;
      }

      Local<Context> context = isolate->GetCurrentContext();
      Local<Value> jsValue = Undefined(isolate);

      int pname = info[0]->ToNumber(context).ToLocalChecked()->Value();
      switch (pname)
      {
      /**
       * GLint
       */
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
      /**
       * GLenum
       */
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
        auto value = handle()->getParameter(static_cast<client_graphics::WebGLIntegerParameterName>(pname));
        jsValue = Integer::New(isolate, value);
        break;
      }
      /**
       * GLboolean
       */
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
        jsValue = Boolean::New(isolate,
                               handle()->getParameter(static_cast<client_graphics::WebGLBooleanParameterName>(pname)));
        break;
      }
      /**
       * GLfloat[]
       */
      case WEBGL_VIEWPORT:
      case WEBGL_SCISSOR_BOX:
      {
        auto values = handle()->getParameter(static_cast<client_graphics::WebGLFloatArrayParameterName>(pname));
        auto arraybuffer = ArrayBuffer::New(isolate, sizeof(float) * values.size());
        for (size_t i = 0; i < values.size(); i++)
        {
          arraybuffer->Set(context, i, Number::New(isolate, values[i])).Check();
        }
        jsValue = Float32Array::New(arraybuffer, 0, values.size());
        break;
      }
      /**
       * GLstring
       */
      case WEBGL_RENDERER:
      case WEBGL_SHADING_LANGUAGE_VERSION:
      case WEBGL_VENDOR:
      case WEBGL_VERSION:
      {
        auto value = handle()->getParameter(static_cast<client_graphics::WebGLStringParameterName>(pname));
        jsValue = String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked();
        break;
      }
      default:
        cerr << "WebGLRenderingContext::GetParameter: Unhandled pname " << pname << endl;
        break;
      }

      // Return the value
      info.GetReturnValue().Set(jsValue);
    }

    void WebGLRenderingContext::GetProgramParameter(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsNumber())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int pname = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (program)
        {
          // auto value = self->handle()->getProgramParameter(program, pname);
          // Wrap and return the value
        }
      }
    }

    void WebGLRenderingContext::GetShaderParameter(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 2 && info[0]->IsObject() && info[1]->IsNumber())
      {
        auto shader = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        int pname = info[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0);
        if (shader)
        {
          // auto value = self->handle()->getShaderParameter(shader, pname);
          // Wrap and return the value
        }
      }
    }

    void WebGLRenderingContext::GetShaderPrecisionFormat(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 2)
      {
        string msg = "Requires 2 arguments, but only " + to_string(info.Length()) + " present.";
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getShaderPrecisionFormat", msg.c_str())));
        info.GetReturnValue().SetUndefined();
        return;
      }

      int shader_type = info[0]->ToNumber(context).ToLocalChecked()->Value();
      int precision_type = info[1]->ToNumber(context).ToLocalChecked()->Value();
      try
      {
        const auto &precision_format = handle()->getShaderPrecisionFormat(shader_type, precision_type);
        info.GetReturnValue().Set(WebGLShaderPrecisionFormat::NewInstance(isolate, precision_format));
      }
      catch (const std::exception &e)
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getShaderPrecisionFormat", e.what())));
        info.GetReturnValue().SetUndefined();
        return;
      }
    }

    void WebGLRenderingContext::GetProgramInfoLog(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto program = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (program)
        {
          // std::string log = self->handle()->getProgramInfoLog(program);
          // info.GetReturnValue().Set(String::NewFromUtf8(isolate, log.c_str()).ToLocalChecked());
        }
      }
    }

    void WebGLRenderingContext::GetShaderInfoLog(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto *self = Unwrap(isolate, info.This());
      if (self && self->handle() && info.Length() >= 1 && info[0]->IsObject())
      {
        auto shader = Unwrap(isolate, info[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (shader)
        {
          // std::string log = self->handle()->getShaderInfoLog(shader);
          // info.GetReturnValue().Set(String::NewFromUtf8(isolate, log.c_str()).ToLocalChecked());
        }
      }
    }

    void WebGLRenderingContext::GetExtension(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1 || !info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getExtension", "First argument must be a string")));
        return;
      }

      auto name_utf8 = String::Utf8Value(isolate, info[0]);
      string name = *name_utf8 ? *name_utf8 : "";
      info.GetReturnValue().Set(WebGLExtensions::GetExtension(isolate, name));
    }

    void WebGLRenderingContext::GetSupportedExtensions(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      auto extensions = Array::New(isolate);
      {
        int index = 0;
        const auto &list = handle()->getSupportedExtensions();
        for (const auto &name : list)
        {
          if (WebGLExtensions::HasExtension(name))
          {
            auto value = String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked();
            extensions->Set(context, index++, value).Check();
          }
        }
      }
      info.GetReturnValue().Set(extensions);
    }
  }
}
