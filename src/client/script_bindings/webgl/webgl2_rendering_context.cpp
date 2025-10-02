#include "./webgl2_rendering_context.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGL2RenderingContext::SetupConstants(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

#define ADD_WEBGL2_CONSTANT(NAME) IntegerConstant(isolate, tpl, #NAME, WEBGL2_##NAME);
#define WEBGL2_CONSTANTS_PARAMETERS()                  \
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

#define WEBGL2_CONSTANTS_TEXTURES()             \
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

#define WEBGL2_CONSTANTS_PIXELTYPES()                 \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_2_10_10_10_REV)    \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_10F_11F_11F_REV)   \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_5_9_9_9_REV)       \
  ADD_WEBGL2_CONSTANT(FLOAT_32_UNSIGNED_INT_24_8_REV) \
  ADD_WEBGL2_CONSTANT(UNSIGNED_INT_24_8)              \
  ADD_WEBGL2_CONSTANT(HALF_FLOAT)                     \
  ADD_WEBGL2_CONSTANT(RG)                             \
  ADD_WEBGL2_CONSTANT(RG_INTEGER)                     \
  ADD_WEBGL2_CONSTANT(INT_2_10_10_10_REV)

#define WEBGL2_CONSTANTS_QUERIES()            \
  ADD_WEBGL2_CONSTANT(CURRENT_QUERY)          \
  ADD_WEBGL2_CONSTANT(QUERY_RESULT)           \
  ADD_WEBGL2_CONSTANT(QUERY_RESULT_AVAILABLE) \
  ADD_WEBGL2_CONSTANT(ANY_SAMPLES_PASSED)     \
  ADD_WEBGL2_CONSTANT(ANY_SAMPLES_PASSED_CONSERVATIVE)

#define WEBGL2_CONSTANTS_DRAWBUFFERS()       \
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

#define WEBGL2_CONSTANTS_SAMPLERS()                  \
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

#define WEBGL2_CONSTANTS_BUFFERS()                 \
  ADD_WEBGL2_CONSTANT(PIXEL_PACK_BUFFER)           \
  ADD_WEBGL2_CONSTANT(PIXEL_UNPACK_BUFFER)         \
  ADD_WEBGL2_CONSTANT(PIXEL_PACK_BUFFER_BINDING)   \
  ADD_WEBGL2_CONSTANT(PIXEL_UNPACK_BUFFER_BINDING) \
  ADD_WEBGL2_CONSTANT(COPY_READ_BUFFER)            \
  ADD_WEBGL2_CONSTANT(COPY_WRITE_BUFFER)           \
  ADD_WEBGL2_CONSTANT(COPY_READ_BUFFER_BINDING)    \
  ADD_WEBGL2_CONSTANT(COPY_WRITE_BUFFER_BINDING)

#define WEBGL2_CONSTANTS_DATA_TYPES()      \
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

#define WEBGL2_CONSTANTS_VERTEX_ATTRIBS()          \
  ADD_WEBGL2_CONSTANT(VERTEX_ATTRIB_ARRAY_INTEGER) \
  ADD_WEBGL2_CONSTANT(VERTEX_ATTRIB_ARRAY_DIVISOR)

#define WEBGL2_CONSTANTS_TRANSFORM_FEEDBACK()                        \
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

#define WEBGL2_CONSTANTS_FRAMEBUFFERS_AND_RENDERBUFFERS()    \
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

#define WEBGL2_CONSTANTS_UNIFORMS()                              \
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

#define WEBGL2_CONSTANTS_SYNC_OBJECTS()           \
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

#define WEBGL2_CONSTANTS_MISCELLANEOUS()  \
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

      WEBGL2_CONSTANTS_PARAMETERS()
      WEBGL2_CONSTANTS_TEXTURES()
      WEBGL2_CONSTANTS_PIXELTYPES()
      WEBGL2_CONSTANTS_QUERIES()
      WEBGL2_CONSTANTS_DRAWBUFFERS()
      WEBGL2_CONSTANTS_SAMPLERS()
      WEBGL2_CONSTANTS_BUFFERS()
      WEBGL2_CONSTANTS_DATA_TYPES()
      WEBGL2_CONSTANTS_VERTEX_ATTRIBS()
      WEBGL2_CONSTANTS_TRANSFORM_FEEDBACK()
      WEBGL2_CONSTANTS_FRAMEBUFFERS_AND_RENDERBUFFERS()
      WEBGL2_CONSTANTS_UNIFORMS()
      WEBGL2_CONSTANTS_SYNC_OBJECTS()
      WEBGL2_CONSTANTS_MISCELLANEOUS()

#undef ADD_WEBGL2_CONSTANT
    }

    void WebGL2RenderingContext::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      /**
       * WebGL2RenderingContext does not inherit from WebGLRenderingContext directly, but it should reuse the same
       * configuration for shared methods and properties.
       */
      WebGLRenderingContext::ConfigureFunctionTemplate(isolate, tpl);

      // WebGL 2.0 constants
      SetupConstants(isolate, tpl);

      // Set up WebGL 2.0 specific methods
      auto prototype = tpl->PrototypeTemplate();

#define ADD_WEBGL2_METHOD(NAME, CALLBACK) \
      InstanceMethod(isolate, prototype, NAME, &WebGL2RenderingContext::CALLBACK);

      // WebGL 2.0 specific methods
      ADD_WEBGL2_METHOD("drawBuffers", DrawBuffers)
      ADD_WEBGL2_METHOD("clearBufferfv", ClearBufferfv)
      ADD_WEBGL2_METHOD("clearBufferiv", ClearBufferiv)
      ADD_WEBGL2_METHOD("clearBufferuiv", ClearBufferuiv)
      ADD_WEBGL2_METHOD("clearBufferfi", ClearBufferfi)

      // Query objects
      ADD_WEBGL2_METHOD("createQuery", CreateQuery)
      ADD_WEBGL2_METHOD("deleteQuery", DeleteQuery)
      ADD_WEBGL2_METHOD("beginQuery", BeginQuery)
      ADD_WEBGL2_METHOD("endQuery", EndQuery)
      ADD_WEBGL2_METHOD("getQuery", GetQuery)
      ADD_WEBGL2_METHOD("getQueryParameter", GetQueryParameter)

      // Sampler objects
      ADD_WEBGL2_METHOD("createSampler", CreateSampler)
      ADD_WEBGL2_METHOD("deleteSampler", DeleteSampler)
      ADD_WEBGL2_METHOD("bindSampler", BindSampler)
      ADD_WEBGL2_METHOD("samplerParameteri", SamplerParameteri)
      ADD_WEBGL2_METHOD("samplerParameterf", SamplerParameterf)
      ADD_WEBGL2_METHOD("getSamplerParameter", GetSamplerParameter)

      // Sync objects
      ADD_WEBGL2_METHOD("fenceSync", FenceSync)
      ADD_WEBGL2_METHOD("isSync", IsSync)
      ADD_WEBGL2_METHOD("deleteSync", DeleteSync)
      ADD_WEBGL2_METHOD("clientWaitSync", ClientWaitSync)
      ADD_WEBGL2_METHOD("waitSync", WaitSync)
      ADD_WEBGL2_METHOD("getSyncParameter", GetSyncParameter)

      // Transform feedback
      ADD_WEBGL2_METHOD("createTransformFeedback", CreateTransformFeedback)
      ADD_WEBGL2_METHOD("deleteTransformFeedback", DeleteTransformFeedback)
      ADD_WEBGL2_METHOD("bindTransformFeedback", BindTransformFeedback)
      ADD_WEBGL2_METHOD("beginTransformFeedback", BeginTransformFeedback)
      ADD_WEBGL2_METHOD("endTransformFeedback", EndTransformFeedback)
      ADD_WEBGL2_METHOD("transformFeedbackVaryings", TransformFeedbackVaryings)
      ADD_WEBGL2_METHOD("getTransformFeedbackVarying", GetTransformFeedbackVarying)
      ADD_WEBGL2_METHOD("pauseTransformFeedback", PauseTransformFeedback)
      ADD_WEBGL2_METHOD("resumeTransformFeedback", ResumeTransformFeedback)

      // Uniform buffer objects
      ADD_WEBGL2_METHOD("bindBufferBase", BindBufferBase)
      ADD_WEBGL2_METHOD("bindBufferRange", BindBufferRange)
      ADD_WEBGL2_METHOD("getUniformIndices", GetUniformIndices)
      ADD_WEBGL2_METHOD("getActiveUniforms", GetActiveUniforms)
      ADD_WEBGL2_METHOD("getUniformBlockIndex", GetUniformBlockIndex)
      ADD_WEBGL2_METHOD("getActiveUniformBlockParameter", GetActiveUniformBlockParameter)
      ADD_WEBGL2_METHOD("getActiveUniformBlockName", GetActiveUniformBlockName)
      ADD_WEBGL2_METHOD("uniformBlockBinding", UniformBlockBinding)

      // Vertex array objects
      ADD_WEBGL2_METHOD("createVertexArray", CreateVertexArray)
      ADD_WEBGL2_METHOD("deleteVertexArray", DeleteVertexArray)
      ADD_WEBGL2_METHOD("bindVertexArray", BindVertexArray)
      ADD_WEBGL2_METHOD("isVertexArray", IsVertexArray)

      // Enhanced texture operations
      ADD_WEBGL2_METHOD("texImage3D", TexImage3D)
      ADD_WEBGL2_METHOD("texSubImage3D", TexSubImage3D)
      ADD_WEBGL2_METHOD("copyTexSubImage3D", CopyTexSubImage3D)
      ADD_WEBGL2_METHOD("compressedTexImage3D", CompressedTexImage3D)
      ADD_WEBGL2_METHOD("compressedTexSubImage3D", CompressedTexSubImage3D)

      // Enhanced drawing operations
      ADD_WEBGL2_METHOD("drawRangeElements", DrawRangeElements)
      ADD_WEBGL2_METHOD("drawElementsInstanced", DrawElementsInstanced)
      ADD_WEBGL2_METHOD("drawArraysInstanced", DrawArraysInstanced)
      ADD_WEBGL2_METHOD("vertexAttribDivisor", VertexAttribDivisor)

#undef ADD_WEBGL2_METHOD
    }

    Local<Object> WebGL2RenderingContext::NewInstance(Isolate *isolate,
                                                      shared_ptr<client_graphics::WebGL2Context> handle)
    {
      EscapableHandleScope scope(isolate);
      assert(handle != nullptr && "WebGL2RenderingContext requires a valid WebGL2Context handle");
      return scope.Escape(WebGL2RenderingContextBase::NewInstance(isolate, handle).As<Object>());
    }

    // WebGL 2.0 specific methods
    void WebGL2RenderingContext::DrawBuffers(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native drawBuffers method
      }
    }

    void WebGL2RenderingContext::ClearBufferfv(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native clearBufferfv method
      }
    }

    void WebGL2RenderingContext::ClearBufferiv(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native clearBufferiv method
      }
    }

    void WebGL2RenderingContext::ClearBufferuiv(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native clearBufferuiv method
      }
    }

    void WebGL2RenderingContext::ClearBufferfi(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native clearBufferfi method
      }
    }

    // Query objects
    void WebGL2RenderingContext::CreateQuery(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native createQuery method
      }
    }

    void WebGL2RenderingContext::DeleteQuery(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native deleteQuery method
      }
    }

    void WebGL2RenderingContext::BeginQuery(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native beginQuery method
      }
    }

    void WebGL2RenderingContext::EndQuery(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native endQuery method
      }
    }

    void WebGL2RenderingContext::GetQuery(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getQuery method
      }
    }

    void WebGL2RenderingContext::GetQueryParameter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getQueryParameter method
      }
    }

    // Sampler objects
    void WebGL2RenderingContext::CreateSampler(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native createSampler method
      }
    }

    void WebGL2RenderingContext::DeleteSampler(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native deleteSampler method
      }
    }

    void WebGL2RenderingContext::BindSampler(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native bindSampler method
      }
    }

    void WebGL2RenderingContext::SamplerParameteri(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native samplerParameteri method
      }
    }

    void WebGL2RenderingContext::SamplerParameterf(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native samplerParameterf method
      }
    }

    void WebGL2RenderingContext::GetSamplerParameter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getSamplerParameter method
      }
    }

    // Sync objects
    void WebGL2RenderingContext::FenceSync(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native fenceSync method
      }
    }

    void WebGL2RenderingContext::IsSync(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native isSync method
      }
    }

    void WebGL2RenderingContext::DeleteSync(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native deleteSync method
      }
    }

    void WebGL2RenderingContext::ClientWaitSync(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native clientWaitSync method
      }
    }

    void WebGL2RenderingContext::WaitSync(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native waitSync method
      }
    }

    void WebGL2RenderingContext::GetSyncParameter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getSyncParameter method
      }
    }

    // Transform feedback
    void WebGL2RenderingContext::CreateTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native createTransformFeedback method
      }
    }

    void WebGL2RenderingContext::DeleteTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native deleteTransformFeedback method
      }
    }

    void WebGL2RenderingContext::BindTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native bindTransformFeedback method
      }
    }

    void WebGL2RenderingContext::BeginTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native beginTransformFeedback method
      }
    }

    void WebGL2RenderingContext::EndTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native endTransformFeedback method
      }
    }

    void WebGL2RenderingContext::TransformFeedbackVaryings(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native transformFeedbackVaryings method
      }
    }

    void WebGL2RenderingContext::GetTransformFeedbackVarying(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getTransformFeedbackVarying method
      }
    }

    void WebGL2RenderingContext::PauseTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native pauseTransformFeedback method
      }
    }

    void WebGL2RenderingContext::ResumeTransformFeedback(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native resumeTransformFeedback method
      }
    }

    // Uniform buffer objects
    void WebGL2RenderingContext::BindBufferBase(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native bindBufferBase method
      }
    }

    void WebGL2RenderingContext::BindBufferRange(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native bindBufferRange method
      }
    }

    void WebGL2RenderingContext::GetUniformIndices(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getUniformIndices method
      }
    }

    void WebGL2RenderingContext::GetActiveUniforms(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getActiveUniforms method
      }
    }

    void WebGL2RenderingContext::GetUniformBlockIndex(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getUniformBlockIndex method
      }
    }

    void WebGL2RenderingContext::GetActiveUniformBlockParameter(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getActiveUniformBlockParameter method
      }
    }

    void WebGL2RenderingContext::GetActiveUniformBlockName(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native getActiveUniformBlockName method
      }
    }

    void WebGL2RenderingContext::UniformBlockBinding(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native uniformBlockBinding method
      }
    }

    // Vertex array objects
    void WebGL2RenderingContext::CreateVertexArray(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native createVertexArray method
      }
    }

    void WebGL2RenderingContext::DeleteVertexArray(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native deleteVertexArray method
      }
    }

    void WebGL2RenderingContext::BindVertexArray(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native bindVertexArray method
      }
    }

    void WebGL2RenderingContext::IsVertexArray(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native isVertexArray method
      }
    }

    // Enhanced texture operations
    void WebGL2RenderingContext::TexImage3D(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native texImage3D method
      }
    }

    void WebGL2RenderingContext::TexSubImage3D(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native texSubImage3D method
      }
    }

    void WebGL2RenderingContext::CopyTexSubImage3D(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native copyTexSubImage3D method
      }
    }

    void WebGL2RenderingContext::CompressedTexImage3D(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native compressedTexImage3D method
      }
    }

    void WebGL2RenderingContext::CompressedTexSubImage3D(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native compressedTexSubImage3D method
      }
    }

    // Enhanced drawing operations
    void WebGL2RenderingContext::DrawRangeElements(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native drawRangeElements method
      }
    }

    void WebGL2RenderingContext::DrawElementsInstanced(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native drawElementsInstanced method
      }
    }

    void WebGL2RenderingContext::DrawArraysInstanced(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native drawArraysInstanced method
      }
    }

    void WebGL2RenderingContext::VertexAttribDivisor(const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      auto *self = Unwrap(isolate, args.This());
      if (self && self->handle())
      {
        // Implementation would call native vertexAttribDivisor method
      }
    }
  }
}
