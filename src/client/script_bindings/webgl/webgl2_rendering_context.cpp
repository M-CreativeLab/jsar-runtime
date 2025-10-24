#include <client/script_bindings/canvas/image_source.hpp>
#include "./webgl2_rendering_context.hpp"
#include "./buffer.hpp"
#include "./program.hpp"
#include "./vertex_array.hpp"

namespace endor
{
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
        ADD_WEBGL2_METHOD("readBuffer", ReadBuffer)
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

        // Buffer objects
        ADD_WEBGL2_METHOD("bindBufferBase", BindBufferBase)
        ADD_WEBGL2_METHOD("bindBufferRange", BindBufferRange)
        ADD_WEBGL2_METHOD("vertexAttribIPointer", VertexAttribIPointer)
        ADD_WEBGL2_METHOD("vertexAttribI4i", VertexAttribI4i)
        ADD_WEBGL2_METHOD("vertexAttribI4ui", VertexAttribI4ui)
        ADD_WEBGL2_METHOD("vertexAttribI4iv", VertexAttribI4iv)
        ADD_WEBGL2_METHOD("vertexAttribI4uiv", VertexAttribI4uiv)

        // Uniform buffer objects
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

        // Enhanced framebuffer operations
        ADD_WEBGL2_METHOD("blitFramebuffer", BlitFramebuffer)
        ADD_WEBGL2_METHOD("renderbufferStorageMultisample", RenderbufferStorageMultisample)

        // Enhanced texture operations
        ADD_WEBGL2_METHOD("texImage3D", TexImage3D)
        ADD_WEBGL2_METHOD("texSubImage3D", TexSubImage3D)
        ADD_WEBGL2_METHOD("texStorage2D", TexStorage2D)
        ADD_WEBGL2_METHOD("texStorage3D", TexStorage3D)
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
      void WebGL2RenderingContext::ReadBuffer(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "readBuffer", 1, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "readBuffer", 0, "number", args[0])));
          return;
        }

        int src = args[0]->Int32Value(context).ToChecked();
        handle()->readBuffer(src);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::DrawBuffers(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "drawBuffers", 1, args.Length())));
          return;
        }
        if (!args[0]->IsArray())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "drawBuffers", 0, "Array", args[0])));
        }

        vector<uint32_t> buffers;
        Local<Array> array = args[0].As<Array>();
        uint32_t length = array->Length();
        buffers.reserve(length);

        for (uint32_t i = 0; i < length; ++i)
        {
          Local<Value> item = array->Get(context, i).ToLocalChecked();
          buffers.push_back(item->Uint32Value(context).ToChecked());
        }

        handle()->drawBuffers(buffers);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::ClearBufferfv(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "clearBufferfv", 3, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfv", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfv", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsArray() && !args[2]->IsFloat32Array())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfv", 2, "Array or Float32Array", args[1])));
        }

        client_graphics::WebGLFramebufferAttachmentType buffer;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          buffer = static_cast<client_graphics::WebGLFramebufferAttachmentType>(value);
        }
        int drawbuffer = args[1]->Int32Value(context).ToChecked();

        vector<float> values;
        {
          auto arg = args[2];
          if (arg->IsFloat32Array())
          {
            auto data = arg.As<Float32Array>();
            auto arrayBuffer = data->Buffer();
            if (arrayBuffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "clearBufferfv", "Invalid Float32Array")));
              return;
            }

            auto bufferData = static_cast<float *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset() / sizeof(float);
            values.assign(bufferData, bufferData + data->Length());
          }
          else if (arg->IsArray())
          {
            Local<Array> array = args[2].As<Array>();
            uint32_t length = array->Length();
            values.reserve(length);

            for (uint32_t i = 0; i < length; ++i)
            {
              Local<Value> item = array->Get(context, i).ToLocalChecked();
              values.push_back(static_cast<float>(item->NumberValue(context).ToChecked()));
            }
          }
        }

        if (args.Length() >= 4)
        {
          isolate->ThrowException(Exception::Error(
            MakeMethodError(isolate, "clearBufferfv", "Not supported for given srcOffset argument")));
        }
        else
        {
          handle()->clearBufferfv(buffer, drawbuffer, values);
        }
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::ClearBufferiv(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "clearBufferiv", 3, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferiv", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferiv", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsArray() && !args[2]->IsInt32Array())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferiv", 2, "Array or Int32Array", args[2])));
          return;
        }

        client_graphics::WebGLFramebufferAttachmentType buffer;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          buffer = static_cast<client_graphics::WebGLFramebufferAttachmentType>(value);
        }
        int drawbuffer = args[1]->Int32Value(context).ToChecked();

        vector<int32_t> values;
        {
          auto arg = args[2];
          if (arg->IsInt32Array())
          {
            auto data = arg.As<Int32Array>();
            auto arrayBuffer = data->Buffer();
            if (arrayBuffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "clearBufferiv", "Invalid Int32Array")));
              return;
            }

            auto bufferData = static_cast<int32_t *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset() / sizeof(int32_t);
            values.assign(bufferData, bufferData + data->Length());
          }
          else if (arg->IsArray())
          {
            Local<Array> array = args[2].As<Array>();
            uint32_t length = array->Length();
            values.reserve(length);

            for (uint32_t i = 0; i < length; ++i)
            {
              Local<Value> item = array->Get(context, i).ToLocalChecked();
              values.push_back(item->Int32Value(context).ToChecked());
            }
          }
        }

        handle()->clearBufferiv(buffer, drawbuffer, values);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::ClearBufferuiv(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "clearBufferuiv", 3, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferuiv", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferuiv", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsArray() && !args[2]->IsUint32Array())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferuiv", 2, "Array or Uint32Array", args[2])));
          return;
        }

        client_graphics::WebGLFramebufferAttachmentType buffer;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          buffer = static_cast<client_graphics::WebGLFramebufferAttachmentType>(value);
        }
        int drawbuffer = args[1]->Int32Value(context).ToChecked();

        vector<uint32_t> values;
        {
          auto arg = args[2];
          if (arg->IsUint32Array())
          {
            auto data = arg.As<Uint32Array>();
            auto arrayBuffer = data->Buffer();
            if (arrayBuffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "clearBufferuiv", "Invalid Uint32Array")));
              return;
            }

            auto bufferData = static_cast<uint32_t *>(arrayBuffer->GetBackingStore()->Data()) + data->ByteOffset() / sizeof(uint32_t);
            values.assign(bufferData, bufferData + data->Length());
          }
          else if (arg->IsArray())
          {
            Local<Array> array = args[2].As<Array>();
            uint32_t length = array->Length();
            values.reserve(length);

            for (uint32_t i = 0; i < length; ++i)
            {
              Local<Value> item = array->Get(context, i).ToLocalChecked();
              values.push_back(item->Uint32Value(context).ToChecked());
            }
          }
        }

        handle()->clearBufferuiv(buffer, drawbuffer, values);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::ClearBufferfi(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "clearBufferfi", 4, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfi", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfi", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfi", 2, "number", args[2])));
          return;
        }
        if (!args[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "clearBufferfi", 3, "number", args[3])));
          return;
        }

        client_graphics::WebGLFramebufferAttachmentType buffer;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          buffer = static_cast<client_graphics::WebGLFramebufferAttachmentType>(value);
        }
        int drawbuffer = args[1]->Int32Value(context).ToChecked();
        float depth = static_cast<float>(args[2]->NumberValue(context).ToChecked());
        int stencil = args[3]->Int32Value(context).ToChecked();

        handle()->clearBufferfi(buffer, drawbuffer, depth, stencil);
        args.GetReturnValue().SetUndefined();
      }

      // Query objects
      void WebGL2RenderingContext::CreateQuery(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "createQuery", "Not implemented")));
      }

      void WebGL2RenderingContext::DeleteQuery(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "deleteQuery", "Not implemented")));
      }

      void WebGL2RenderingContext::BeginQuery(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "beginQuery", "Not implemented")));
      }

      void WebGL2RenderingContext::EndQuery(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "endQuery", "Not implemented")));
      }

      void WebGL2RenderingContext::GetQuery(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getQuery", "Not implemented")));
      }

      void WebGL2RenderingContext::GetQueryParameter(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getQueryParameter", "Not implemented")));
      }

      // Sampler objects
      void WebGL2RenderingContext::CreateSampler(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "createSampler", "Not implemented")));
      }

      void WebGL2RenderingContext::DeleteSampler(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "deleteSampler", "Not implemented")));
      }

      void WebGL2RenderingContext::BindSampler(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "bindSampler", "Not implemented")));
      }

      void WebGL2RenderingContext::SamplerParameteri(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "samplerParameteri", "Not implemented")));
      }

      void WebGL2RenderingContext::SamplerParameterf(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "samplerParameterf", "Not implemented")));
      }

      void WebGL2RenderingContext::GetSamplerParameter(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getSamplerParameter", "Not implemented")));
      }

      // Sync objects
      void WebGL2RenderingContext::FenceSync(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "fenceSync", "Not implemented")));
      }

      void WebGL2RenderingContext::IsSync(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "isSync", "Not implemented")));
      }

      void WebGL2RenderingContext::DeleteSync(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "deleteSync", "Not implemented")));
      }

      void WebGL2RenderingContext::ClientWaitSync(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "clientWaitSync", "Not implemented")));
      }

      void WebGL2RenderingContext::WaitSync(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "waitSync", "Not implemented")));
      }

      void WebGL2RenderingContext::GetSyncParameter(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getSyncParameter", "Not implemented")));
      }

      // Transform feedback
      void WebGL2RenderingContext::CreateTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "createTransformFeedback", "Not implemented")));
      }

      void WebGL2RenderingContext::DeleteTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "deleteTransformFeedback", "Not implemented")));
      }

      void WebGL2RenderingContext::BindTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "bindTransformFeedback", "Not implemented")));
      }

      void WebGL2RenderingContext::BeginTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "beginTransformFeedback", "Not implemented")));
      }

      void WebGL2RenderingContext::EndTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "endTransformFeedback", "Not implemented")));
      }

      void WebGL2RenderingContext::TransformFeedbackVaryings(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "transformFeedbackVaryings", "Not implemented")));
      }

      void WebGL2RenderingContext::GetTransformFeedbackVarying(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getTransformFeedbackVarying", "Not implemented")));
      }

      void WebGL2RenderingContext::PauseTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "pauseTransformFeedback", "Not implemented")));
      }

      void WebGL2RenderingContext::ResumeTransformFeedback(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "resumeTransformFeedback", "Not implemented")));
      }

      // Uniform buffer objects
      void WebGL2RenderingContext::BindBufferBase(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "bindBufferBase", 2, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferBase", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferBase", 1, "number", args[1])));
          return;
        }
        if (!WebGLBuffer::IsInstanceOf(isolate, args[2]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferBase", 2, "WebGLBuffer", args[2])));
          return;
        }

        client_graphics::WebGLBufferBindingTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLBufferBindingTarget>(value);
        }
        int index = args[1]->Int32Value(context).ToChecked();

        auto bufferObj = args[2].As<Object>();
        auto bufferBinding = WebGLBuffer::Unwrap(isolate, bufferObj);

        handle()->bindBufferBase(target, index, bufferBinding->handle());
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::BindBufferRange(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "bindBufferRange", 5, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferRange", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferRange", 1, "number", args[1])));
          return;
        }
        if (!WebGLBuffer::IsInstanceOf(isolate, args[2]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferRange", 2, "WebGLBuffer", args[2])));
          return;
        }
        if (!args[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferRange", 3, "number", args[3])));
          return;
        }
        if (!args[4]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindBufferRange", 4, "number", args[4])));
          return;
        }

        client_graphics::WebGLBufferBindingTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLBufferBindingTarget>(value);
        }
        int index = args[1]->Int32Value(context).ToChecked();

        auto bufferObj = args[2].As<Object>();
        auto bufferBinding = WebGLBuffer::Unwrap(isolate, bufferObj);

        int offset = args[3]->Int32Value(context).ToChecked();
        int size = args[4]->Int32Value(context).ToChecked();

        handle()->bindBufferRange(target, index, bufferBinding->handle(), offset, size);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::VertexAttribIPointer(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "vertexAttribIPointer", 4, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "vertexAttribIPointer", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "vertexAttribIPointer", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "vertexAttribIPointer", 2, "number", args[2])));
          return;
        }
        if (!args[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "vertexAttribIPointer", 3, "number", args[3])));
          return;
        }

        uint32_t index = args[0]->Uint32Value(context).ToChecked();
        int size = args[1]->Int32Value(context).ToChecked();
        int type = args[2]->Int32Value(context).ToChecked();
        int stride = args[3]->Int32Value(context).ToChecked();
        int offset = 0;

        if (args.Length() > 4 && args[4]->IsNumber())
          offset = args[4]->Int32Value(context).ToChecked();

        handle()->vertexAttribIPointer(index, size, type, stride, offset);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::VertexAttribI4i(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "vertexAttribI4i", "Not implemented")));
      }

      void WebGL2RenderingContext::VertexAttribI4ui(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "vertexAttribI4ui", "Not implemented")));
      }

      void WebGL2RenderingContext::VertexAttribI4iv(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "vertexAttribI4iv", "Not implemented")));
      }

      void WebGL2RenderingContext::VertexAttribI4uiv(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "vertexAttribI4uiv", "Not implemented")));
      }

      void WebGL2RenderingContext::GetUniformIndices(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getUniformIndices", "Not implemented")));
      }

      void WebGL2RenderingContext::GetActiveUniforms(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getActiveUniforms", "Not implemented")));
      }

      void WebGL2RenderingContext::GetUniformBlockIndex(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "getUniformBlockIndex", 2, args.Length())));
          return;
        }
        if (!args[0]->IsObject())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "getUniformBlockIndex", 0, "WebGLProgram", args[0])));
          return;
        }
        if (!args[1]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "getUniformBlockIndex", 1, "string", args[1])));
          return;
        }

        auto programObj = args[0].As<Object>();
        auto program = WebGLProgram::Unwrap(isolate, programObj);

        String::Utf8Value blockName(isolate, args[1]);
        if (blockName.length() == 0)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "getUniformBlockIndex", "Invalid block name")));
          return;
        }

        int blockIndex = handle()->getUniformBlockIndex(program->handle(), *blockName);
        args.GetReturnValue().Set(Integer::New(isolate, blockIndex));
      }

      void WebGL2RenderingContext::GetActiveUniformBlockParameter(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getActiveUniformBlockParameter", "Not implemented")));
      }

      void WebGL2RenderingContext::GetActiveUniformBlockName(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getActiveUniformBlockName", "Not implemented")));
      }

      void WebGL2RenderingContext::UniformBlockBinding(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 3)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "uniformBlockBinding", 3, args.Length())));
          return;
        }
        if (!args[0]->IsObject())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "uniformBlockBinding", 0, "WebGLProgram", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "uniformBlockBinding", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "uniformBlockBinding", 2, "number", args[2])));
          return;
        }

        auto programObj = args[0].As<Object>();
        auto program = WebGLProgram::Unwrap(isolate, programObj);
        if (program == nullptr || !program->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "uniformBlockBinding", "Invalid WebGLProgram")));
          return;
        }

        uint32_t uniformBlockIndex = args[1]->Uint32Value(context).ToChecked();
        uint32_t uniformBlockBinding = args[2]->Uint32Value(context).ToChecked();

        handle()->uniformBlockBinding(program->handle(), uniformBlockIndex, uniformBlockBinding);
        args.GetReturnValue().SetUndefined();
      }

      // Vertex array objects
      void WebGL2RenderingContext::CreateVertexArray(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        auto vertexArray = handle()->createVertexArray();
        if (!vertexArray)
        {
          args.GetReturnValue().SetNull();
          return;
        }

        auto instance = WebGLVertexArray::NewInstance(isolate, vertexArray);
        args.GetReturnValue().Set(instance);
      }

      void WebGL2RenderingContext::DeleteVertexArray(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "deleteVertexArray", 1, args.Length())));
          return;
        }
        if (!WebGLVertexArray::IsInstanceOf(isolate, args[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "deleteVertexArray", 0, "WebGLVertexArray", args[0])));
          return;
        }

        auto vertexArrayObj = args[0].As<Object>();
        auto vertexArray = WebGLVertexArray::Unwrap(isolate, vertexArrayObj);
        if (vertexArray == nullptr || !vertexArray->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "deleteVertexArray", "Invalid WebGLVertexArray")));
          return;
        }

        handle()->deleteVertexArray(vertexArray->handle());
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::BindVertexArray(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "bindVertexArray", 1, args.Length())));
          return;
        }

        // Fast path for `bindVertexArray(null)`
        if (args[0]->IsNull() || args[0]->IsUndefined())
        {
          handle()->bindVertexArray(nullptr);
          args.GetReturnValue().SetUndefined();
          return;
        }

        if (!WebGLVertexArray::IsInstanceOf(isolate, args[0]))
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "bindVertexArray", 0, "WebGLVertexArray or null", args[0])));
          return;
        }
        auto vertexArrayObj = args[0].As<Object>();
        auto vertexArray = WebGLVertexArray::Unwrap(isolate, vertexArrayObj);
        if (vertexArray == nullptr || !vertexArray->hasData())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "bindVertexArray", "Invalid WebGLVertexArray")));
          return;
        }

        handle()->bindVertexArray(vertexArray->handle());
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::IsVertexArray(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.Length() < 1)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "isVertexArray", 1, args.Length())));
          return;
        }

        if (!WebGLVertexArray::IsInstanceOf(isolate, args[0]))
        {
          args.GetReturnValue().Set(Boolean::New(isolate, false));
          return;
        }

        auto vertexArrayObj = args[0].As<Object>();
        auto vertexArray = WebGLVertexArray::Unwrap(isolate, vertexArrayObj);
        if (vertexArray == nullptr || !vertexArray->hasData())
        {
          args.GetReturnValue().Set(Boolean::New(isolate, false));
          return;
        }

        bool result = handle()->isVertexArray(vertexArray->handle());
        args.GetReturnValue().Set(Boolean::New(isolate, result));
      }

      void WebGL2RenderingContext::BlitFramebuffer(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 10)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "blitFramebuffer", 10, args.Length())));
          return;
        }
        for (int i = 0; i < 10; ++i)
        {
          if (!args[i]->IsNumber())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "blitFramebuffer", i, "number", args[i])));
            return;
          }
        }

        int srcX0 = args[0]->Int32Value(context).ToChecked();
        int srcY0 = args[1]->Int32Value(context).ToChecked();
        int srcX1 = args[2]->Int32Value(context).ToChecked();
        int srcY1 = args[3]->Int32Value(context).ToChecked();
        int dstX0 = args[4]->Int32Value(context).ToChecked();
        int dstY0 = args[5]->Int32Value(context).ToChecked();
        int dstX1 = args[6]->Int32Value(context).ToChecked();
        int dstY1 = args[7]->Int32Value(context).ToChecked();
        uint32_t mask = args[8]->Uint32Value(context).ToChecked();
        uint32_t filter = args[9]->Uint32Value(context).ToChecked();

        handle()->blitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::RenderbufferStorageMultisample(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "renderbufferStorageMultisample", 4, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "renderbufferStorageMultisample", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "renderbufferStorageMultisample", 1, "number", args[1])));
          return;
        }
        if (!args[2]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "renderbufferStorageMultisample", 2, "number", args[2])));
          return;
        }
        if (!args[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "renderbufferStorageMultisample", 3, "number", args[3])));
          return;
        }

        client_graphics::WebGLRenderbufferBindingTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLRenderbufferBindingTarget>(value);
        }
        int samples = args[1]->Int32Value(context).ToChecked();
        int internalformat = args[2]->Int32Value(context).ToChecked();
        int width = args[3]->Int32Value(context).ToChecked();
        int height = args[4]->Int32Value(context).ToChecked();

        handle()->renderbufferStorageMultisample(target, samples, internalformat, width, height);
        args.GetReturnValue().SetUndefined();
      }

      // Enhanced texture operations
      void WebGL2RenderingContext::TexImage3D(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 10)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "texImage3D", 10, args.Length())));
          return;
        }

        if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() ||
            !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber() ||
            !args[6]->IsNumber() || !args[7]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texImage3D", "Invalid argument types")));
          return;
        }

        client_graphics::WebGLTexture3DTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLTexture3DTarget>(value);
        }
        int level = args[1]->Int32Value(context).ToChecked();
        int internalformat = args[2]->Int32Value(context).ToChecked();
        int width = args[3]->Int32Value(context).ToChecked();
        int height = args[4]->Int32Value(context).ToChecked();
        int depth = args[5]->Int32Value(context).ToChecked();
        int border = args[6]->Int32Value(context).ToChecked();
        client_graphics::WebGLTextureFormat format;
        {
          int value = args[7]->Int32Value(context).ToChecked();
          format = static_cast<client_graphics::WebGLTextureFormat>(value);
        }
        client_graphics::WebGLPixelType pixelType;
        {
          int value = args[8]->Int32Value(context).ToChecked();
          pixelType = static_cast<client_graphics::WebGLPixelType>(value);
        }

        SkPixmap imagePixmap;
        unsigned char *pixels = nullptr;

        if (!args[9]->IsNullOrUndefined())
        {
          auto arg = args[9];
          if (arg->IsArrayBufferView())
          {
            auto data = arg.As<ArrayBufferView>();
            auto buffer = data->Buffer();
            if (buffer.IsEmpty())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "texImage3D", "Invalid srcData")));
              return;
            }
            pixels = static_cast<uint8_t *>(buffer->GetBackingStore()->Data()) + data->ByteOffset();
          }
          else
          {
            auto imageSource = canvas_bindings::GetImageSourceFromValue(isolate, arg);
            if (imageSource == nullptr)
            {
              auto msg =
                "ImageData, HTMLImageElement, HTMLCanvasElement, HTMLVideoElement, ImageBitmap, OffscreenCanvas"
                " or null object";
              isolate->ThrowException(Exception::TypeError(
                MakeMethodArgTypeError(isolate, "texImage2D", "pixels", msg, arg)));
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
              pixels = reinterpret_cast<unsigned char *>(imagePixmap.writable_addr());
            }
          }
        }

        handle()->texImage3D(target,
                             level,
                             internalformat,
                             width,
                             height,
                             depth,
                             border,
                             format,
                             pixelType,
                             pixels);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::TexSubImage3D(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 11)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "texSubImage3D", 11, args.Length())));
          return;
        }

        if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() ||
            !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber() ||
            !args[6]->IsNumber() || !args[7]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texSubImage3D", "Invalid argument types")));
          return;
        }

        client_graphics::WebGLTexture3DTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLTexture3DTarget>(value);
        }
        int level = args[1]->Int32Value(context).ToChecked();
        int xoffset = args[2]->Int32Value(context).ToChecked();
        int yoffset = args[3]->Int32Value(context).ToChecked();
        int zoffset = args[4]->Int32Value(context).ToChecked();
        int width = args[5]->Int32Value(context).ToChecked();
        int height = args[6]->Int32Value(context).ToChecked();
        int depth = args[7]->Int32Value(context).ToChecked();
        client_graphics::WebGLTextureFormat format;
        {
          int value = args[8]->Int32Value(context).ToChecked();
          format = static_cast<client_graphics::WebGLTextureFormat>(value);
        }
        client_graphics::WebGLPixelType pixelType;
        {
          int value = args[9]->Int32Value(context).ToChecked();
          pixelType = static_cast<client_graphics::WebGLPixelType>(value);
        }

        unsigned char *pixels = nullptr;
        if (!args[10]->IsNullOrUndefined())
        {
          if (!args[10]->IsArrayBufferView())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodArgTypeError(isolate, "texSubImage3D", 10, "ArrayBufferView", args[10])));
            return;
          }

          auto data = args[10].As<ArrayBufferView>();
          auto buffer = data->Buffer();
          if (buffer.IsEmpty())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeMethodError(isolate, "texSubImage3D", "Invalid ArrayBufferView")));
            return;
          }

          pixels = static_cast<uint8_t *>(buffer->GetBackingStore()->Data()) + data->ByteOffset();
        }

        handle()->texSubImage3D(target,
                                level,
                                xoffset,
                                yoffset,
                                zoffset,
                                width,
                                height,
                                depth,
                                format,
                                pixelType,
                                pixels);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::TexStorage2D(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "texStorage2D", 4, args.Length())));
          return;
        }

        if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texStorage2D", "Invalid argument types")));
          return;
        }

        client_graphics::WebGLTexture2DTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int levels = args[1]->Int32Value(context).ToChecked();
        int internalformat = args[2]->Int32Value(context).ToChecked();
        int width = args[3]->Int32Value(context).ToChecked();
        int height = args[4]->Int32Value(context).ToChecked();

        handle()->texStorage2D(target, levels, internalformat, width, height);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::TexStorage3D(const v8::FunctionCallbackInfo<v8::Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "texStorage3D", 5, args.Length())));
          return;
        }

        if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() ||
            !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "texStorage3D", "Invalid argument types")));
          return;
        }

        client_graphics::WebGLTexture3DTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLTexture3DTarget>(value);
        }
        int levels = args[1]->Int32Value(context).ToChecked();
        int internalformat = args[2]->Int32Value(context).ToChecked();
        int width = args[3]->Int32Value(context).ToChecked();
        int height = args[4]->Int32Value(context).ToChecked();
        int depth = args[5]->Int32Value(context).ToChecked();

        handle()->texStorage3D(target, levels, internalformat, width, height, depth);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::CopyTexSubImage3D(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 9)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "copyTexSubImage3D", 9, args.Length())));
          return;
        }

        if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() ||
            !args[3]->IsNumber() || !args[4]->IsNumber() || !args[5]->IsNumber() ||
            !args[6]->IsNumber() || !args[7]->IsNumber() || !args[8]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "copyTexSubImage3D", "Invalid argument types")));
          return;
        }

        client_graphics::WebGLTexture2DTarget target;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          target = static_cast<client_graphics::WebGLTexture2DTarget>(value);
        }
        int level = args[1]->Int32Value(context).ToChecked();
        int xoffset = args[2]->Int32Value(context).ToChecked();
        int yoffset = args[3]->Int32Value(context).ToChecked();
        int zoffset = args[4]->Int32Value(context).ToChecked();
        int x = args[5]->Int32Value(context).ToChecked();
        int y = args[6]->Int32Value(context).ToChecked();
        int width = args[7]->Int32Value(context).ToChecked();
        int height = args[8]->Int32Value(context).ToChecked();

        handle()->copyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::CompressedTexImage3D(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "compressedTexImage3D", "Not implemented")));
      }

      void WebGL2RenderingContext::CompressedTexSubImage3D(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "compressedTexSubImage3D", "Not implemented")));
      }

      // Enhanced drawing operations
      void WebGL2RenderingContext::DrawRangeElements(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 6)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "drawRangeElements", 6, args.Length())));
          return;
        }

        client_graphics::WebGLDrawMode mode;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          mode = static_cast<client_graphics::WebGLDrawMode>(value);
        }
        uint32_t start = args[1]->Uint32Value(context).ToChecked();
        uint32_t end = args[2]->Uint32Value(context).ToChecked();
        int count = args[3]->Int32Value(context).ToChecked();
        int type = args[4]->Int32Value(context).ToChecked();
        int offset = args[5]->Int32Value(context).ToChecked();

        handle()->drawRangeElements(mode, start, end, count, type, offset);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::DrawElementsInstanced(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 5)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "drawElementsInstanced", 5, args.Length())));
          return;
        }

        client_graphics::WebGLDrawMode mode;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          mode = static_cast<client_graphics::WebGLDrawMode>(value);
        }
        int count = args[1]->Int32Value(context).ToChecked();
        int type = args[2]->Int32Value(context).ToChecked();
        int offset = args[3]->Int32Value(context).ToChecked();
        int instanceCount = args[4]->Int32Value(context).ToChecked();

        handle()->drawElementsInstanced(mode, count, type, offset, instanceCount);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::DrawArraysInstanced(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 4)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "drawArraysInstanced", 4, args.Length())));
          return;
        }

        client_graphics::WebGLDrawMode mode;
        {
          int value = args[0]->Int32Value(context).ToChecked();
          mode = static_cast<client_graphics::WebGLDrawMode>(value);
        }
        int first = args[1]->Int32Value(context).ToChecked();
        int count = args[2]->Int32Value(context).ToChecked();
        int instanceCount = args[3]->Int32Value(context).ToChecked();

        handle()->drawArraysInstanced(mode, first, count, instanceCount);
        args.GetReturnValue().SetUndefined();
      }

      void WebGL2RenderingContext::VertexAttribDivisor(const FunctionCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 2)
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgCountError(isolate, "vertexAttribDivisor", 2, args.Length())));
          return;
        }
        if (!args[0]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "vertexAttribDivisor", 0, "number", args[0])));
          return;
        }
        if (!args[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodArgTypeError(isolate, "vertexAttribDivisor", 1, "number", args[1])));
          return;
        }

        uint32_t index = args[0]->Uint32Value(context).ToChecked();
        uint32_t divisor = args[1]->Uint32Value(context).ToChecked();

        handle()->vertexAttribDivisor(index, divisor);
        args.GetReturnValue().SetUndefined();
      }
    }
  }
} // namespace endor
