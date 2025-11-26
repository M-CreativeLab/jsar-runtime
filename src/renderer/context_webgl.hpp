#pragma once

#include <unordered_map>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/webgl_constants.hpp>

namespace renderer
{
  class TrContextWebGL
  {
  public:
    TrContextWebGL();
    ~TrContextWebGL();

    class TextureTarget
    {
    public:
      enum
      {
        k2D = WEBGL_TEXTURE_2D,
        k3D = WEBGL2_TEXTURE_3D,
        k2DArray = WEBGL2_TEXTURE_2D_ARRAY,
      };

      TextureTarget(WebGLenum target)
          : target_(target)
      {
        assert(target_ == k2D || target_ == k3D || target_ == k2DArray);
      }

      inline WebGLenum value() const
      {
        return target_;
      }

      inline bool operator==(const TextureTarget &rhs) const
      {
        return target_ == rhs.target_;
      }

      struct HashKey
      {
        size_t operator()(const TextureTarget &t) const noexcept
        {
          return std::hash<WebGLenum>{}(t.value());
        }
      };

    private:
      WebGLenum target_;
    };

    struct TextureBinding
    {
      WebGLenum target;
      WebGLuint texture;
    };

    struct FramebufferBinding
    {
      WebGLenum target;
      WebGLuint framebuffer;
    };

    struct RenderbufferBinding
    {
      WebGLenum target;
      WebGLuint renderbuffer;
    };

    struct Capabilities
    {
      using Map = std::unordered_map<WebGLenum, WebGLboolean>;

      inline void enable(WebGLenum cap)
      {
        caps_[cap] = true;
      }

      inline void disable(WebGLenum cap)
      {
        caps_[cap] = false;
      }

      inline WebGLboolean isEnabled(WebGLenum cap) const
      {
        auto it = caps_.find(cap);
        return it != caps_.end() ? it->second : false;
      }

    private:
      Map caps_;
    };

    void receiveIncomingCall(const commandbuffers::TrCommandBufferRequest &);

  private:
    /**
     * @brief Convert the request to the given type.
     * 
     * @tparam T The type to convert.
     * @param req The request to convert.
     * @return const T& The converted request.
     */
    template <typename T>
    static const T &To(const commandbuffers::TrCommandBufferRequest &req)
    {
      return dynamic_cast<const T &>(req);
    }

    // Textures
    void glActiveTexture(WebGLenum texture);
    void glBindTexture(WebGLenum target, WebGLuint texture);
    void glCompressedTexImage2D(WebGLenum target,
                                WebGLint level,
                                WebGLenum internalformat,
                                WebGLsizei width,
                                WebGLsizei height,
                                WebGLsizei border,
                                WebGLsizei imageSize,
                                const WebGLvoid *data);
    void glCompressedTexImage3D(WebGLenum target,
                                WebGLint level,
                                WebGLenum internalformat,
                                WebGLsizei width,
                                WebGLsizei height,
                                WebGLsizei depth,
                                WebGLsizei border,
                                WebGLsizei imageSize,
                                const WebGLvoid *data);
    void glCompressedTexSubImage2D(WebGLenum target,
                                   WebGLint level,
                                   WebGLint xoffset,
                                   WebGLint yoffset,
                                   WebGLsizei width,
                                   WebGLsizei height,
                                   WebGLenum format,
                                   WebGLsizei imageSize,
                                   const WebGLvoid *data);
    void glCompressedTexSubImage3D(WebGLenum target,
                                   WebGLint level,
                                   WebGLint xoffset,
                                   WebGLint yoffset,
                                   WebGLint zoffset,
                                   WebGLsizei width,
                                   WebGLsizei height,
                                   WebGLsizei depth,
                                   WebGLenum format,
                                   WebGLsizei imageSize,
                                   const WebGLvoid *data);
    void glCopyTexImage2D(WebGLenum target,
                          WebGLenum internalformat,
                          WebGLint level,
                          WebGLint x,
                          WebGLint y,
                          WebGLsizei width,
                          WebGLsizei height,
                          WebGLint border);
    void glCopyTexSubImage2D(WebGLenum target,
                             WebGLint level,
                             WebGLint xoffset,
                             WebGLint yoffset,
                             WebGLint x,
                             WebGLint y,
                             WebGLsizei width,
                             WebGLsizei height);
    void glDeleteTextures(WebGLsizei n, const WebGLuint *textures);
    void glGenTextures(WebGLsizei n, WebGLuint *textures);
    void glGetTexParameter(WebGLenum target, WebGLenum pname, WebGLint *params);
    WebGLboolean glIsTexture(WebGLuint texture);
    void glTexImage2D(WebGLenum target,
                      WebGLint level,
                      WebGLenum internalformat,
                      WebGLsizei width,
                      WebGLsizei height,
                      WebGLsizei border,
                      WebGLenum format,
                      WebGLenum type,
                      const WebGLvoid *data);
    void glTexImage3D(WebGLenum target,
                      WebGLint level,
                      WebGLenum internalformat,
                      WebGLsizei width,
                      WebGLsizei height,
                      WebGLsizei depth,
                      WebGLsizei border,
                      WebGLenum format,
                      WebGLenum type,
                      const WebGLvoid *data);
    void glTexParameterf(WebGLenum target, WebGLenum pname, WebGLfloat param);
    void glTexParameteri(WebGLenum target, WebGLenum pname, WebGLint param);
    void glTexParameterfv(WebGLenum target, WebGLenum pname, const WebGLfloat *params);
    void glTexParameteriv(WebGLenum target, WebGLenum pname, const WebGLint *params);
    void glTexStorage2D(WebGLenum target,
                        WebGLint levels,
                        WebGLenum internalformat,
                        WebGLsizei width,
                        WebGLsizei height);
    void glTexStorage3D(WebGLenum target,
                        WebGLint levels,
                        WebGLenum internalformat,
                        WebGLsizei width,
                        WebGLsizei height,
                        WebGLsizei depth);
    void glTexSubImage2D(WebGLenum target,
                         WebGLint level,
                         WebGLint xoffset,
                         WebGLint yoffset,
                         WebGLsizei width,
                         WebGLsizei height,
                         WebGLenum format,
                         WebGLenum type,
                         const WebGLvoid *data);
    void glTexSubImage3D(WebGLenum target,
                         WebGLint level,
                         WebGLint xoffset,
                         WebGLint yoffset,
                         WebGLint zoffset,
                         WebGLsizei width,
                         WebGLsizei height,
                         WebGLsizei depth,
                         WebGLenum format,
                         WebGLenum type,
                         const WebGLvoid *data);

    // Rendering
    void glClear(WebGLbitfield mask);
    void glClearBufferiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLint *value);
    void glClearBufferuiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLuint *value);
    void glClearBufferfv(WebGLenum buffer, WebGLint drawbuffer, const WebGLfloat *value);
    void glClearBufferfi(WebGLenum buffer, WebGLint drawbuffer, WebGLfloat depth, WebGLint stencil);
    void glClearColor(WebGLfloat red, WebGLfloat green, WebGLfloat blue, WebGLfloat alpha);
    void glClearDepthf(WebGLfloat depth);
    void glClearStencil(WebGLint s);
    void glFinish();
    void glFlush();
    void glReadBuffer(WebGLenum buffer);
    void glReadPixels(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height, WebGLenum format, WebGLenum type, WebGLvoid *pixels);

    // Frame Buffers
    void glBindFramebuffer(WebGLenum target, WebGLuint framebuffer);
    void glBindRenderbuffer(WebGLenum target, WebGLuint renderbuffer);
    void glBlitFramebuffer(WebGLint srcX0,
                           WebGLint srcY0,
                           WebGLint srcX1,
                           WebGLint srcY1,
                           WebGLint dstX0,
                           WebGLint dstY0,
                           WebGLint dstX1,
                           WebGLint dstY1,
                           WebGLbitfield mask,
                           WebGLenum filter);
    void glCheckFramebufferStatus(WebGLenum target);
    void glDeleteFramebuffers(WebGLsizei n, const WebGLuint *framebuffers);
    void glDeleteRenderbuffers(WebGLsizei n, const WebGLuint *renderbuffers);
    void glDrawBuffers(WebGLsizei n, const WebGLenum *buffers);
    void glFramebufferRenderbuffer(WebGLenum target, WebGLenum attachment, WebGLenum renderbuffertarget, WebGLuint renderbuffer);
    void glFramebufferTexture2D(WebGLenum target, WebGLenum attachment, WebGLenum textarget, WebGLuint texture, WebGLint level);
    void glFramebufferTextureLayer(WebGLenum target, WebGLenum attachment, WebGLuint texture, WebGLint level, WebGLint layer);
    void glGenFramebuffers(WebGLsizei n, WebGLuint *framebuffers);
    void glGenRenderbuffers(WebGLsizei n, WebGLuint *renderbuffers);
    void glGenerateMipmap(WebGLenum target);
    void glGetFramebufferAttachmentParameteriv(WebGLenum target, WebGLenum attachment, WebGLenum pname, WebGLint *params);
    void glGetRenderbufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glInvalidateFramebuffer(WebGLenum target, WebGLsizei n, const WebGLenum *attachments);
    void glInvalidateSubFramebuffer(WebGLenum target,
                                    WebGLsizei n,
                                    const WebGLenum *attachments,
                                    WebGLint x,
                                    WebGLint y,
                                    WebGLsizei width,
                                    WebGLsizei height);
    void glIsFramebuffer(WebGLuint framebuffer);
    void glIsRenderbuffer(WebGLuint renderbuffer);
    void glRenderbufferStorage(WebGLenum target, WebGLenum internalformat, WebGLsizei width, WebGLsizei height);
    void glRenderbufferStorageMultisample(WebGLenum target,
                                          WebGLsizei samples,
                                          WebGLenum internalformat,
                                          WebGLsizei width,
                                          WebGLsizei height);

    // Shaders
    void glAttachShader(WebGLuint program, WebGLuint shader);
    void glBindAttribLocation(WebGLuint program, WebGLuint index, const WebGLchar *name);
    void glCompileShader(WebGLuint shader);
    void glCreateProgram();
    void glCreateShader(WebGLenum type);
    void glDeleteProgram(WebGLuint program);
    void glDeleteShader(WebGLuint shader);
    void glDetachShader(WebGLuint program, WebGLuint shader);
    void glGetActiveAttrib(WebGLuint program,
                           WebGLuint index,
                           WebGLsizei maxLength,
                           WebGLsizei *length,
                           WebGLint *size,
                           WebGLenum *type,
                           WebGLchar *name);
    void glGetActiveUniform(WebGLuint program,
                            WebGLuint index,
                            WebGLsizei maxLength,
                            WebGLsizei *length,
                            WebGLint *size,
                            WebGLenum *type,
                            WebGLchar *name);
    void glGetActiveUniformBlockName(WebGLuint program,
                                     WebGLuint index,
                                     WebGLsizei maxLength,
                                     WebGLsizei *length,
                                     WebGLchar *name);
    void glGetActiveUniformBlockiv(WebGLuint program,
                                   WebGLuint index,
                                   WebGLenum pname,
                                   WebGLint *params);
    void glGetActiveUniformsiv(WebGLuint program,
                               WebGLsizei count,
                               const WebGLuint *uniforms,
                               WebGLenum pname,
                               WebGLint *params);
    void glGetAttachedShaders(WebGLuint program, WebGLsizei maxCount, WebGLsizei *count, WebGLuint *shaders);
    void glGetAttribLocation(WebGLuint program, const WebGLchar *name);
    void glGetFragDataLocation(WebGLuint program, const WebGLchar *name);
    void glGetProgramBinary(WebGLuint program,
                            WebGLsizei maxLength,
                            WebGLsizei *length,
                            WebGLenum binaryFormat,
                            WebGLsizei *binaryLength,
                            WebGLbyte *binary);
    void glGetProgramInfoLog(WebGLuint program, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog);
    void glGetProgramiv(WebGLuint program, WebGLenum pname, WebGLint *params);
    void glGetShaderInfoLog(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog);
    void glGetShaderPrecisionFormat(WebGLenum shadertype, WebGLenum precisiontype, WebGLint *range, WebGLint *precision);
    void glGetShaderSource(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *source);
    void glGetShaderiv(WebGLuint shader, WebGLenum pname, WebGLint *params);
    void glGetUniformfv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLfloat *params);
    void glGetUniformiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLint *params);
    void glGetUniformuiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLuint *params);
    void glGetUniformBlockIndex(WebGLuint program, const WebGLchar *name);
    void glGetUniformIndices(WebGLuint program, WebGLsizei count, const WebGLchar **names, WebGLuint *indices);
    void glGetUniformLocation(WebGLuint program, const WebGLchar *name);
    void glIsProgram(WebGLuint program);
    void glIsShader(WebGLuint shader);
    void glLinkProgram(WebGLuint program);
    void glProgramBinary(WebGLuint program, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength);
    void glProgramParameteri(WebGLuint program, WebGLenum pname, WebGLint param);
    void glReleaseShaderCompiler();
    void glShaderBinary(WebGLuint shader, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength);
    void glShaderSource(WebGLuint shader, WebGLsizei count, const WebGLchar **string, const WebGLint *length);
    void glUniform1f(WebGLuint location, WebGLfloat v0);
    void glUniform2f(WebGLuint location, WebGLfloat v0, WebGLfloat v1);
    void glUniform3f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2);
    void glUniform4f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2, WebGLfloat v3);
    void glUniform1i(WebGLuint location, WebGLint v0);
    void glUniform2i(WebGLuint location, WebGLint v0, WebGLint v1);
    void glUniform3i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2);
    void glUniform4i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2, WebGLint v3);
    void glUniform1ui(WebGLuint location, WebGLuint v0);
    void glUniform2ui(WebGLuint location, WebGLuint v0, WebGLuint v1);
    void glUniform3ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2);
    void glUniform4ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2, WebGLuint v3);
    void glUniform1fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform2fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform3fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform4fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value);
    void glUniform1iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform2iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform3iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform4iv(WebGLuint location, WebGLsizei count, const WebGLint *value);
    void glUniform1uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniform2uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniform3uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniform4uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value);
    void glUniformMatrix2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix2x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix3x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix2x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix4x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix3x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformMatrix4x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value);
    void glUniformBlockBinding(WebGLuint program, WebGLuint uniformBlockIndex, WebGLuint bindingPoint);
    void glUseProgram(WebGLuint program);
    void glValidateProgram(WebGLuint program);

    // Buffer Objects
    void glBindBuffer(WebGLenum target, WebGLuint buffer);
    void glBindBufferBase(WebGLenum target, WebGLuint bindingPoint, WebGLuint buffer);
    void glBindBufferRange(WebGLenum target, WebGLuint bindingPoint, WebGLuint buffer, WebGLintptr offset, WebGLsizeiptr size);
    void glBufferData(WebGLenum target, WebGLsizeiptr size, const WebGLvoid *data, WebGLenum usage);
    void glBufferSubData(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size, const WebGLvoid *data);
    void glCopyBufferSubData(WebGLenum readTarget,
                             WebGLenum writeTarget,
                             WebGLintptr readOffset,
                             WebGLintptr writeOffset,
                             WebGLsizeiptr size);
    void glDeleteBuffers(WebGLsizei count, const WebGLuint *buffers);
    void glDisableVertexAttribArray(WebGLuint index);
    void glDrawArrays(WebGLenum mode, WebGLint first, WebGLsizei count);
    void glDrawArraysInstanced(WebGLenum mode, WebGLint first, WebGLsizei count, WebGLsizei instanceCount);
    void glDrawElements(WebGLenum mode, WebGLsizei count, WebGLenum type, const WebGLvoid *indices);
    void glDrawElementsInstanced(WebGLenum mode, WebGLsizei count, WebGLenum type, const WebGLvoid *indices, WebGLsizei instanceCount);
    void glDrawRangeElements(WebGLenum mode, WebGLuint start, WebGLuint end, WebGLsizei count, WebGLenum type, const WebGLvoid *indices);
    void glEnableVertexAttribArray(WebGLuint index);
    void glFlushMappedBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size);
    void glGenBuffers(WebGLsizei n, WebGLuint *buffers);
    void glGetBufferParameter(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glGetBufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glGetBufferPointerv(WebGLenum target, WebGLenum pname, WebGLvoid **params);
    void glGetVertexAttrib(WebGLuint index, WebGLenum pname, WebGLint *params);
    void glGetVertexAttribPointerv(WebGLuint index, WebGLenum pname, WebGLvoid **pointer);
    void glIsBuffer(WebGLuint buffer);
    void glMapBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr length, WebGLbitfield access);
    void glUnmapBuffer(WebGLenum target);
    void glVertexAttrib(WebGLuint index, WebGLfloat x);
    void glVertexAttrib1f(WebGLuint index, WebGLfloat x);
    void glVertexAttrib2f(WebGLuint index, WebGLfloat x, WebGLfloat y);
    void glVertexAttrib3f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z);
    void glVertexAttrib4f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z, WebGLfloat w);
    void glVertexAttrib1fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttrib2fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttrib3fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttrib4fv(WebGLuint index, const WebGLfloat *v);
    void glVertexAttribDivisor(WebGLuint index, WebGLuint divisor);
    void glVertexAttribPointer(WebGLuint index,
                               WebGLint size,
                               WebGLenum type,
                               WebGLboolean normalized,
                               WebGLsizei stride,
                               WebGLintptr offset);
    void glVertexAttribIPointer(WebGLuint index,
                                WebGLint size,
                                WebGLenum type,
                                WebGLsizei stride,
                                WebGLintptr offset);

    // State Management
    void glBlendColor(WebGLfloat red, WebGLfloat green, WebGLfloat blue, WebGLfloat alpha);
    void glBlendEquation(WebGLenum mode);
    void glBlendEquationSeparate(WebGLenum modeRGB, WebGLenum modeAlpha);
    void glBlendFunc(WebGLenum sfactor, WebGLenum dfactor);
    void glBlendFuncSeparate(WebGLenum srcRGB, WebGLenum dstRGB, WebGLenum srcAlpha, WebGLenum dstAlpha);
    void glColorMask(WebGLboolean red, WebGLboolean green, WebGLboolean blue, WebGLboolean alpha);
    void glCullFace(WebGLenum mode);
    void glDepthFunc(WebGLenum func);
    void glDepthMask(WebGLboolean flag);
    void glDepthRangef(WebGLfloat near, WebGLfloat far);
    void glDisable(WebGLenum cap);
    void glEnable(WebGLenum cap);
    void glFrontFace(WebGLenum mode);
    void glGet(WebGLenum pname, WebGLint *params);
    WebGLenum glGetError();
    void glHint(WebGLenum target, WebGLenum mode);
    WebGLboolean glIsEnabled(WebGLenum cap);
    void glLineWidth(WebGLfloat width);
    void glPixelStorei(WebGLenum pname, WebGLint param);
    void glPolygonOffset(WebGLfloat factor, WebGLfloat units);
    void glSampleCoverage(WebGLfloat value, WebGLboolean invert);
    void glScissor(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height);
    void glStencilFunc(WebGLenum func, WebGLint ref, WebGLuint mask);
    void glStencilFuncSeparate(WebGLenum face, WebGLenum func, WebGLint ref, WebGLuint mask);
    void glStencilMask(WebGLuint mask);
    void glStencilMaskSeparate(WebGLenum face, WebGLuint mask);
    void glStencilOp(WebGLenum fail, WebGLenum zfail, WebGLenum zpass);
    void glStencilOpSeparate(WebGLenum face, WebGLenum fail, WebGLenum zfail, WebGLenum zpass);
    void glViewport(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height);

    // Transform Feedback
    void glBeginTransformFeedback(WebGLenum primitiveMode);
    void glBindTransformFeedback(WebGLenum target, WebGLuint transformFeedback);
    void glDeleteTransformFeedbacks(WebGLsizei n, const WebGLuint *transformFeedbacks);
    void glEndTransformFeedback();
    void glGenTransformFeedbacks(WebGLsizei n, WebGLuint *transformFeedbacks);
    void glGetTransformFeedbackVarying(WebGLuint program,
                                       WebGLuint index,
                                       WebGLsizei bufSize,
                                       WebGLsizei *length,
                                       WebGLsizei *size,
                                       WebGLenum *type,
                                       WebGLchar *name);
    void glIsTransformFeedback(WebGLuint transformFeedback);
    void glPauseTransformFeedback();
    void glResumeTransformFeedback();
    void glTransformFeedbackVaryings(WebGLuint program,
                                     WebGLsizei count,
                                     const WebGLchar **varyings,
                                     WebGLenum bufferMode);

    // Utility
    void glGetInternalformativ(WebGLenum target, WebGLenum internalformat, WebGLsizei propCount, WebGLenum *props, WebGLint *params);
    void glGetString(WebGLenum pname, WebGLchar *params);
    void glGetStringi(WebGLenum pname, WebGLuint index);

    // Queries
    void glBeginQuery(WebGLenum target, WebGLuint id);
    void glDeleteQueries(WebGLsizei n, const WebGLuint *ids);
    void glEndQuery(WebGLenum target);
    void glGenQueries(WebGLsizei n, WebGLuint *ids);
    void glGetQueryObjectuiv(WebGLuint id, WebGLenum pname, WebGLuint *params);
    void glGetQueryiv(WebGLenum target, WebGLenum pname, WebGLint *params);
    void glIsQuery(WebGLuint id);

    // Syncing
    void glClientWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout);
    void glDeleteSync(WebGLsync sync);
    WebGLsync glFenceSync(WebGLenum condition, WebGLbitfield flags);
    void glGetSynciv(WebGLsync sync, WebGLenum pname, WebGLsizei bufSize, WebGLsizei *length, WebGLint *values);
    WebGLboolean glIsSync(WebGLsync sync);
    void glWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout);

    // Vertex Array Objects
    void glBindVertexArray(WebGLuint array);
    void glDeleteVertexArrays(WebGLsizei n, const WebGLuint *arrays);
    void glGenVertexArrays(WebGLsizei n, WebGLuint *arrays);
    void glIsVertexArray(WebGLuint array);

    // Samplers
    void glBindSampler(WebGLuint unit, WebGLuint sampler);
    void glDeleteSamplers(WebGLsizei n, const WebGLuint *samplers);
    void glGenSamplers(WebGLsizei n, WebGLuint *samplers);
    void glGetSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint *params);
    void glIsSampler(WebGLuint sampler);
    void glSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint param);

    WebGLenum last_error_ = WEBGL_NO_ERROR;
    Capabilities caps_;
    WebGLenum active_texture_;
    std::unordered_map<TextureTarget, TextureBinding, TextureTarget::HashKey> texture_bindings_;

    WebGLfloat blend_color_[4];
    WebGLenum blend_equation_rgb_;
    WebGLenum blend_equation_alpha_;
    WebGLenum blend_sfactor_rgb_;
    WebGLenum blend_dfactor_rgb_;
    WebGLenum blend_sfactor_alpha_;
    WebGLenum blend_dfactor_alpha_;
    WebGLboolean color_mask_[4];

    WebGLenum cull_face_;
    WebGLenum front_face_;
    WebGLenum depth_func_;
    WebGLboolean depth_mask_;
    WebGLfloat depth_range_[2];

    WebGLenum stencil_func_;
    WebGLint stencil_ref_;
    WebGLuint stencil_mask_;

    WebGLfloat line_width_;
    WebGLfloat polygon_offset_factor_;
    WebGLfloat polygon_offset_units_;
    WebGLfloat sample_coverage_value_;
    WebGLboolean sample_coverage_invert_;
    WebGLint scissor_box_[4];
    WebGLint viewport_[4];
  };
}

namespace std
{
  template <>
  struct hash<renderer::TrContextWebGL::TextureTarget>
  {
    size_t operator()(const renderer::TrContextWebGL::TextureTarget &t) const noexcept
    {
      return std::hash<WebGLenum>{}(t.value());
    }
  };
}
