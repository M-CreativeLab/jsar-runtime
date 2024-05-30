#include "./sender.hpp"
#include "./command_buffers.hpp"

namespace commandbuffers
{

  bool TrCommandBufferSender::sendCommandBufferRequest(TrCommandBufferBase &req)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (req.type)
    {
#define CASE_COMMAND_BUFFER_REQUEST(commandType, requestType) \
  case COMMAND_BUFFER_##commandType##_REQ:                    \
  {                                                           \
    message = dynamic_cast<requestType *>(&req)->serialize(); \
    break;                                                    \
  }
      CASE_COMMAND_BUFFER_REQUEST(WEBGL_CONTEXT_INIT, WebGL1ContextInitCommandBufferRequest);
      CASE_COMMAND_BUFFER_REQUEST(WEBGL2_CONTEXT_INIT, WebGL2ContextInitCommandBufferRequest);
      CASE_COMMAND_BUFFER_REQUEST(CREATE_PROGRAM, CreateProgramCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_PROGRAM, DeleteProgramCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(LINK_PROGRAM, LinkProgramCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(USE_PROGRAM, UseProgramCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_PROGRAM_PARAM, GetProgramParamCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_PROGRAM_INFO_LOG, GetProgramInfoLogCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(ATTACH_SHADER, AttachShaderCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DETACH_SHADER, DetachShaderCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CREATE_SHADER, CreateShaderCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_SHADER, DeleteShaderCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(SHADER_SOURCE, ShaderSourceCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(COMPILE_SHADER, CompileShaderCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_SHADER_SOURCE, GetShaderSourceCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_SHADER_PARAM, GetShaderParamCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_SHADER_INFO_LOG, GetShaderInfoLogCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CREATE_BUFFER, CreateBufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_BUFFER, DeleteBufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_BUFFER, BindBufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BUFFER_DATA, BufferDataCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BUFFER_SUB_DATA, BufferSubDataCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CREATE_FRAMEBUFFER, CreateFramebufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_FRAMEBUFFER, DeleteFramebufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_FRAMEBUFFER, BindFramebufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(FRAMEBUFFER_RENDERBUFFER, FramebufferRenderbufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(FRAMEBUFFER_TEXTURE2D, FramebufferTexture2DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CHECK_FRAMEBUFFER_STATUS, CheckFramebufferStatusCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CREATE_RENDERBUFFER, CreateRenderbufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_RENDERBUFFER, DeleteRenderbufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_RENDERBUFFER, BindRenderbufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(RENDERBUFFER_STORAGE, RenderbufferStorageCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(READ_BUFFER, ReadBufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_BUFFER_BASE, BindBufferBaseCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_BUFFER_RANGE, BindBufferRangeCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BLIT_FRAMEBUFFER, BlitFramebufferCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(RENDERBUFFER_STORAGE_MULTISAMPLE, RenderbufferStorageMultisampleCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CREATE_VERTEX_ARRAY, CreateVertexArrayCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_VERTEX_ARRAY, DeleteVertexArrayCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_VERTEX_ARRAY, BindVertexArrayCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CREATE_TEXTURE, CreateTextureCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DELETE_TEXTURE, DeleteTextureCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BIND_TEXTURE, BindTextureCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(TEXTURE_IMAGE_2D, TextureImage2DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(TEXTURE_SUB_IMAGE_2D, TextureSubImage2DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(COPY_TEXTURE_IMAGE_2D, CopyTextureImage2DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(COPY_TEXTURE_SUB_IMAGE_2D, CopyTextureSubImage2DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(TEXTURE_PARAMETERI, TextureParameteriCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(ACTIVE_TEXTURE, ActiveTextureCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GENERATE_MIPMAP, GenerateMipmapCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(TEXTURE_IMAGE_3D, TextureImage3DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(TEXTURE_SUB_IMAGE_3D, TextureSubImage3DCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(ENABLE_VERTEX_ATTRIB_ARRAY, EnableVertexAttribArrayCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DISABLE_VERTEX_ATTRIB_ARRAY, DisableVertexAttribArrayCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(VERTEX_ATTRIB_POINTER, VertexAttribPointerCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(VERTEX_ATTRIB_IPOINTER, VertexAttribIPointerCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(VERTEX_ATTRIB_DIVISOR, VertexAttribDivisorCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM_BLOCK_BINDING, UniformBlockBindingCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM1F, Uniform1fCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM1FV, Uniform1fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM1I, Uniform1iCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM1IV, Uniform1ivCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM2F, Uniform2fCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM2FV, Uniform2fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM2I, Uniform2iCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM2IV, Uniform2ivCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM3F, Uniform3fCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM3FV, Uniform3fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM3I, Uniform3iCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM3IV, Uniform3ivCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM4F, Uniform4fCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM4FV, Uniform4fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM4I, Uniform4iCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM4IV, Uniform4ivCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM_MATRIX2FV, UniformMatrix2fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM_MATRIX3FV, UniformMatrix3fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(UNIFORM_MATRIX4FV, UniformMatrix4fvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DRAW_ARRAYS, DrawArraysCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DRAW_ELEMENTS, DrawElementsCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DRAW_ARRAYS_INSTANCED, DrawArraysInstancedCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DRAW_ELEMENTS_INSTANCED, DrawElementsInstancedCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DRAW_RANGE_ELEMENTS, DrawRangeElementsCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DRAW_BUFFERS, DrawBuffersCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(PIXEL_STOREI, PixelStoreiCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(POLYGON_OFFSET, PolygonOffsetCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(SET_VIEWPORT, SetViewportCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(SET_SCISSOR, SetScissorCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_EXTENSIONS, GetExtensionsCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DEPTH_MASK, DepthMaskCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DEPTH_FUNC, DepthFuncCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DEPTH_RANGE, DepthRangeCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(STENCIL_FUNC, StencilFuncCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(STENCIL_FUNC_SEPARATE, StencilFuncSeparateCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(STENCIL_MASK, StencilMaskCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(STENCIL_MASK_SEPARATE, StencilMaskSeparateCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(STENCIL_OP, StencilOpCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(STENCIL_OP_SEPARATE, StencilOpSeparateCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BLEND_COLOR, BlendColorCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BLEND_EQUATION, BlendEquationCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BLEND_EQUATION_SEPARATE, BlendEquationSeparateCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BLEND_FUNC, BlendFuncCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(BLEND_FUNC_SEPARATE, BlendFuncSeparateCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(COLOR_MASK, ColorMaskCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(CULL_FACE, CullFaceCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(FRONT_FACE, FrontFaceCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(ENABLE, EnableCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(DISABLE, DisableCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_BOOLEANV, GetBooleanvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_INTEGERV, GetIntegervCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_FLOATV, GetFloatvCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_STRING, GetStringCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_SHADER_PRECISION_FORMAT,
                                  GetShaderPrecisionFormatCommandBufferRequest)
      CASE_COMMAND_BUFFER_REQUEST(GET_ERROR, GetErrorCommandBufferRequest)
#undef CASE_COMMAND_BUFFER_REQUEST
    default:
      break;
    };

    if (message == nullptr)
    {
      DEBUG(LOG_TAG_CONTENT, "Failed to serialize command buffer: %d", req.type);
      return false;
    }

    void *data = nullptr;
    size_t size = 0;
    if (!message->serialize(&data, &size))
      return false;

    return sendRaw(data, size);
  }

  bool TrCommandBufferSender::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (res.type)
    {
#define CASE_COMMAND_BUFFER_RESPONSE(commandType, responseType) \
  case COMMAND_BUFFER_##commandType##_RES:                      \
  {                                                             \
    message = dynamic_cast<responseType *>(&res)->serialize();  \
    break;                                                      \
  }
      CASE_COMMAND_BUFFER_RESPONSE(WEBGL_CONTEXT_INIT, WebGL1ContextInitCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(WEBGL2_CONTEXT_INIT, WebGL2ContextInitCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_EXTENSIONS, GetExtensionsCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(LINK_PROGRAM, LinkProgramCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_PROGRAM_PARAM, GetProgramParamCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_PROGRAM_INFO_LOG, GetProgramInfoLogCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_SOURCE, GetShaderSourceCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_PARAM, GetShaderParamCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_INFO_LOG, GetShaderInfoLogCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(CHECK_FRAMEBUFFER_STATUS, CheckFramebufferStatusCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(IS_VERTEX_ARRAY, IsVertexArrayCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SAMPLER_PARAMETER, GetSamplerParameterCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(IS_SAMPLER, IsSamplerCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_BOOLEANV, GetBooleanvCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_INTEGERV, GetIntegervCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_FLOATV, GetFloatvCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_STRING, GetStringCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_PRECISION_FORMAT, GetShaderPrecisionFormatCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_ERROR, GetErrorCommandBufferResponse)
#undef CASE_COMMAND_BUFFER_RESPONSE
    default:
      break;
    };

    if (message == nullptr)
    {
      DEBUG(LOG_TAG_CONTENT, "Failed to serialize command buffer: %d", res.type);
      return false;
    }

    void *data = nullptr;
    size_t size = 0;
    if (!message->serialize(&data, &size))
      return false;

    return sendRaw(data, size);
  }
}
