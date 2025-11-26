#include <renderer/context_webgl.hpp>
#include "command_buffers/details/texture.hpp"
#include "command_buffers/shared.hpp"

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  TrContextWebGL::TrContextWebGL()
  {
  }

  TrContextWebGL::~TrContextWebGL()
  {
  }

  void TrContextWebGL::receiveIncomingCall(const TrCommandBufferRequest &req)
  {
    switch (req.type)
    {
    case COMMAND_BUFFER_ACTIVE_TEXTURE_REQ:
    {
      const auto &typed_req = To<ActiveTextureCommandBufferRequest>(req);
      glActiveTexture(typed_req.activeUnit);
      break;
    }
    case COMMAND_BUFFER_BIND_TEXTURE_REQ:
    {
      const auto &typed_req = To<BindTextureCommandBufferRequest>(req);
      glBindTexture(typed_req.target, typed_req.texture);
      break;
    }
    case COMMAND_BUFFER_COPY_TEXTURE_IMAGE_2D_REQ:
    {
      const auto &typed_req = To<CopyTextureImage2DCommandBufferRequest>(req);
      glCopyTexImage2D(
        typed_req.target,
        typed_req.internalFormat,
        typed_req.level,
        typed_req.x,
        typed_req.y,
        typed_req.width,
        typed_req.height,
        typed_req.border);
      break;
    }
    case COMMAND_BUFFER_COPY_TEXTURE_SUB_IMAGE_2D_REQ:
    {
      const auto &typed_req = To<CopyTextureSubImage2DCommandBufferRequest>(req);
      glCopyTexSubImage2D(
        typed_req.target,
        typed_req.level,
        typed_req.xoffset,
        typed_req.yoffset,
        typed_req.x,
        typed_req.y,
        typed_req.width,
        typed_req.height);
      break;
    }
    case COMMAND_BUFFER_DELETE_TEXTURE_REQ:
    {
      const auto &typed_req = To<DeleteTextureCommandBufferRequest>(req);
      glDeleteTextures(1, (const WebGLuint *)&typed_req.texture);
      break;
    }
    case COMMAND_BUFFER_CREATE_TEXTURE_REQ:
    {
      const auto &typed_req = To<CreateTextureCommandBufferRequest>(req);
      WebGLint texture;
      glGenTextures(1, (WebGLuint *)&texture);
      break;
    }
    default:
      break;
    }
  }

  void TrContextWebGL::glActiveTexture(WebGLenum texture)
  {
    active_texture_ = texture;
  }

  void TrContextWebGL::glBindTexture(WebGLenum target, WebGLuint texture)
  {
    texture_bindings_[TextureTarget(target)] = {target, texture};
  }

  void TrContextWebGL::glCopyTexImage2D(WebGLenum target,
                                        WebGLenum internalformat,
                                        WebGLint level,
                                        WebGLint x,
                                        WebGLint y,
                                        WebGLsizei width,
                                        WebGLsizei height,
                                        WebGLint border)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCopyTexSubImage2D(WebGLenum target,
                                           WebGLint level,
                                           WebGLint xoffset,
                                           WebGLint yoffset,
                                           WebGLint x,
                                           WebGLint y,
                                           WebGLsizei width,
                                           WebGLsizei height)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glDeleteTextures(WebGLsizei n, const WebGLuint *textures)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glGenTextures(WebGLsizei n, WebGLuint *textures)
  {
    // TODO(yorkie): implement
  }
}
