#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>
#include "command_buffers/gpu/gpu_buffer.hpp"

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glBindBuffer(WebGLenum target, WebGLuint id)
  {
    auto buffer = buffers_.get(id);
    if (!buffer) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    auto buffer_target = details::BufferTarget(target);
    buffer_bindings_[buffer_target] = buffer;
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
    for (WebGLsizei i = 0; i < count; i++)
    {
      for (auto it = buffers_.begin(); it != buffers_.end(); ++it)
      {
        Ref<details::Buffer> buffer = *it;
        if (buffer->id == buffers[i])
        {
          buffers_.erase(it);
          break;
        }
      }
    }
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
    glGenTypedObjects(buffers_, n, buffers);

    {
      GPUBufferDescriptor desc = {
        .label = "TrContextWebGL::glGenBuffers",
        .size = 0,
        .usage = GPUBufferUsage::kMapRead |
                 GPUBufferUsage::kMapWrite |
                 GPUBufferUsage::kCopySrc |
                 GPUBufferUsage::kCopyDst |
                 GPUBufferUsage::kVertex |
                 GPUBufferUsage::kIndex |
                 GPUBufferUsage::kUniform,
      };
      getRenderResource()->createBuffer(&desc);
    }
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

  WebGLboolean TrContextWebGL::glIsBuffer(WebGLuint buffer)
  {
    return buffers_.has(buffer);
  }

  void TrContextWebGL::glMapBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr length, WebGLbitfield access)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUnmapBuffer(WebGLenum target)
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
}
