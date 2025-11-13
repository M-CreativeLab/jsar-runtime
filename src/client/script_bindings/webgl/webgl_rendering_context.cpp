#include <set>
#include <client/script_bindings/canvas/image_source.hpp>
#include "./webgl_rendering_context.hpp"
#include "./active_info.hpp"
#include "./uniform_location.hpp"
#include "./shader.hpp"
#include "./program.hpp"
#include "./vertex_array.hpp"
#include "./texture.hpp"
#include "./buffer.hpp"
#include "./renderbuffer.hpp"
#include "./framebuffer.hpp"
#include "./extensions/all.hpp"

namespace endor
{
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
        auto instance = tpl->InstanceTemplate();
        auto prototype = tpl->PrototypeTemplate();

#define ADD_WEBGL1_METHOD(NAME, CALLBACK) \
  InstanceMethod(isolate,                 \
                 prototype,               \
                 NAME,                    \
                 &WebGLRenderingContext::CALLBACK);

        // Getters and setters
        InstanceReadonlyAccessor(isolate, instance, "canvas", &WebGLRenderingContext::CanvasGetter);
        InstanceReadonlyAccessor(isolate,
                                 instance,
                                 "drawingBufferWidth",
                                 &WebGLRenderingContext::DrawingBufferWidthGetter);
        InstanceReadonlyAccessor(isolate,
                                 instance,
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
            MakeMethodArgTypeError(isolate, "pixelStorei", "pname", "number", info[0])));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int pname = info[0]->Int32Value(context).FromMaybe(0);
        int param = info[1]->ToNumber(context).ToLocalChecked()->Int32Value(context).FromMaybe(0);
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
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "attachShader", "2 arguments required, but only fewer present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "attachShader", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[1]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "attachShader", "second argument must be a WebGLShader object")));
          return;
        }

        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "attachShader", "invalid WebGLProgram object")));
          return;
        }

        auto shaderObj = info[1]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "attachShader", "invalid WebGLShader object")));
          return;
        }

        handle()->attachShader(programBinding->handle(), shaderBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DetachShader(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "detachShader", "2 arguments required, but only fewer present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "detachShader", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[1]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "detachShader", "second argument must be a WebGLShader object")));
          return;
        }

        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "detachShader", "invalid WebGLProgram object")));
          return;
        }

        auto shaderObj = info[1]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "detachShader", "invalid WebGLShader object")));
          return;
        }

        handle()->detachShader(programBinding->handle(), shaderBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BindAttribLocation(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindAttribLocation", "3 arguments required, but only fewer present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindAttribLocation", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindAttribLocation", "second argument must be a number")));
          return;
        }
        if (!info[2]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindAttribLocation", "third argument must be a string")));
          return;
        }

        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindAttribLocation", "invalid WebGLProgram object")));
          return;
        }

        int index = info[1]->Int32Value(context).FromMaybe(0);
        auto name_utf8 = String::Utf8Value(isolate, info[2]);
        string name = *name_utf8;
        handle()->bindAttribLocation(programBinding->handle(), index, name);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BindBuffer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindBuffer", "2 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindBuffer", "first argument must be a number")));
          return;
        }

        client_graphics::WebGLBufferBindingTarget bufferTarget;
        {
          int intValue = info[0]->Int32Value(context).FromMaybe(0);
          bufferTarget = static_cast<client_graphics::WebGLBufferBindingTarget>(intValue);
        }

        if (info[1]->IsNull())
        {
          handle()->bindBuffer(bufferTarget, nullptr);
        }
        else if (WebGLBuffer::IsInstanceOf(isolate, info[1]))
        {
          auto bufferObj = info[1]->ToObject(context).ToLocalChecked();
          auto bufferBinding = WebGLBuffer::Unwrap(isolate, bufferObj);
          if (bufferBinding == nullptr || !bufferBinding->hasData())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "bindBuffer", "invalid WebGLBuffer object")));
            return;
          }
          else
          {
            handle()->bindBuffer(bufferTarget, bufferBinding->handle());
          }
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindBuffer", "second argument must be a WebGLBuffer or null")));
          return;
        }

        // Always return undefined
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BindFramebuffer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindFramebuffer", "2 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindFramebuffer", "first argument must be a number")));
          return;
        }

        client_graphics::WebGLFramebufferBindingTarget framebufferTarget;
        {
          int intValue = info[0]->Int32Value(context).FromMaybe(0);
          framebufferTarget = static_cast<client_graphics::WebGLFramebufferBindingTarget>(intValue);
        }

        if (info[1]->IsNull())
        {
          handle()->bindFramebuffer(framebufferTarget, nullptr);
        }
        else if (WebGLFramebuffer::IsInstanceOf(isolate, info[1]))
        {
          auto framebufferObj = info[1]->ToObject(context).ToLocalChecked();
          auto framebufferBinding = WebGLFramebuffer::Unwrap(isolate, framebufferObj);
          if (framebufferBinding == nullptr || !framebufferBinding->hasData())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "bindFramebuffer", "invalid WebGLFramebuffer object")));
            return;
          }
          else
          {
            handle()->bindFramebuffer(framebufferTarget, framebufferBinding->handle());
          }
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindFramebuffer", "second argument must be a WebGLFramebuffer or null")));
          return;
        }

        // Always return undefined
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BindRenderbuffer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindRenderbuffer", "2 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindRenderbuffer", "first argument must be a number")));
          return;
        }

        client_graphics::WebGLRenderbufferBindingTarget renderbufferTarget;
        {
          int intValue = info[0]->Int32Value(context).FromMaybe(0);
          renderbufferTarget = static_cast<client_graphics::WebGLRenderbufferBindingTarget>(intValue);
        }

        if (info[1]->IsNull())
        {
          handle()->bindRenderbuffer(renderbufferTarget, nullptr);
        }
        else if (WebGLRenderbuffer::IsInstanceOf(isolate, info[1]))
        {
          auto renderbufferObj = info[1]->ToObject(context).ToLocalChecked();
          auto renderbufferBinding = WebGLRenderbuffer::Unwrap(isolate, renderbufferObj);
          if (renderbufferBinding == nullptr || !renderbufferBinding->hasData())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "bindRenderbuffer", "invalid WebGLRenderbuffer object")));
            return;
          }
          else
          {
            handle()->bindRenderbuffer(renderbufferTarget, renderbufferBinding->handle());
          }
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindRenderbuffer", "second argument must be a WebGLRenderbuffer or null")));
          return;
        }

        // Always return undefined
        info.GetReturnValue().SetUndefined();
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
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 3)
        {
          auto msg = "3 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "bufferData", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bufferData", "first argument(target) must be a number")));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bufferData", "third argument(usage) must be a number")));
        }

        client_graphics::WebGLBufferBindingTarget bufferTarget;
        {
          uint32_t value = info[0]->Uint32Value(context).FromMaybe(0);
          bufferTarget = static_cast<client_graphics::WebGLBufferBindingTarget>(value);
        }

        client_graphics::WebGLBufferUsage usage;
        {
          uint32_t value = info[2]->Uint32Value(context).FromMaybe(0);
          usage = static_cast<client_graphics::WebGLBufferUsage>(value);
        }

        auto secondArg = info[1];
        if (secondArg->IsNumber())
        {
          uint32_t size = secondArg->Uint32Value(context).FromMaybe(0);
          handle()->bufferData(bufferTarget, size, usage);
        }
        else if (secondArg->IsNull())
        {
          handle()->bufferData(bufferTarget, 0, usage);
        }
        else if (secondArg->IsArrayBufferView())
        {
          auto arrayBufferView = secondArg.As<ArrayBufferView>();
          auto arrayBuffer = arrayBufferView->Buffer();
          if (arrayBuffer.IsEmpty())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "bufferData", "invalid ArrayBufferView")));
            return;
          }
          auto data = static_cast<uint8_t *>(arrayBuffer->Data()) + arrayBufferView->ByteOffset();
          size_t size = arrayBufferView->ByteLength();
          handle()->bufferData(bufferTarget, size, data, usage);
        }
        else if (secondArg->IsArray())
        {
          auto jsArray = secondArg.As<Array>();
          size_t length = jsArray->Length();
          std::vector<float> data(length);
          for (size_t i = 0; i < length; ++i)
          {
            auto element = jsArray->Get(context, i).ToLocalChecked();
            data[i] = static_cast<float>(element->NumberValue(context).FromMaybe(0.0));
          }
          handle()->bufferData(bufferTarget, data.size() * sizeof(float), data.data(), usage);
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bufferData", "second argument must be a number or ArrayBufferView, Array or null")));
          return;
        }

        // Returns undefined
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BufferSubData(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          auto msg = "2 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "bufferSubData", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bufferSubData", "first argument(target) must be a number")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bufferSubData", "second argument(offset) must be a number")));
          return;
        }

        client_graphics::WebGLBufferBindingTarget bufferTarget;
        {
          uint32_t value = info[0]->Uint32Value(context).FromMaybe(0);
          bufferTarget = static_cast<client_graphics::WebGLBufferBindingTarget>(value);
        }
        uint32_t offset = info[1]->Uint32Value(context).FromMaybe(0);

        if (info.Length() == 2)
        {
          handle()->bufferSubData(bufferTarget, offset, 0, nullptr);
        }
        else
        {
          assert(info.Length() >= 3);
          auto srcDataValue = info[2];
          if (srcDataValue->IsNull())
          {
            handle()->bufferSubData(bufferTarget, offset, 0, nullptr);
          }
          else if (srcDataValue->IsArrayBufferView())
          {
            auto arrayBufferView = srcDataValue.As<ArrayBufferView>();
            auto arrayBuffer = arrayBufferView->Buffer();
            if (arrayBuffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "bufferSubData", "invalid ArrayBufferView")));
              return;
            }
            auto data = static_cast<uint8_t *>(arrayBuffer->Data()) + arrayBufferView->ByteOffset();
            size_t size = arrayBufferView->ByteLength();
            handle()->bufferSubData(bufferTarget, offset, size, data);
          }
          else if (srcDataValue->IsArray())
          {
            auto jsArray = srcDataValue.As<Array>();
            size_t length = jsArray->Length();
            std::vector<float> data(length);
            for (size_t i = 0; i < length; ++i)
            {
              auto element = jsArray->Get(context, i).ToLocalChecked();
              data[i] = static_cast<float>(element->NumberValue(context).FromMaybe(0.0));
            }
            handle()->bufferSubData(bufferTarget, offset, data.size() * sizeof(float), data.data());
          }
          else
          {
            isolate->ThrowException(Exception::TypeError(MakeMethodError(
              isolate, "bufferSubData", "third argument must be an ArrayBufferView, Array or null")));
            return;
          }
        }

        // Returns undefined
        info.GetReturnValue().SetUndefined();
      }

      // Framebuffer operations
      void WebGLRenderingContext::FramebufferRenderbuffer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferRenderbuffer", "4 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferRenderbuffer", "first argument(target) must be a number")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferRenderbuffer", "second argument(attachment) must be a number")));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferRenderbuffer", "third argument(renderbuffertarget) must be a number")));
          return;
        }
        if (!WebGLRenderbuffer::IsInstanceOf(isolate, info[3]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferRenderbuffer", "fourth argument must be a WebGLRenderbuffer or null")));
          return;
        }

        client_graphics::WebGLFramebufferBindingTarget target;
        {
          int value = info[0]->Int32Value(context).FromMaybe(0);
          target = static_cast<client_graphics::WebGLFramebufferBindingTarget>(value);
        }
        client_graphics::WebGLFramebufferAttachment attachment;
        {
          int value = info[1]->Int32Value(context).FromMaybe(0);
          attachment = static_cast<client_graphics::WebGLFramebufferAttachment>(value);
        }
        client_graphics::WebGLRenderbufferBindingTarget renderbufferTarget;
        {
          int value = info[2]->Int32Value(context).FromMaybe(0);
          renderbufferTarget = static_cast<client_graphics::WebGLRenderbufferBindingTarget>(value);
        }

        auto renderbufferObj = info[3]->ToObject(context).ToLocalChecked();
        auto renderbufferBinding = WebGLRenderbuffer::Unwrap(isolate, renderbufferObj);
        if (renderbufferBinding == nullptr || !renderbufferBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferRenderbuffer", "invalid WebGLRenderbuffer object")));
          return;
        }

        handle()->framebufferRenderbuffer(target, attachment, renderbufferTarget, renderbufferBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::FramebufferTexture2D(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferTexture2D", "5 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferTexture2D", "first argument(target) must be a number")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferTexture2D", "second argument(attachment) must be a number")));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferTexture2D", "third argument(textarget) must be a number")));
          return;
        }
        if (!WebGLTexture::IsInstanceOf(isolate, info[3]) && !info[3]->IsNull())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferTexture2D", "fourth argument must be a WebGLTexture or null")));
          return;
        }
        if (!info[4]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "framebufferTexture2D", "fifth argument(level) must be a number")));
          return;
        }

        client_graphics::WebGLFramebufferBindingTarget target;
        {
          int value = info[0]->Int32Value(context).FromMaybe(0);
          target = static_cast<client_graphics::WebGLFramebufferBindingTarget>(value);
        }
        client_graphics::WebGLFramebufferAttachment attachment;
        {
          int value = info[1]->Int32Value(context).FromMaybe(0);
          attachment = static_cast<client_graphics::WebGLFramebufferAttachment>(value);
        }
        client_graphics::WebGLTexture2DTarget textarget;
        {
          int value = info[2]->Int32Value(context).FromMaybe(0);
          textarget = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int level = info[4]->Int32Value(context).FromMaybe(0);

        if (info[3]->IsNull())
        {
          handle()->framebufferTexture2D(target, attachment, textarget, nullptr, level);
        }
        else
        {
          auto textureObj = info[3]->ToObject(context).ToLocalChecked();
          auto textureBinding = WebGLTexture::Unwrap(isolate, textureObj);
          if (textureBinding == nullptr || !textureBinding->hasData())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "framebufferTexture2D", "invalid WebGLTexture object")));
            return;
          }
          handle()->framebufferTexture2D(target, attachment, textarget, textureBinding->handle(), level);
        }
        info.GetReturnValue().SetUndefined();
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
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clear", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clear", "first argument(mask) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        uint32_t mask = info[0]->Uint32Value(context).FromMaybe(0);
        handle()->clear(mask);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::ClearColor(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clearColor", "4 arguments required, but only fewer present")));
          return;
        }
        for (int i = 0; i < 4; ++i)
        {
          if (!info[i]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "clearColor", "all arguments must be numbers")));
            return;
          }
        }

        Local<Context> context = isolate->GetCurrentContext();
        float r = static_cast<float>(info[0]->NumberValue(context).FromMaybe(0.0));
        float g = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float b = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        float a = static_cast<float>(info[3]->NumberValue(context).FromMaybe(0.0));
        handle()->clearColor(r, g, b, a);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::ClearDepth(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clearDepth", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clearDepth", "first argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        float depth = static_cast<float>(info[0]->NumberValue(context).FromMaybe(0.0));
        handle()->clearDepth(depth);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::ClearStencil(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clearStencil", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "clearStencil", "first argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int s = info[0]->Int32Value(context).FromMaybe(0);
        handle()->clearStencil(s);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::ColorMask(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "colorMask", "4 arguments required, but only fewer present")));
          return;
        }
        for (int i = 0; i < 4; ++i)
        {
          if (!info[i]->IsBoolean())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "colorMask", "all arguments must be boolean")));
            return;
          }
        }

        Local<Context> context = isolate->GetCurrentContext();
        bool r = info[0]->BooleanValue(isolate);
        bool g = info[1]->BooleanValue(isolate);
        bool b = info[2]->BooleanValue(isolate);
        bool a = info[3]->BooleanValue(isolate);
        handle()->colorMask(r, g, b, a);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DepthMask(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "depthMask", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsBoolean())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "depthMask", "first argument must be boolean")));
          return;
        }

        bool flag = info[0]->BooleanValue(isolate);
        handle()->depthMask(flag);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DepthFunc(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "depthFunc", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "depthFunc", "first argument(func) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int func = info[0]->Int32Value(context).FromMaybe(0);
        handle()->depthFunc(func);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DepthRange(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "depthRange", "2 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "depthRange", "both arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        float zNear = static_cast<float>(info[0]->NumberValue(context).FromMaybe(0.0));
        float zFar = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        handle()->depthRange(zNear, zFar);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::StencilFunc(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilFunc", "3 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilFunc", "all arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int func = info[0]->Int32Value(context).FromMaybe(0);
        int ref = info[1]->Int32Value(context).FromMaybe(0);
        int mask = info[2]->Int32Value(context).FromMaybe(0);
        handle()->stencilFunc(func, ref, mask);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::StencilFuncSeparate(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilFuncSeparate", "4 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilFuncSeparate", "all arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int face = info[0]->Int32Value(context).FromMaybe(0);
        int func = info[1]->Int32Value(context).FromMaybe(0);
        int ref = info[2]->Int32Value(context).FromMaybe(0);
        int mask = info[3]->Int32Value(context).FromMaybe(0);
        handle()->stencilFuncSeparate(face, func, ref, mask);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::StencilMask(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilMask", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilMask", "first argument(mask) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int mask = info[0]->Int32Value(context).FromMaybe(0);
        handle()->stencilMask(mask);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::StencilMaskSeparate(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilMaskSeparate", "2 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilMaskSeparate", "both arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int face = info[0]->Int32Value(context).FromMaybe(0);
        int mask = info[1]->Int32Value(context).FromMaybe(0);
        handle()->stencilMaskSeparate(face, mask);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::StencilOp(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          auto msg = "3 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "stencilOp", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilOp", "all arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int fail = info[0]->Int32Value(context).FromMaybe(0);
        int zfail = info[1]->Int32Value(context).FromMaybe(0);
        int zpass = info[2]->Int32Value(context).FromMaybe(0);
        handle()->stencilOp(fail, zfail, zpass);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::StencilOpSeparate(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          auto msg = "4 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "stencilOpSeparate", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "stencilOpSeparate", "all arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int face = info[0]->Int32Value(context).FromMaybe(0);
        int fail = info[1]->Int32Value(context).FromMaybe(0);
        int zfail = info[2]->Int32Value(context).FromMaybe(0);
        int zpass = info[3]->Int32Value(context).FromMaybe(0);
        handle()->stencilOpSeparate(face, fail, zfail, zpass);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BlendColor(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          auto msg = "4 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "blendColor", msg.c_str())));
          return;
        }
        for (int i = 0; i < 4; ++i)
        {
          if (!info[i]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "blendColor", "all arguments must be numbers")));
            return;
          }
        }

        Local<Context> context = isolate->GetCurrentContext();
        float r = static_cast<float>(info[0]->NumberValue(context).FromMaybe(0.0));
        float g = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float b = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        float a = static_cast<float>(info[3]->NumberValue(context).FromMaybe(0.0));
        handle()->blendColor(r, g, b, a);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BlendEquation(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "blendEquation", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "blendEquation", "first argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int mode = info[0]->Int32Value(context).FromMaybe(0);
        handle()->blendEquation(mode);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BlendEquationSeparate(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          auto msg = "2 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "blendEquationSeparate", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "blendEquationSeparate", "both arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int modeRGB = info[0]->Int32Value(context).FromMaybe(0);
        int modeAlpha = info[1]->Int32Value(context).FromMaybe(0);

        handle()->blendEquationSeparate(modeRGB, modeAlpha);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BlendFunc(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          auto msg = "2 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "blendFunc", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "blendFunc", "both arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int sfactor = info[0]->Int32Value(context).FromMaybe(0);
        int dfactor = info[1]->Int32Value(context).FromMaybe(0);

        handle()->blendFunc(sfactor, dfactor);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::BlendFuncSeparate(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          auto msg = "4 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "blendFuncSeparate", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "blendFuncSeparate", "all arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int srcRGB = info[0]->Int32Value(context).FromMaybe(0);
        int dstRGB = info[1]->Int32Value(context).FromMaybe(0);
        int srcAlpha = info[2]->Int32Value(context).FromMaybe(0);
        int dstAlpha = info[3]->Int32Value(context).FromMaybe(0);

        handle()->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::RenderbufferStorage(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "renderbufferStorage", "4 arguments required, but only fewer present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "renderbufferStorage", "first argument(target) must be a number")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "renderbufferStorage", "second argument(internalformat) must be a number")));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "renderbufferStorage", "third argument(width) must be a number")));
          return;
        }
        if (!info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "renderbufferStorage", "fourth argument(height) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int target = info[0]->Int32Value(context).FromMaybe(0);
        int internalformat = info[1]->Int32Value(context).FromMaybe(0);
        int width = info[2]->Int32Value(context).FromMaybe(0);
        int height = info[3]->Int32Value(context).FromMaybe(0);

        handle()->renderbufferStorage(static_cast<client_graphics::WebGLRenderbufferBindingTarget>(target),
                                      internalformat,
                                      width,
                                      height);
        info.GetReturnValue().SetUndefined();
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
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteBuffer", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLBuffer::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "deleteBuffer", 0, "WebGLBuffer", info[0])));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto bufferObj = info[0]->ToObject(context).ToLocalChecked();
        auto bufferBinding = WebGLBuffer::Unwrap(isolate, bufferObj);
        if (bufferBinding == nullptr || !bufferBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteBuffer", "invalid WebGLBuffer object")));
          return;
        }

        handle()->deleteBuffer(bufferBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DeleteFramebuffer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteFramebuffer", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLFramebuffer::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteFramebuffer", "first argument must be a WebGLFramebuffer object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto framebufferObj = info[0]->ToObject(context).ToLocalChecked();
        auto framebufferBinding = WebGLFramebuffer::Unwrap(isolate, framebufferObj);
        if (framebufferBinding == nullptr || !framebufferBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteFramebuffer", "invalid WebGLFramebuffer object")));
          return;
        }

        handle()->deleteFramebuffer(framebufferBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DeleteProgram(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteProgram", "1 argument required, and it must not be null")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteProgram", "first argument must be a WebGLProgram object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteProgram", "invalid WebGLProgram object")));
          return;
        }

        handle()->deleteProgram(programBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DeleteRenderbuffer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteRenderbuffer", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLRenderbuffer::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteRenderbuffer", "first argument must be a WebGLRenderbuffer object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto renderbufferObj = info[0]->ToObject(context).ToLocalChecked();
        auto renderbufferBinding = WebGLRenderbuffer::Unwrap(isolate, renderbufferObj);
        if (renderbufferBinding == nullptr || !renderbufferBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteRenderbuffer", "invalid WebGLRenderbuffer object")));
          return;
        }

        handle()->deleteRenderbuffer(renderbufferBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DeleteShader(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteShader", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteShader", "first argument must be a WebGLShader object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto shaderObj = info[0]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteShader", "invalid WebGLShader object")));
          return;
        }

        handle()->deleteShader(shaderBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DeleteTexture(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteTexture", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLTexture::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteTexture", "first argument must be a WebGLTexture object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto textureObj = info[0]->ToObject(context).ToLocalChecked();
        auto textureBinding = WebGLTexture::Unwrap(isolate, textureObj);
        if (textureBinding == nullptr || !textureBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteTexture", "invalid WebGLTexture object")));
          return;
        }

        handle()->deleteTexture(textureBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DrawArrays(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "drawArrays", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "drawArrays", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int mode = info[0]->Int32Value(context).FromMaybe(0);
        int first = info[1]->Int32Value(context).FromMaybe(0);
        int count = info[2]->Int32Value(context).FromMaybe(0);

        handle()->drawArrays(static_cast<client_graphics::WebGLDrawMode>(mode), first, count);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DrawElements(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          auto msg = "3 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "drawElements", msg.c_str())));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "drawElements", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int mode = info[0]->Int32Value(context).FromMaybe(0);
        int count = info[1]->Int32Value(context).FromMaybe(0);
        int type = info[2]->Int32Value(context).FromMaybe(0);

        int offset = 0;
        if (info.Length() > 3 && info[3]->IsNumber())
        {
          offset = info[3]->Int32Value(context).FromMaybe(0);
        }

        handle()->drawElements(static_cast<client_graphics::WebGLDrawMode>(mode),
                               count,
                               type,
                               offset);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Enable(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "enable", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "enable", "first argument(cap) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int cap = info[0]->Int32Value(context).FromMaybe(0);
        handle()->enable(cap);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Disable(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "disable", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "disable", "first argument(cap) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int cap = info[0]->Int32Value(context).FromMaybe(0);
        handle()->disable(cap);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::EnableVertexAttribArray(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "enableVertexAttribArray", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "enableVertexAttribArray", "first argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        handle()->enableVertexAttribArray(index);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::DisableVertexAttribArray(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "disableVertexAttribArray", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "disableVertexAttribArray", "first argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        handle()->disableVertexAttribArray(index);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::GetActiveAttrib(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveAttrib", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveAttrib", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveAttrib", "second argument must be a number")));
          return;
        }

        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveAttrib", "invalid WebGLProgram object")));
          return;
        }

        int index = info[1]->Int32Value(context).FromMaybe(0);
        auto activeAttrib = handle()->getActiveAttrib(programBinding->handle(), index);
        if (!activeAttrib.has_value())
        {
          info.GetReturnValue().Set(Null(isolate));
        }
        else
        {
          info.GetReturnValue().Set(WebGLActiveInfo::NewInstance(isolate,
                                                                 activeAttrib.value()));
        }
      }

      void WebGLRenderingContext::GetActiveUniform(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveUniform", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveUniform", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveUniform", "second argument must be a number")));
          return;
        }

        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getActiveUniform", "invalid WebGLProgram object")));
          return;
        }

        int index = info[1]->Int32Value(context).FromMaybe(0);
        auto activeUniform = handle()->getActiveUniform(programBinding->handle(), index);
        if (!activeUniform.has_value())
        {
          info.GetReturnValue().Set(Null(isolate));
        }
        else
        {
          info.GetReturnValue().Set(WebGLActiveInfo::NewInstance(isolate, activeUniform.value()));
        }
      }

      void WebGLRenderingContext::GetAttribLocation(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getAttribLocation", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getAttribLocation", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!info[1]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getAttribLocation", "second argument must be a string")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getAttribLocation", "invalid WebGLProgram object")));
          return;
        }

        auto nameUtf8 = String::Utf8Value(isolate, info[1]);
        string name = *nameUtf8;

        // WebGL 2.0 spec §5.18: Built-in attributes always return -1
        static const std::set<string> kBuiltInAttribs = {
          "gl_VertexID", "gl_InstanceID", "gl_Position", "gl_PointSize"};
        if (kBuiltInAttribs.count(name))
        {
          info.GetReturnValue().Set(Number::New(isolate, -1));
          return;
        }

        auto loc = handle()->getAttribLocation(programBinding->handle(), name);
        if (!loc.has_value())
        {
          info.GetReturnValue().Set(Number::New(isolate, -1));
        }
        else
        {
          info.GetReturnValue().Set(Number::New(isolate, loc->index.value_or(-1)));
        }
      }

      void WebGLRenderingContext::GetUniformLocation(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          auto msg = "2 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "getUniformLocation", msg.c_str())));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getUniformLocation", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!info[1]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getUniformLocation", "second argument must be a string")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getUniformLocation", "invalid WebGLProgram object")));
          return;
        }

        auto nameUtf8 = String::Utf8Value(isolate, info[1]);
        string name = *nameUtf8;

        auto location = handle()->getUniformLocation(programBinding->handle(), name);
        if (!location.has_value())
        {
          info.GetReturnValue().SetNull();
        }
        else
        {
          info.GetReturnValue().Set(WebGLUniformLocation::NewInstance(isolate, location.value()));
        }
      }

      void WebGLRenderingContext::LinkProgram(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "linkProgram", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "linkProgram", "first argument must be a WebGLProgram object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "linkProgram", "invalid WebGLProgram object")));
          return;
        }

        handle()->linkProgram(programBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::UseProgram(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "useProgram", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]) && !info[0]->IsNull())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "useProgram", "first argument must be a WebGLProgram object or null")));
          return;
        }

        if (info[0]->IsNull())
        {
          handle()->useProgram(nullptr);
        }
        else
        {
          Local<Context> context = isolate->GetCurrentContext();
          auto programObj = info[0]->ToObject(context).ToLocalChecked();
          auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
          if (programBinding == nullptr || !programBinding->hasData())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "useProgram", "invalid WebGLProgram object")));
            return;
          }
          handle()->useProgram(programBinding->handle());
        }

        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::ValidateProgram(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "validateProgram", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "validateProgram", "first argument must be a WebGLProgram object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "validateProgram", "invalid WebGLProgram object")));
          return;
        }

        handle()->validateProgram(programBinding->handle());
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::ShaderSource(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "shaderSource", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "shaderSource", "first argument must be a WebGLShader object")));
          return;
        }
        if (!info[1]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "shaderSource", "second argument must be a string")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto shaderObj = info[0]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "shaderSource", "invalid WebGLShader object")));
          return;
        }

        auto sourceUtf8 = String::Utf8Value(isolate, info[1]);
        string source = *sourceUtf8 ? *sourceUtf8 : "";

        handle()->shaderSource(shaderBinding->handle(), source);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::GetShaderSource(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderSource", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderSource", "first argument must be a WebGLShader object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto shaderObj = info[0]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderSource", "invalid WebGLShader object")));
          return;
        }

        string source = handle()->getShaderSource(shaderBinding->handle());
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, source.c_str()).ToLocalChecked());
      }

      void WebGLRenderingContext::TexImage2D(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 6)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "texImage2D", 6, info.Length())));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texImage2D", "target", "number", info[0])));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texImage2D", "level", "number", info[1])));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texImage2D", "internalformat", "number", info[2])));
          return;
        }

        client_graphics::WebGLTexture2DTarget target;
        {
          int value = info[0]->Int32Value(context).FromMaybe(0);
          target = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int level = info[1]->Int32Value(context).FromMaybe(0);
        int internalformat = info[2]->Int32Value(context).FromMaybe(0);
        int width;
        int height;
        int border = 0;
        client_graphics::WebGLTextureFormat format;
        client_graphics::WebGLPixelType pixelType;

        SkPixmap imagePixmap;
        unsigned char *pixels = nullptr;

        if (info.Length() == 6)
        {
          if (!info[3]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "format", "number", info[3])));
            return;
          }
          if (!info[4]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "type", "number", info[4])));
            return;
          }
          {
            // Update `format` from args[3]
            int value = info[3]->Int32Value(context).FromMaybe(0);
            format = static_cast<client_graphics::WebGLTextureFormat>(value);
          }
          {
            // Assign `pixelType` from args[4]
            int value = info[4]->Int32Value(context).FromMaybe(0);
            pixelType = static_cast<client_graphics::WebGLPixelType>(value);
          }

          auto sourceValue = info[5];
          auto imageSource = canvas_bindings::GetImageSourceFromValue(isolate, sourceValue);
          if (imageSource == nullptr)
          {
            auto msg =
              "ImageData, HTMLImageElement, HTMLCanvasElement, HTMLVideoElement, ImageBitmap, OffscreenCanvas"
              " or null object";
            cerr << "Invalid image source for texImage2D: " << string(msg) << endl;
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "pixels", msg, sourceValue)));
            return;
          }
          else
          {
            if (!imageSource->readPixels(imagePixmap))
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "texImage2D", "Failed to read pixels from the image source")));
              return;
            }
            width = imagePixmap.width();
            height = imagePixmap.height();
            // FIXME(yorkie): Support other pixel formats.
            format = client_graphics::WebGLTextureFormat::kRGBA;
            internalformat = WEBGL2_RGBA8;
            pixels = reinterpret_cast<unsigned char *>(imagePixmap.writable_addr());
          }
        }
        else if (info.Length() >= 9)
        {
          if (!info[3]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "width", "number", info[3])));
            return;
          }
          if (!info[4]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "height", "number", info[4])));
            return;
          }
          if (!info[5]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "border", "number", info[5])));
            return;
          }
          if (!info[6]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "format", "number", info[6])));
            return;
          }
          if (!info[7]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texImage2D", "type", "number", info[7])));
            return;
          }

          width = info[3]->Int32Value(context).FromMaybe(0);
          height = info[4]->Int32Value(context).FromMaybe(0);
          border = info[5]->Int32Value(context).FromMaybe(0);
          {
            // Update `format` from args[6]
            int value = info[6]->Int32Value(context).FromMaybe(0);
            format = static_cast<client_graphics::WebGLTextureFormat>(value);
          }
          {
            // Assign `pixelType` from args[7]
            int value = info[7]->Int32Value(context).FromMaybe(0);
            pixelType = static_cast<client_graphics::WebGLPixelType>(value);
          }

          if (info[8]->IsNull())
          {
            pixels = nullptr;
          }
          else if (info[8]->IsArrayBufferView())
          {
            auto data = info[8].As<ArrayBufferView>();
            auto arrayBuffer = data->Buffer();
            if (arrayBuffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "texImage2D", "Invalid ArrayBufferView")));
              return;
            }
            pixels = static_cast<uint8_t *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset();
          }
          else
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "texImage2D", "Argument 8 must be a TypedArray or null object")));
            return;
          }
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texImage2D", "Either 6 or 9 arguments must be provided")));
          return;
        }

        handle()->texImage2D(target,
                             level,
                             internalformat,
                             width,
                             height,
                             border,
                             format,
                             pixelType,
                             pixels);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::TexSubImage2D(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 7)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texSubImage2D", "7 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texSubImage2D", "target", "number", info[0])));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texSubImage2D", "level", "number", info[1])));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texSubImage2D", "xoffset", "number", info[2])));
          return;
        }
        if (!info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "texSubImage2D", "yoffset", "number", info[3])));
          return;
        }

        client_graphics::WebGLTexture2DTarget target;
        {
          int value = info[0]->Int32Value(context).FromMaybe(0);
          target = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int level = info[1]->Int32Value(context).FromMaybe(0);
        int xoffset = info[2]->Int32Value(context).FromMaybe(0);
        int yoffset = info[3]->Int32Value(context).FromMaybe(0);

        if (info.Length() == 7)
        {
          if (!info[4]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "format", "number", info[4])));
            return;
          }
          if (!info[5]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "type", "number", info[5])));
            return;
          }

          client_graphics::WebGLTextureFormat format;
          client_graphics::WebGLPixelType pixelType;
          {
            int value = info[4]->Int32Value(context).FromMaybe(0);
            format = static_cast<client_graphics::WebGLTextureFormat>(value);
          }
          {
            int value = info[5]->Int32Value(context).FromMaybe(0);
            pixelType = static_cast<client_graphics::WebGLPixelType>(value);
          }

          auto sourceValue = info[6];
          auto imageSource = canvas_bindings::GetImageSourceFromValue(isolate, sourceValue);
          if (imageSource == nullptr)
          {
            auto msg =
              "ImageData, HTMLImageElement, HTMLCanvasElement, HTMLVideoElement, ImageBitmap, OffscreenCanvas"
              " or null object";
            cerr << "Invalid image source for texSubImage2D: " << string(msg) << endl;
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "pixels", msg, sourceValue)));
            return;
          }

          SkPixmap imagePixmap;
          if (!imageSource->readPixels(imagePixmap))
          {
            Warn(isolate, sourceValue);
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "texSubImage2D", "Failed to read pixels from the image source")));
            return;
          }

          handle()->texSubImage2D(target,
                                  level,
                                  xoffset,
                                  yoffset,
                                  imagePixmap.width(),
                                  imagePixmap.height(),
                                  format,
                                  pixelType,
                                  reinterpret_cast<unsigned char *>(imagePixmap.writable_addr()));
        }
        else if (info.Length() >= 9)
        {
          if (!info[4]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "width", "number", info[4])));
            return;
          }
          if (!info[5]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "height", "number", info[5])));
            return;
          }
          if (!info[6]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "format", "number", info[6])));
            return;
          }
          if (!info[7]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage2D", "type", "number", info[7])));
            return;
          }

          int width = info[4]->Int32Value(context).FromMaybe(0);
          int height = info[5]->Int32Value(context).FromMaybe(0);
          client_graphics::WebGLTextureFormat format;
          client_graphics::WebGLPixelType pixelType;
          {
            int value = info[6]->Int32Value(context).FromMaybe(0);
            format = static_cast<client_graphics::WebGLTextureFormat>(value);
          }
          {
            int value = info[7]->Int32Value(context).FromMaybe(0);
            pixelType = static_cast<client_graphics::WebGLPixelType>(value);
          }

          SkPixmap imagePixmap;
          unsigned char *pixels = nullptr;

          if (info[8]->IsNull())
          {
            pixels = nullptr;
          }
          else if (info[8]->IsArrayBufferView())
          {
            auto data = info[8].As<ArrayBufferView>();
            auto arrayBuffer = data->Buffer();
            if (arrayBuffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "texSubImage2D", "Invalid ArrayBufferView")));
              return;
            }
            pixels = static_cast<uint8_t *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset();
          }

          // WebGL2 supports:
          //  texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, srcData, srcOffset)
          //  texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, source)
          //  texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, offset)
          if (handle()->isWebGL2())
          {
            if (info[8]->IsNumber()) // offset
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "texSubImage2D", "Offset source is not supported")));
              return;
            }
            else if (info[8]->IsArrayBufferView() && info.Length() >= 10) // srcData, srcOffset
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "texSubImage2D", "srcData with srcOffset is not supported")));
              return;
            }

            // source
            auto imageSource = canvas_bindings::GetImageSourceFromValue(isolate, info[8]);
            if (imageSource != nullptr)
            {
              if (imageSource->readPixels(imagePixmap))
              {
                pixels = reinterpret_cast<unsigned char *>(imagePixmap.writable_addr());
                width = imagePixmap.width();
                height = imagePixmap.height();
              }
            }
          }

          handle()->texSubImage2D(target,
                                  level,
                                  xoffset,
                                  yoffset,
                                  width,
                                  height,
                                  format,
                                  pixelType,
                                  pixels);
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texSubImage2D", "Either 7 or 9 arguments must be provided")));
          return;
        }
      }

      void WebGLRenderingContext::TexParameterf(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texParameterf", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texParameterf", "third argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int target = info[0]->Int32Value(context).FromMaybe(0);
        int pname = info[1]->Int32Value(context).FromMaybe(0);
        float param = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));

        handle()->texParameterf(static_cast<client_graphics::WebGLTextureTarget>(target),
                                static_cast<client_graphics::WebGLTextureParameterName>(pname),
                                param);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::TexParameteri(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texParameteri", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texParameteri", "third argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int target = info[0]->Int32Value(context).FromMaybe(0);
        int pname = info[1]->Int32Value(context).FromMaybe(0);
        int param = info[2]->Int32Value(context).FromMaybe(0);

        handle()->texParameteri(static_cast<client_graphics::WebGLTextureTarget>(target),
                                static_cast<client_graphics::WebGLTextureParameterName>(pname),
                                param);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::CopyTexImage2D(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        if (info.Length() < 8)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "copyTexImage2D", "8 arguments required, but fewer were provided")));
          return;
        }

        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() ||
            !info[3]->IsNumber() || !info[4]->IsNumber() || !info[5]->IsNumber() ||
            !info[6]->IsNumber() || !info[7]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "copyTexImage2D", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        client_graphics::WebGLTexture2DTarget target;
        {
          int value = info[0]->Int32Value(context).FromMaybe(0);
          target = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int level = info[1]->Int32Value(context).FromMaybe(0);
        int internalformat = info[2]->Int32Value(context).FromMaybe(0);
        int x = info[3]->Int32Value(context).FromMaybe(0);
        int y = info[4]->Int32Value(context).FromMaybe(0);
        int width = info[5]->Int32Value(context).FromMaybe(0);
        int height = info[6]->Int32Value(context).FromMaybe(0);
        int border = info[7]->Int32Value(context).FromMaybe(0);

        handle()->copyTexImage2D(target, level, internalformat, x, y, width, height, border);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::CopyTexSubImage2D(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 7)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "copyTexSubImage2D", "7 arguments required, but fewer were provided")));
          return;
        }

        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() ||
            !info[3]->IsNumber() || !info[4]->IsNumber() || !info[5]->IsNumber() || !info[6]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "copyTexSubImage2D", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        client_graphics::WebGLTexture2DTarget target;
        {
          int value = info[0]->Int32Value(context).FromMaybe(0);
          target = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int level = info[1]->Int32Value(context).FromMaybe(0);
        int xoffset = info[2]->Int32Value(context).FromMaybe(0);
        int yoffset = info[3]->Int32Value(context).FromMaybe(0);
        int x = info[4]->Int32Value(context).FromMaybe(0);
        int y = info[5]->Int32Value(context).FromMaybe(0);
        int width = info[6]->Int32Value(context).FromMaybe(0);
        int height = info[7]->Int32Value(context).FromMaybe(0);

        handle()->copyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::GenerateMipmap(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "generateMipmap", "1 argument required, but only 0 present")));
          return;
        }
        if (!info[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "generateMipmap", "first argument(target) must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int target = info[0]->Int32Value(context).FromMaybe(0);
        handle()->generateMipmap(static_cast<client_graphics::WebGLTextureTarget>(target));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform1f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 2)
        {
          auto msg = "2 arguments required, but only " + std::to_string(info.Length()) + " present";
          isolate->ThrowException(Exception::TypeError(MakeMethodError(isolate, "uniform1f", msg.c_str())));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate,
                            "uniform1f",
                            "First argument must be a WebGLUniformLocation object and second argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        float v0 = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        handle()->uniform1f(locBinding->handleRef(), v0);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform1i(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1i", "2 arguments required, but only fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1i", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        int v0 = info[1]->ToNumber(context).ToLocalChecked()->Int32Value(context).FromMaybe(0);
        handle()->uniform1i(locBinding->handleRef(), v0);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform2f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2f", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2f", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        float v0 = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float v1 = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        handle()->uniform2f(locBinding->handleRef(), v0, v1);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform2i(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2i", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2i", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        int v0 = info[1]->Int32Value(context).FromMaybe(0);
        int v1 = info[2]->Int32Value(context).FromMaybe(0);
        handle()->uniform2i(locBinding->handleRef(), v0, v1);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform3f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3f", "4 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3f", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        float v0 = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float v1 = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        float v2 = static_cast<float>(info[3]->NumberValue(context).FromMaybe(0.0));
        handle()->uniform3f(locBinding->handleRef(), v0, v1, v2);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform3i(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3i", "4 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3i", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        int v0 = info[1]->Int32Value(context).FromMaybe(0);
        int v1 = info[2]->Int32Value(context).FromMaybe(0);
        int v2 = info[3]->Int32Value(context).FromMaybe(0);
        handle()->uniform3i(locBinding->handleRef(), v0, v1, v2);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform4f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4f", "5 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4f", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        float v0 = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float v1 = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        float v2 = static_cast<float>(info[3]->NumberValue(context).FromMaybe(0.0));
        float v3 = static_cast<float>(info[4]->NumberValue(context).FromMaybe(0.0));
        handle()->uniform4f(locBinding->handleRef(), v0, v1, v2, v3);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform4i(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4i", "5 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4i", "First argument must be a WebGLUniformLocation object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        int v0 = info[1]->Int32Value(context).FromMaybe(0);
        int v1 = info[2]->Int32Value(context).FromMaybe(0);
        int v2 = info[3]->Int32Value(context).FromMaybe(0);
        int v3 = info[4]->Int32Value(context).FromMaybe(0);
        handle()->uniform4i(locBinding->handleRef(), v0, v1, v2, v3);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform1fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1fv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1fv", "Second argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform1fv(locBinding->handleRef(),
                             GetFloatValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform1iv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);


        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1iv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1iv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1iv", "Second argument must be an Int32Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform1iv(locBinding->handleRef(),
                             GetIntValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform2fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2fv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2fv", "Second argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform2fv(locBinding->handleRef(),
                             GetFloatValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform2iv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2iv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2iv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform2iv", "Second argument must be an Int32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform2iv(locBinding->handleRef(),
                             GetIntValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform3fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3fv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3fv", "Second argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform3fv(locBinding->handleRef(),
                             GetFloatValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform3iv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3iv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3iv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform3iv", "Second argument must be an Int32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform3iv(locBinding->handleRef(),
                             GetIntValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform4fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4fv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4fv", "Second argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform4fv(locBinding->handleRef(),
                             GetFloatValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Uniform4iv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4iv", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4iv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsTypedArray() && !info[1]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform4iv", "Second argument must be an Int32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        handle()->uniform4iv(locBinding->handleRef(),
                             GetIntValuesFromValue(isolate, info[1]));
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::UniformMatrix2fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix2fv", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix2fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsBoolean())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix2fv", "Second argument must be a boolean")));
          return;
        }
        if (!info[2]->IsTypedArray() && !info[2]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix2fv", "Third argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        bool transpose = info[1]->BooleanValue(isolate);
        const auto &values = GetFloatValuesFromValue(isolate, info[2]);

        if (values.size() % 4 != 0)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix2fv", "Data length must be a multiple of 4")));
          return;
        }

        handle()->uniformMatrix2fv(locBinding->handleRef(), transpose, values);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::UniformMatrix3fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix3fv", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix3fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsBoolean())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix3fv", "Second argument must be a boolean")));
          return;
        }
        if (!info[2]->IsTypedArray() && !info[2]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix3fv", "Third argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        bool transpose = info[1]->BooleanValue(isolate);
        const auto &values = GetFloatValuesFromValue(isolate, info[2]);

        if (values.size() % 9 != 0)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix3fv", "Data length must be a multiple of 9")));
          return;
        }

        handle()->uniformMatrix3fv(locBinding->handleRef(), transpose, values);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::UniformMatrix4fv(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix4fv", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLUniformLocation::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix4fv", "First argument must be a WebGLUniformLocation object")));
          return;
        }
        if (!info[1]->IsBoolean())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix4fv", "Second argument must be a boolean")));
          return;
        }
        if (!info[2]->IsTypedArray() && !info[2]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix4fv", "Third argument must be a Float32Array or Array")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto locObj = info[0]->ToObject(context).ToLocalChecked();
        auto locBinding = WebGLUniformLocation::Unwrap(isolate, locObj);

        bool transpose = info[1]->BooleanValue(isolate);
        const auto &values = GetFloatValuesFromValue(isolate, info[2]);

        if (values.size() % 16 != 0)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformMatrix4fv", "Data length must be a multiple of 16")));
          return;
        }

        handle()->uniformMatrix4fv(locBinding->handleRef(), transpose, values);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::VertexAttrib1f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib1f", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib1f", "Both arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));

        handle()->vertexAttrib1f(index, x);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::VertexAttrib2f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib2f", "3 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib2f", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));

        handle()->vertexAttrib2f(index, x, y);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::VertexAttrib3f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib3f", "4 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib3f", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        float z = static_cast<float>(info[3]->NumberValue(context).FromMaybe(0.0));

        handle()->vertexAttrib3f(index, x, y, z);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::VertexAttrib4f(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib4f", "5 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber() || !info[4]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttrib4f", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        float x = static_cast<float>(info[1]->NumberValue(context).FromMaybe(0.0));
        float y = static_cast<float>(info[2]->NumberValue(context).FromMaybe(0.0));
        float z = static_cast<float>(info[3]->NumberValue(context).FromMaybe(0.0));
        float w = static_cast<float>(info[4]->NumberValue(context).FromMaybe(0.0));

        handle()->vertexAttrib4f(index, x, y, z, w);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::VertexAttribPointer(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 6)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttribPointer", "6 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() ||
            !info[3]->IsBoolean() || !info[4]->IsNumber() || !info[5]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "vertexAttribPointer", "Arguments must be of the correct types")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int index = info[0]->Int32Value(context).FromMaybe(0);
        int size = info[1]->Int32Value(context).FromMaybe(0);
        int type = info[2]->Int32Value(context).FromMaybe(0);
        bool normalized = info[3]->BooleanValue(isolate);
        int stride = info[4]->Int32Value(context).FromMaybe(0);
        int offset = info[5]->Int32Value(context).FromMaybe(0);

        handle()->vertexAttribPointer(index, size, type, normalized, stride, offset);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Viewport(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "viewport", "4 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "viewport", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int x = info[0]->Int32Value(context).FromMaybe(0);
        int y = info[1]->Int32Value(context).FromMaybe(0);
        int width = info[2]->Int32Value(context).FromMaybe(0);
        int height = info[3]->Int32Value(context).FromMaybe(0);

        handle()->viewport(x, y, width, height);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::Scissor(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        if (info.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "scissor", "4 arguments required, but fewer were provided")));
          return;
        }
        if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "scissor", "All arguments must be numbers")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        int x = info[0]->Int32Value(context).FromMaybe(0);
        int y = info[1]->Int32Value(context).FromMaybe(0);
        int width = info[2]->Int32Value(context).FromMaybe(0);
        int height = info[3]->Int32Value(context).FromMaybe(0);

        handle()->scissor(x, y, width, height);
        info.GetReturnValue().SetUndefined();
      }

      void WebGLRenderingContext::GetError(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto err = handle()->getError();
        info.GetReturnValue().Set(Integer::New(isolate, err));
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

        // WebGL2 parameters
        if (handle()->isWebGL2())
        {
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
          {
            auto value = handle<client_graphics::WebGL2Context>()
                           ->getParameterV2(static_cast<client_graphics::WebGL2IntegerParameterName>(pname));
            jsValue = Integer::New(isolate, value);
            break;
          }
          default:
            break;
          }
        }

        // Return the value
        info.GetReturnValue().Set(jsValue);
      }

      void WebGLRenderingContext::GetProgramParameter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramParameter", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramParameter", "first argument must be a WebGLProgram object")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramParameter", "second argument must be a number")));
          return;
        }

        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramParameter", "invalid WebGLProgram object")));
          return;
        }

        int pname = info[1]->Int32Value(context).FromMaybe(0);
        try
        {
          auto value = handle()->getProgramParameter(programBinding->handle(), pname);
          info.GetReturnValue().Set(Number::New(isolate, value));
        }
        catch (const exception &e)
        {
          cerr << "WebGLRenderingContext::GetProgramParameter: Exception: " << e.what() << endl;
          info.GetReturnValue().SetUndefined();
        }
      }

      void WebGLRenderingContext::GetShaderParameter(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderParameter", "2 arguments required, but fewer were provided")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderParameter", "first argument must be a WebGLShader object")));
          return;
        }
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderParameter", "second argument must be a number")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto shaderObj = info[0]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderParameter", "invalid WebGLShader object")));
          return;
        }

        int pname = info[1]->Int32Value(context).FromMaybe(0);
        try
        {
          auto value = handle()->getShaderParameter(shaderBinding->handle(), pname);
          info.GetReturnValue().Set(Number::New(isolate, value));
        }
        catch (const exception &e)
        {
          cerr << "WebGLRenderingContext::GetShaderParameter: Exception: " << e.what() << endl;
          info.GetReturnValue().SetUndefined();
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
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramInfoLog", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLProgram::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramInfoLog", "first argument must be a WebGLProgram object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto programObj = info[0]->ToObject(context).ToLocalChecked();
        auto programBinding = WebGLProgram::Unwrap(isolate, programObj);
        if (programBinding == nullptr || !programBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getProgramInfoLog", "invalid WebGLProgram object")));
          return;
        }

        string log = handle()->getProgramInfoLog(programBinding->handle());
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, log.c_str()).ToLocalChecked());
      }

      void WebGLRenderingContext::GetShaderInfoLog(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderInfoLog", "1 argument required, but only 0 present")));
          return;
        }
        if (!WebGLShader::IsInstanceOf(isolate, info[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderInfoLog", "first argument must be a WebGLShader object")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        auto shaderObj = info[0]->ToObject(context).ToLocalChecked();
        auto shaderBinding = WebGLShader::Unwrap(isolate, shaderObj);
        if (shaderBinding == nullptr || !shaderBinding->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getShaderInfoLog", "invalid WebGLShader object")));
          return;
        }

        string log = handle()->getShaderInfoLog(shaderBinding->handle());
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, log.c_str()).ToLocalChecked());
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

        if (handle()->supportsExtension(name))
        {
          info.GetReturnValue().Set(WebGLExtensions::GetExtension(isolate, name));
        }
        else
        {
          info.GetReturnValue().SetNull();
        }
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

      vector<float> WebGLRenderingContext::GetFloatValuesFromValue(Isolate *isolate, Local<Value> value)
      {
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        vector<float> values;
        if (value->IsFloat32Array())
        {
          auto data = value.As<Float32Array>();
          auto arrayBuffer = data->Buffer();
          if (arrayBuffer.IsEmpty())
          {
            isolate->ThrowException(Exception::TypeError(
              WebGLRenderingContextBase::MakeMethodError(isolate, "uniform1fv", "Invalid Float32Array")));
            return values;
          }

          auto bufferData = static_cast<float *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset() / sizeof(float);
          values.assign(bufferData, bufferData + data->Length());
        }
        else if (value->IsArray())
        {
          auto dataArray = value.As<Array>();
          uint32_t len = dataArray->Length();
          values.reserve(len);

          for (uint32_t i = 0; i < len; i++)
          {
            auto val = dataArray->Get(context, i).ToLocalChecked();
            if (val->IsNumber())
              values.push_back(static_cast<float>(val->NumberValue(context).FromMaybe(0.0)));
            else
              values.push_back(0.0f);
          }
        }
        return values;
      }

      vector<int> WebGLRenderingContext::GetIntValuesFromValue(Isolate *isolate, Local<Value> value)
      {
        auto data = value.As<Int32Array>();
        auto arrayBuffer = data->Buffer();
        if (arrayBuffer.IsEmpty())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniform1iv", "Invalid Int32Array")));
          return vector<int>{};
        }

        vector<int> values;
        auto bufferData = static_cast<int *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset() / sizeof(int);
        values.assign(bufferData, bufferData + data->Length());
        return values;
      }
    }
  }
} // namespace endor
