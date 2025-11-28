#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>
#include "utility.hpp"

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBindAttribLocation(WebGLuint program, WebGLuint index, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCompileShader(WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  WebGLuint TrContextWebGL::glCreateProgram()
  {
    WebGLuint id = programs_.size();
    auto program = AcquireRef(new details::Program(id));
    programs_.push_back(program);
    return program->id;
  }

  WebGLuint TrContextWebGL::glCreateShader(WebGLenum type)
  {
    WebGLuint id = shader_modules_.size();
    auto shader = AcquireRef(new details::ShaderModule(id, type));
    shader_modules_.push_back(shader);
    return shader->id;
  }

  void TrContextWebGL::glDeleteProgram(WebGLuint program)
  {
    programs_.remove(program);
  }

  void TrContextWebGL::glDeleteShader(WebGLuint shader)
  {
    shader_modules_.remove(shader);
  }

  void TrContextWebGL::glAttachShader(WebGLuint program_id, WebGLuint shader_id)
  {
    auto program = programs_.get(program_id);
    auto shader_module = shader_modules_.get(shader_id);
    if (program == nullptr || shader_module == nullptr) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    if (shader_module->type == WEBGL_VERTEX_SHADER)
      program->vertexShader = shader_module;
    else if (shader_module->type == WEBGL_FRAGMENT_SHADER)
      program->fragmentShader = shader_module;
    else [[unlikely]]
      last_error_ = WEBGL_INVALID_OPERATION;
  }

  void TrContextWebGL::glDetachShader(WebGLuint program_id, WebGLuint shader_id)
  {
    auto program = programs_.get(program_id);
    auto shader_module = shader_modules_.get(shader_id);
    if (program == nullptr || shader_module == nullptr) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    if (shader_module->type == WEBGL_VERTEX_SHADER)
      program->vertexShader = nullptr;
    else if (shader_module->type == WEBGL_FRAGMENT_SHADER)
      program->fragmentShader = nullptr;
    else [[unlikely]]
      last_error_ = WEBGL_INVALID_OPERATION;
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
}
