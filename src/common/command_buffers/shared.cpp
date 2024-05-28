#include "./shared.hpp"

namespace commandbuffers
{
  std::string commandTypeToStr(CommandBufferType type)
  {
    switch (type)
    {
    case COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ:
      return "WebGLContextInitRequest";
    case COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES:
      return "WebGLContextInitResponse";
    case COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ:
      return "WebGL2ContextInitRequest";
    case COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES:
      return "WebGL2ContextInitResponse";
    case COMMAND_BUFFER_GET_EXTENSIONS_REQ:
      return "GetExtensionsRequest";
    case COMMAND_BUFFER_GET_EXTENSIONS_RES:
      return "GetExtensionsResponse";
    case COMMAND_BUFFER_CREATE_PROGRAM_REQ:
      return "CreateProgramRequest";
    case COMMAND_BUFFER_DELETE_PROGRAM_REQ:
      return "DeleteProgramRequest";
    case COMMAND_BUFFER_LINK_PROGRAM_REQ:
      return "LinkProgramRequest";
    case COMMAND_BUFFER_LINK_PROGRAM_RES:
      return "LinkProgramResponse";
    case COMMAND_BUFFER_USE_PROGRAM_REQ:
      return "UseProgramRequest";
    case COMMAND_BUFFER_GET_PROGRAM_PARAM_REQ:
      return "GetProgramParamRequest";
    case COMMAND_BUFFER_GET_PROGRAM_PARAM_RES:
      return "GetProgramParamResponse";
    case COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_REQ:
      return "GetProgramInfoLogRequest";
    case COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_RES:
      return "GetProgramInfoLogResponse";
    case COMMAND_BUFFER_ATTACH_SHADER_REQ:
      return "AttachShaderRequest";
    case COMMAND_BUFFER_DETACH_SHADER_REQ:
      return "DetachShaderRequest";
    case COMMAND_BUFFER_CREATE_SHADER_REQ:
      return "CreateShaderRequest";
    case COMMAND_BUFFER_DELETE_SHADER_REQ:
      return "DeleteShaderRequest";
    case COMMAND_BUFFER_COMPILE_SHADER_REQ:
      return "CompileShaderRequest";
    case COMMAND_BUFFER_SHADER_SOURCE_REQ:
      return "ShaderSourceRequest";
    case COMMAND_BUFFER_UNKNOWN:
    default:
      return "Unknown";
    }
  }
}
