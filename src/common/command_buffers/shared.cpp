#include "./shared.hpp"
#include "./macros.hpp"

using namespace std;

namespace commandbuffers
{
  string commandTypeToStr(CommandBufferType type)
  {
    switch (type)
    {

// Request
#define XX(COMMAND_TYPE, _, STR)            \
  case COMMAND_BUFFER_##COMMAND_TYPE##_REQ: \
  {                                         \
    return STR;                             \
  }
      TR_COMMAND_BUFFER_REQUESTS_MAP(XX)
#undef XX

// Response
#define XX(COMMAND_TYPE, _, STR)            \
  case COMMAND_BUFFER_##COMMAND_TYPE##_RES: \
  {                                         \
    return STR;                             \
  }
      TR_COMMAND_BUFFER_RESPONSES_MAP(XX)
#undef XX

    case COMMAND_BUFFER_UNKNOWN:
    default:
      return "Unknown";
    }
  }

  // static
  bool CommandTypes::IsXRFrameControl(CommandBufferType type)
  {
    return type == COMMAND_BUFFER_XRFRAME_START_REQ ||
           type == COMMAND_BUFFER_XRFRAME_FLUSH_REQ ||
           type == COMMAND_BUFFER_XRFRAME_END_REQ;
  }

  // static
  bool CommandTypes::IsFramebufferDependentCommand(CommandBufferType type)
  {
    switch (type)
    {
    // --- Framebuffer binding & attachment ---
    case COMMAND_BUFFER_BIND_FRAMEBUFFER_REQ:
    case COMMAND_BUFFER_FRAMEBUFFER_RENDERBUFFER_REQ:
    case COMMAND_BUFFER_FRAMEBUFFER_TEXTURE2D_REQ:
    case COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_REQ:
    case COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES:
    case COMMAND_BUFFER_READ_BUFFER_REQ:
    case COMMAND_BUFFER_BLIT_FRAMEBUFFER_REQ:
    case COMMAND_BUFFER_RENDERBUFFER_STORAGE_REQ:
    case COMMAND_BUFFER_RENDERBUFFER_STORAGE_MULTISAMPLE_REQ:
    case COMMAND_BUFFER_BIND_RENDERBUFFER_REQ:
      return true;

    // --- Program / Shader states ---
    case COMMAND_BUFFER_USE_PROGRAM_REQ:
      return true;

    // --- Vertex Inputs ---
    // case COMMAND_BUFFER_BIND_VERTEX_ARRAY_REQ:
    // case COMMAND_BUFFER_BIND_BUFFER_REQ:
    // case COMMAND_BUFFER_BUFFER_DATA_REQ:
    // case COMMAND_BUFFER_BUFFER_SUB_DATA_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_POINTER_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_IPOINTER_REQ:
    // case COMMAND_BUFFER_ENABLE_VERTEX_ATTRIB_ARRAY_REQ:
    // case COMMAND_BUFFER_DISABLE_VERTEX_ATTRIB_ARRAY_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_DIVISOR_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_1F_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_2F_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_3F_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_4F_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_I4I_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_I4UI_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_I4IV_REQ:
    // case COMMAND_BUFFER_VERTEX_ATTRIB_I4UIV_REQ:
    //   return true;

    // --- Texture / Sampler ---
    case COMMAND_BUFFER_ACTIVE_TEXTURE_REQ:
    // case COMMAND_BUFFER_BIND_TEXTURE_REQ:
    // case COMMAND_BUFFER_BIND_SAMPLER_REQ:
    case COMMAND_BUFFER_TEXTURE_PARAMETERI_REQ:
    case COMMAND_BUFFER_TEXTURE_PARAMETERF_REQ:
    case COMMAND_BUFFER_SAMPLER_PARAMETERI_REQ:
    case COMMAND_BUFFER_SAMPLER_PARAMETERF_REQ:
    case COMMAND_BUFFER_TEXTURE_IMAGE_2D_REQ:
    case COMMAND_BUFFER_TEXTURE_IMAGE_3D_REQ:
    case COMMAND_BUFFER_TEXTURE_SUB_IMAGE_2D_REQ:
    case COMMAND_BUFFER_TEXTURE_SUB_IMAGE_3D_REQ:
    case COMMAND_BUFFER_GENERATE_MIPMAP_REQ:
      return true;

    // --- Uniforms ---
    case COMMAND_BUFFER_UNIFORM1F_REQ:
    case COMMAND_BUFFER_UNIFORM1FV_REQ:
    case COMMAND_BUFFER_UNIFORM1I_REQ:
    case COMMAND_BUFFER_UNIFORM1IV_REQ:
    case COMMAND_BUFFER_UNIFORM2F_REQ:
    case COMMAND_BUFFER_UNIFORM2FV_REQ:
    case COMMAND_BUFFER_UNIFORM2I_REQ:
    case COMMAND_BUFFER_UNIFORM2IV_REQ:
    case COMMAND_BUFFER_UNIFORM3F_REQ:
    case COMMAND_BUFFER_UNIFORM3FV_REQ:
    case COMMAND_BUFFER_UNIFORM3I_REQ:
    case COMMAND_BUFFER_UNIFORM3IV_REQ:
    case COMMAND_BUFFER_UNIFORM4F_REQ:
    case COMMAND_BUFFER_UNIFORM4FV_REQ:
    case COMMAND_BUFFER_UNIFORM4I_REQ:
    case COMMAND_BUFFER_UNIFORM4IV_REQ:
    case COMMAND_BUFFER_UNIFORM_MATRIX2FV_REQ:
    case COMMAND_BUFFER_UNIFORM_MATRIX3FV_REQ:
    case COMMAND_BUFFER_UNIFORM_MATRIX4FV_REQ:
      return true;

    // --- Draw calls (write to framebuffer) ---
    case COMMAND_BUFFER_DRAW_ARRAYS_REQ:
    case COMMAND_BUFFER_DRAW_ARRAYS_INSTANCED_REQ:
    case COMMAND_BUFFER_DRAW_ELEMENTS_REQ:
    case COMMAND_BUFFER_DRAW_ELEMENTS_INSTANCED_REQ:
    case COMMAND_BUFFER_DRAW_RANGE_ELEMENTS_REQ:
    case COMMAND_BUFFER_DRAW_BUFFERS_REQ:
      return true;

    // --- Clear (modify framebuffer attachments) ---
    case COMMAND_BUFFER_CLEAR_REQ:
    case COMMAND_BUFFER_CLEAR_COLOR_REQ:
    case COMMAND_BUFFER_CLEAR_DEPTH_REQ:
    case COMMAND_BUFFER_CLEAR_STENCIL_REQ:
    case COMMAND_BUFFER_CLEAR_BUFFERFV_REQ:
    case COMMAND_BUFFER_CLEAR_BUFFERIV_REQ:
    case COMMAND_BUFFER_CLEAR_BUFFERUIV_REQ:
    case COMMAND_BUFFER_CLEAR_BUFFERFI_REQ:
      return true;

    // --- Framebuffer size/region dependent states ---
    case COMMAND_BUFFER_SET_VIEWPORT_REQ:
    case COMMAND_BUFFER_SET_SCISSOR_REQ:
    case COMMAND_BUFFER_DEPTH_FUNC_REQ:
    case COMMAND_BUFFER_DEPTH_MASK_REQ:
    case COMMAND_BUFFER_STENCIL_FUNC_REQ:
    case COMMAND_BUFFER_STENCIL_FUNC_SEPARATE_REQ:
    case COMMAND_BUFFER_STENCIL_MASK_REQ:
    case COMMAND_BUFFER_STENCIL_MASK_SEPARATE_REQ:
    case COMMAND_BUFFER_STENCIL_OP_REQ:
    case COMMAND_BUFFER_STENCIL_OP_SEPARATE_REQ:
    case COMMAND_BUFFER_BLEND_COLOR_REQ:
    case COMMAND_BUFFER_BLEND_EQUATION_REQ:
    case COMMAND_BUFFER_BLEND_EQUATION_SEPARATE_REQ:
    case COMMAND_BUFFER_BLEND_FUNC_REQ:
    case COMMAND_BUFFER_BLEND_FUNC_SEPARATE_REQ:
    case COMMAND_BUFFER_COLOR_MASK_REQ:
    case COMMAND_BUFFER_POLYGON_OFFSET_REQ:
    case COMMAND_BUFFER_CULL_FACE_REQ:
    case COMMAND_BUFFER_FRONT_FACE_REQ:
    case COMMAND_BUFFER_ENABLE_REQ:
    case COMMAND_BUFFER_DISABLE_REQ:
    case COMMAND_BUFFER_LINE_WIDTH_REQ:
      return true;

    default:
      return false;
    }
  }

  // static
  bool CommandTypes::IsResourceCreatingCommand(CommandBufferType type)
  {
    switch (type)
    {
    // --- Shader / Program ---
    case COMMAND_BUFFER_CREATE_SHADER_REQ:
    case COMMAND_BUFFER_CREATE_PROGRAM_REQ:
      return true;

    // --- Buffer objects ---
    case COMMAND_BUFFER_CREATE_BUFFER_REQ:
      return true;

    // --- Framebuffer / Renderbuffer ---
    case COMMAND_BUFFER_CREATE_FRAMEBUFFER_REQ:
    case COMMAND_BUFFER_CREATE_RENDERBUFFER_REQ:
      return true;

    // --- Vertex Array ---
    case COMMAND_BUFFER_CREATE_VERTEX_ARRAY_REQ:
      return true;

    // --- Texture objects ---
    case COMMAND_BUFFER_CREATE_TEXTURE_REQ:
      return true;

    // --- Sampler objects ---
    case COMMAND_BUFFER_CREATE_SAMPLER_REQ:
      return true;

    // --- Transform Feedback ---
    case COMMAND_BUFFER_CREATE_TRANSFORM_FEEDBACK_REQ:
      return true;

    default:
      return false;
    }
  }

}
