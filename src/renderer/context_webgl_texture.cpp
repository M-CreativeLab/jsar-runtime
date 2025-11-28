#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

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
    glGenObjects(textures_, n, textures);
  }

  void TrContextWebGL::glGetTexParameter(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    // TODO(yorkie): implement
  }

  WebGLboolean TrContextWebGL::glIsTexture(WebGLuint texture)
  {
    // TODO(yorkie): implement
    return true;
  }

  void TrContextWebGL::glTexImage2D(WebGLenum target,
                                    WebGLint level,
                                    WebGLenum internalformat,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLsizei border,
                                    WebGLenum format,
                                    WebGLenum type,
                                    const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexImage3D(WebGLenum target,
                                    WebGLint level,
                                    WebGLenum internalformat,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLsizei depth,
                                    WebGLsizei border,
                                    WebGLenum format,
                                    WebGLenum type,
                                    const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  // --- Additional texture APIs ---
  void TrContextWebGL::glCompressedTexImage2D(WebGLenum target,
                                              WebGLint level,
                                              WebGLenum internalformat,
                                              WebGLsizei width,
                                              WebGLsizei height,
                                              WebGLsizei border,
                                              WebGLsizei imageSize,
                                              const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexImage3D(WebGLenum target,
                                              WebGLint level,
                                              WebGLenum internalformat,
                                              WebGLsizei width,
                                              WebGLsizei height,
                                              WebGLsizei depth,
                                              WebGLsizei border,
                                              WebGLsizei imageSize,
                                              const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexSubImage2D(WebGLenum target,
                                                 WebGLint level,
                                                 WebGLint xoffset,
                                                 WebGLint yoffset,
                                                 WebGLsizei width,
                                                 WebGLsizei height,
                                                 WebGLenum format,
                                                 WebGLsizei imageSize,
                                                 const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexSubImage3D(WebGLenum target,
                                                 WebGLint level,
                                                 WebGLint xoffset,
                                                 WebGLint yoffset,
                                                 WebGLint zoffset,
                                                 WebGLsizei width,
                                                 WebGLsizei height,
                                                 WebGLsizei depth,
                                                 WebGLenum format,
                                                 WebGLsizei imageSize,
                                                 const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameterf(WebGLenum target, WebGLenum pname, WebGLfloat param)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameteri(WebGLenum target, WebGLenum pname, WebGLint param)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameterfv(WebGLenum target, WebGLenum pname, const WebGLfloat *params)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameteriv(WebGLenum target, WebGLenum pname, const WebGLint *params)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexStorage2D(WebGLenum target,
                                      WebGLint levels,
                                      WebGLenum internalformat,
                                      WebGLsizei width,
                                      WebGLsizei height)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexStorage3D(WebGLenum target,
                                      WebGLint levels,
                                      WebGLenum internalformat,
                                      WebGLsizei width,
                                      WebGLsizei height,
                                      WebGLsizei depth)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexSubImage2D(WebGLenum target,
                                       WebGLint level,
                                       WebGLint xoffset,
                                       WebGLint yoffset,
                                       WebGLsizei width,
                                       WebGLsizei height,
                                       WebGLenum format,
                                       WebGLenum type,
                                       const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexSubImage3D(WebGLenum target,
                                       WebGLint level,
                                       WebGLint xoffset,
                                       WebGLint yoffset,
                                       WebGLint zoffset,
                                       WebGLsizei width,
                                       WebGLsizei height,
                                       WebGLsizei depth,
                                       WebGLenum format,
                                       WebGLenum type,
                                       const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }
}
