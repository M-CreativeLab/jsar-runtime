#include <renderer/context_webgl.hpp>
#include <command_buffers/details/texture.hpp>
#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>

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

  // --- Rendering ---
  void TrContextWebGL::glClear(WebGLbitfield mask)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferuiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferfv(WebGLenum buffer, WebGLint drawbuffer, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferfi(WebGLenum buffer, WebGLint drawbuffer, WebGLfloat depth, WebGLint stencil)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearColor(WebGLfloat red, WebGLfloat green, WebGLfloat blue, WebGLfloat alpha)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearDepthf(WebGLfloat depth)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearStencil(WebGLint s)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFinish()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFlush()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReadBuffer(WebGLenum buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReadPixels(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height, WebGLenum format, WebGLenum type, WebGLvoid *pixels)
  {
    /* TODO(yorkie): implement */
  }

  // --- Framebuffers ---
  void TrContextWebGL::glBindFramebuffer(WebGLenum target, WebGLuint framebuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindRenderbuffer(WebGLenum target, WebGLuint renderbuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBlitFramebuffer(WebGLint srcX0,
                                         WebGLint srcY0,
                                         WebGLint srcX1,
                                         WebGLint srcY1,
                                         WebGLint dstX0,
                                         WebGLint dstY0,
                                         WebGLint dstX1,
                                         WebGLint dstY1,
                                         WebGLbitfield mask,
                                         WebGLenum filter)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCheckFramebufferStatus(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteFramebuffers(WebGLsizei n, const WebGLuint *framebuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteRenderbuffers(WebGLsizei n, const WebGLuint *renderbuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawBuffers(WebGLsizei n, const WebGLenum *buffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferRenderbuffer(WebGLenum target, WebGLenum attachment, WebGLenum renderbuffertarget, WebGLuint renderbuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferTexture2D(WebGLenum target, WebGLenum attachment, WebGLenum textarget, WebGLuint texture, WebGLint level)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferTextureLayer(WebGLenum target, WebGLenum attachment, WebGLuint texture, WebGLint level, WebGLint layer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenFramebuffers(WebGLsizei n, WebGLuint *framebuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenRenderbuffers(WebGLsizei n, WebGLuint *renderbuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenerateMipmap(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetFramebufferAttachmentParameteriv(WebGLenum target, WebGLenum attachment, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetRenderbufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glInvalidateFramebuffer(WebGLenum target, WebGLsizei n, const WebGLenum *attachments)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glInvalidateSubFramebuffer(WebGLenum target,
                                                  WebGLsizei n,
                                                  const WebGLenum *attachments,
                                                  WebGLint x,
                                                  WebGLint y,
                                                  WebGLsizei width,
                                                  WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsFramebuffer(WebGLuint framebuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsRenderbuffer(WebGLuint renderbuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glRenderbufferStorage(WebGLenum target, WebGLenum internalformat, WebGLsizei width, WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glRenderbufferStorageMultisample(WebGLenum target,
                                                        WebGLsizei samples,
                                                        WebGLenum internalformat,
                                                        WebGLsizei width,
                                                        WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  // --- Shaders ---
  void TrContextWebGL::glAttachShader(WebGLuint program, WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindAttribLocation(WebGLuint program, WebGLuint index, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCompileShader(WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCreateProgram()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCreateShader(WebGLenum type)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteShader(WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDetachShader(WebGLuint program, WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveAttrib(WebGLuint program,
                                         WebGLuint index,
                                         WebGLsizei maxLength,
                                         WebGLsizei *length,
                                         WebGLint *size,
                                         WebGLenum *type,
                                         WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniform(WebGLuint program,
                                          WebGLuint index,
                                          WebGLsizei maxLength,
                                          WebGLsizei *length,
                                          WebGLint *size,
                                          WebGLenum *type,
                                          WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniformBlockName(WebGLuint program,
                                                   WebGLuint index,
                                                   WebGLsizei maxLength,
                                                   WebGLsizei *length,
                                                   WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniformBlockiv(WebGLuint program,
                                                 WebGLuint index,
                                                 WebGLenum pname,
                                                 WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniformsiv(WebGLuint program,
                                             WebGLsizei count,
                                             const WebGLuint *uniforms,
                                             WebGLenum pname,
                                             WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetAttachedShaders(WebGLuint program, WebGLsizei maxCount, WebGLsizei *count, WebGLuint *shaders)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetAttribLocation(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetFragDataLocation(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetProgramBinary(WebGLuint program,
                                          WebGLsizei maxLength,
                                          WebGLsizei *length,
                                          WebGLenum binaryFormat,
                                          WebGLsizei *binaryLength,
                                          WebGLbyte *binary)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetProgramInfoLog(WebGLuint program, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetProgramiv(WebGLuint program, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderInfoLog(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderPrecisionFormat(WebGLenum shadertype, WebGLenum precisiontype, WebGLint *range, WebGLint *precision)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderSource(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *source)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderiv(WebGLuint shader, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformfv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLfloat *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformuiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLuint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformBlockIndex(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformIndices(WebGLuint program, WebGLsizei count, const WebGLchar **names, WebGLuint *indices)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformLocation(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsShader(WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glLinkProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glProgramBinary(WebGLuint program, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glProgramParameteri(WebGLuint program, WebGLenum pname, WebGLint param)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReleaseShaderCompiler()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glShaderBinary(WebGLuint shader, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glShaderSource(WebGLuint shader, WebGLsizei count, const WebGLchar **string, const WebGLint *length)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1f(WebGLuint location, WebGLfloat v0)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2f(WebGLuint location, WebGLfloat v0, WebGLfloat v1)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2, WebGLfloat v3)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1i(WebGLuint location, WebGLint v0)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2i(WebGLuint location, WebGLint v0, WebGLint v1)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2, WebGLint v3)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1ui(WebGLuint location, WebGLuint v0)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2ui(WebGLuint location, WebGLuint v0, WebGLuint v1)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2, WebGLuint v3)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix2x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix3x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix2x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix4x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix3x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix4x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformBlockBinding(WebGLuint program, WebGLuint uniformBlockIndex, WebGLuint bindingPoint)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUseProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glValidateProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  // --- Buffer Objects ---
  void TrContextWebGL::glBindBuffer(WebGLenum target, WebGLuint buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindBufferBase(WebGLenum target,
                                        WebGLuint bindingPoint,
                                        WebGLuint buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindBufferRange(WebGLenum target,
                                         WebGLuint bindingPoint,
                                         WebGLuint buffer,
                                         WebGLintptr offset,
                                         WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }
  void TrContextWebGL::glBufferData(WebGLenum target,
                                    WebGLsizeiptr size,
                                    const WebGLvoid *data,
                                    WebGLenum usage)
  {
    /* TODO(yorkie): implement */
  }
  void TrContextWebGL::glBufferSubData(WebGLenum target,
                                       WebGLintptr offset,
                                       WebGLsizeiptr size,
                                       const WebGLvoid *data)
  {
    /* TODO(yorkie): implement */
  }
  void TrContextWebGL::glCopyBufferSubData(WebGLenum readTarget,
                                           WebGLenum writeTarget,
                                           WebGLintptr readOffset,
                                           WebGLintptr writeOffset,
                                           WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteBuffers(WebGLsizei count, const WebGLuint *buffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDisableVertexAttribArray(WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawArrays(WebGLenum mode, WebGLint first, WebGLsizei count)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawArraysInstanced(WebGLenum mode,
                                             WebGLint first,
                                             WebGLsizei count,
                                             WebGLsizei instanceCount)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawElements(WebGLenum mode,
                                      WebGLsizei count,
                                      WebGLenum type,
                                      const WebGLvoid *indices)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawElementsInstanced(WebGLenum mode, WebGLsizei count, WebGLenum type, const WebGLvoid *indices, WebGLsizei instanceCount)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawRangeElements(WebGLenum mode, WebGLuint start, WebGLuint end, WebGLsizei count, WebGLenum type, const WebGLvoid *indices)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEnableVertexAttribArray(WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFlushMappedBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenBuffers(WebGLsizei n, WebGLuint *buffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetBufferParameter(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetBufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetBufferPointerv(WebGLenum target, WebGLenum pname, WebGLvoid **params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetVertexAttrib(WebGLuint index, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetVertexAttribPointerv(WebGLuint index, WebGLenum pname, WebGLvoid **pointer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsBuffer(WebGLuint buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glMapBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr length, WebGLbitfield access)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUnmapBuffer(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib(WebGLuint index, WebGLfloat x)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib1f(WebGLuint index, WebGLfloat x)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib2f(WebGLuint index, WebGLfloat x, WebGLfloat y)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib3f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib4f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z, WebGLfloat w)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib1fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib2fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib3fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib4fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttribDivisor(WebGLuint index, WebGLuint divisor)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttribPointer(WebGLuint index,
                                             WebGLint size,
                                             WebGLenum type,
                                             WebGLboolean normalized,
                                             WebGLsizei stride,
                                             WebGLintptr offset)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttribIPointer(WebGLuint index,
                                              WebGLint size,
                                              WebGLenum type,
                                              WebGLsizei stride,
                                              WebGLintptr offset)
  {
    /* TODO(yorkie): implement */
  }

  // --- State Management ---
  void TrContextWebGL::glBlendColor(WebGLfloat red,
                                    WebGLfloat green,
                                    WebGLfloat blue,
                                    WebGLfloat alpha)
  {
    blend_color_[0] = red;
    blend_color_[1] = green;
    blend_color_[2] = blue;
    blend_color_[3] = alpha;
  }

  void TrContextWebGL::glBlendEquation(WebGLenum mode)
  {
    blend_equation_rgb_ = mode;
    blend_equation_alpha_ = mode;
  }

  void TrContextWebGL::glBlendEquationSeparate(WebGLenum mode_rgb, WebGLenum mode_alpha)
  {
    blend_equation_rgb_ = mode_rgb;
    blend_equation_alpha_ = mode_alpha;
  }

  void TrContextWebGL::glBlendFunc(WebGLenum sfactor, WebGLenum dfactor)
  {
    blend_sfactor_rgb_ = sfactor;
    blend_dfactor_rgb_ = dfactor;
    blend_sfactor_alpha_ = sfactor;
    blend_dfactor_alpha_ = dfactor;
  }

  void TrContextWebGL::glBlendFuncSeparate(WebGLenum src_rgb,
                                           WebGLenum dst_rgb,
                                           WebGLenum src_alpha,
                                           WebGLenum dst_alpha)
  {
    blend_sfactor_rgb_ = src_rgb;
    blend_dfactor_rgb_ = dst_rgb;
    blend_sfactor_alpha_ = src_alpha;
    blend_dfactor_alpha_ = dst_alpha;
  }

  void TrContextWebGL::glColorMask(WebGLboolean red,
                                   WebGLboolean green,
                                   WebGLboolean blue,
                                   WebGLboolean alpha)
  {
    color_mask_[0] = red;
    color_mask_[1] = green;
    color_mask_[2] = blue;
    color_mask_[3] = alpha;
  }

  void TrContextWebGL::glCullFace(WebGLenum mode)
  {
    cull_face_ = mode;
  }

  void TrContextWebGL::glDepthFunc(WebGLenum func)
  {
    depth_func_ = func;
  }

  void TrContextWebGL::glDepthMask(WebGLboolean flag)
  {
    depth_mask_ = flag;
  }

  void TrContextWebGL::glDepthRangef(WebGLfloat near, WebGLfloat far)
  {
    depth_range_[0] = near;
    depth_range_[1] = far;
  }

  void TrContextWebGL::glDisable(WebGLenum cap)
  {
    caps_.disable(cap);
  }

  void TrContextWebGL::glEnable(WebGLenum cap)
  {
    caps_.enable(cap);
  }

  void TrContextWebGL::glFrontFace(WebGLenum mode)
  {
    front_face_ = mode;
  }

  void TrContextWebGL::glGet(WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  WebGLenum TrContextWebGL::glGetError()
  {
    WebGLenum error = last_error_;
    last_error_ = WEBGL_NO_ERROR;
    return error;
  }

  void TrContextWebGL::glHint(WebGLenum target, WebGLenum mode)
  { /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsEnabled(WebGLenum cap)
  {
    return caps_.isEnabled(cap);
  }

  void TrContextWebGL::glLineWidth(WebGLfloat width)
  {
    line_width_ = width;
  }

  void TrContextWebGL::glPixelStorei(WebGLenum pname, WebGLint param)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glPolygonOffset(WebGLfloat factor, WebGLfloat units)
  {
    polygon_offset_factor_ = factor;
    polygon_offset_units_ = units;
  }

  void TrContextWebGL::glSampleCoverage(WebGLfloat value, WebGLboolean invert)
  {
    sample_coverage_value_ = value;
    sample_coverage_invert_ = invert;
  }

  void TrContextWebGL::glScissor(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height)
  {
    scissor_box_[0] = x;
    scissor_box_[1] = y;
    scissor_box_[2] = width;
    scissor_box_[3] = height;
  }

  void TrContextWebGL::glStencilFunc(WebGLenum func, WebGLint ref, WebGLuint mask)
  {
    stencil_func_ = func;
    stencil_ref_ = ref;
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilFuncSeparate(WebGLenum face,
                                             WebGLenum func,
                                             WebGLint ref,
                                             WebGLuint mask)
  {
    stencil_func_ = func;
    stencil_ref_ = ref;
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilMask(WebGLuint mask)
  {
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilMaskSeparate(WebGLenum face, WebGLuint mask)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glStencilOp(WebGLenum fail, WebGLenum zfail, WebGLenum zpass)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glStencilOpSeparate(WebGLenum face, WebGLenum fail, WebGLenum zfail, WebGLenum zpass)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glViewport(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height)
  {
    viewport_[0] = x;
    viewport_[1] = y;
    viewport_[2] = width;
    viewport_[3] = height;
  }

  // --- Transform Feedback ---
  void TrContextWebGL::glBeginTransformFeedback(WebGLenum primitiveMode)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindTransformFeedback(WebGLenum target, WebGLuint transformFeedback)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteTransformFeedbacks(WebGLsizei n, const WebGLuint *transformFeedbacks)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEndTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenTransformFeedbacks(WebGLsizei n, WebGLuint *transformFeedbacks)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetTransformFeedbackVarying(WebGLuint program,
                                                     WebGLuint index,
                                                     WebGLsizei bufSize,
                                                     WebGLsizei *length,
                                                     WebGLsizei *size,
                                                     WebGLenum *type,
                                                     WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsTransformFeedback(WebGLuint transformFeedback)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glPauseTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glResumeTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glTransformFeedbackVaryings(WebGLuint program,
                                                   WebGLsizei count,
                                                   const WebGLchar **varyings,
                                                   WebGLenum bufferMode)
  {
    /* TODO(yorkie): implement */
  }

  // --- Utility ---
  void TrContextWebGL::glGetInternalformativ(WebGLenum target, WebGLenum internalformat, WebGLsizei propCount, WebGLenum *props, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetString(WebGLenum pname, WebGLchar *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetStringi(WebGLenum pname, WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }

  // --- Queries ---
  void TrContextWebGL::glBeginQuery(WebGLenum target, WebGLuint id)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteQueries(WebGLsizei n, const WebGLuint *ids)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEndQuery(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenQueries(WebGLsizei n, WebGLuint *ids)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetQueryObjectuiv(WebGLuint id, WebGLenum pname, WebGLuint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetQueryiv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsQuery(WebGLuint id)
  {
    /* TODO(yorkie): implement */
  }

  // --- Syncing ---
  void TrContextWebGL::glClientWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteSync(WebGLsync sync)
  {
    /* TODO(yorkie): implement */
  }

  WebGLsync TrContextWebGL::glFenceSync(WebGLenum condition, WebGLbitfield flags)
  {
    /* TODO(yorkie): implement */
    return nullptr;
  }

  void TrContextWebGL::glGetSynciv(WebGLsync sync, WebGLenum pname, WebGLsizei bufSize, WebGLsizei *length, WebGLint *values)
  {
    /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsSync(WebGLsync sync)
  {
    /* TODO(yorkie): implement */
    return false;
  }

  void TrContextWebGL::glWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout)
  {
    /* TODO(yorkie): implement */
  }

  // --- Vertex Array Objects ---
  void TrContextWebGL::glBindVertexArray(WebGLuint array)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteVertexArrays(WebGLsizei n, const WebGLuint *arrays)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenVertexArrays(WebGLsizei n, WebGLuint *arrays)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsVertexArray(WebGLuint array)
  {
    /* TODO(yorkie): implement */
  }

  // --- Samplers ---
  void TrContextWebGL::glBindSampler(WebGLuint unit, WebGLuint sampler)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteSamplers(WebGLsizei n, const WebGLuint *samplers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenSamplers(WebGLsizei n, WebGLuint *samplers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsSampler(WebGLuint sampler)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint param)
  {
    /* TODO(yorkie): implement */
  }
}
