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
    auto buf = buffers_.get(buffer);
    if (!buf) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    auto buffer_target = details::BufferTarget(target);
    buffer_bindings_[buffer_target] = buf;
  }

  void TrContextWebGL::glBindBufferRange(WebGLenum target,
                                         WebGLuint bindingPoint,
                                         WebGLuint buffer,
                                         WebGLintptr offset,
                                         WebGLsizeiptr size)
  {
    auto buf = buffers_.get(buffer);
    if (!buf) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    auto buffer_target = details::BufferTarget(target);
    buffer_bindings_[buffer_target] = buf;
  }

  void TrContextWebGL::glBufferData(WebGLenum target,
                                    WebGLsizeiptr size,
                                    const WebGLvoid *data,
                                    WebGLenum usage)
  {
    auto target_key = details::BufferTarget(target);
    auto it = buffer_bindings_.find(target_key);
    if (it == buffer_bindings_.end() || !it->second)
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    auto &buffer = it->second;
    GPUBufferUsage gpu_usage = GPUBufferUsage::kCopyDst | GPUBufferUsage::kMapWrite;
    if (target == WEBGL_ARRAY_BUFFER)
      gpu_usage |= GPUBufferUsage::kVertex;
    else if (target == WEBGL_ELEMENT_ARRAY_BUFFER)
      gpu_usage |= GPUBufferUsage::kIndex;

    GPUBufferDescriptor desc = {
      .label = "WebGLBuffer",
      .usage = gpu_usage,
      .size = static_cast<uint64_t>(size),
      .mappedAtCreation = false,
    };
    auto gpu_buffer = getRenderResource()->createBuffer(&desc);
    buffer->gpu_id = gpu_buffer->id;
    buffer->size = static_cast<WebGLsizei>(size);
    buffer->usage_hint = usage;

    if (data && size > 0)
    {
      [[maybe_unused]] auto _ = gpu_buffer->uploadData(0, data, static_cast<size_t>(size));
    }
  }

  void TrContextWebGL::glBufferSubData(WebGLenum target,
                                       WebGLintptr offset,
                                       WebGLsizeiptr size,
                                       const WebGLvoid *data)
  {
    auto target_key = details::BufferTarget(target);
    auto it = buffer_bindings_.find(target_key);
    if (it == buffer_bindings_.end() || !it->second)
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    auto &buffer = it->second;
    if (buffer->gpu_id < 0)
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    auto gpu_buffer = getRenderResource()->getBuffer(buffer->gpu_id);
    [[maybe_unused]] auto _ = gpu_buffer->uploadData(static_cast<uint64_t>(offset), data, static_cast<size_t>(size));
  }

  void TrContextWebGL::glCopyBufferSubData(WebGLenum readTarget,
                                           WebGLenum writeTarget,
                                           WebGLintptr readOffset,
                                           WebGLintptr writeOffset,
                                           WebGLsizeiptr size)
  {
    auto read_it = buffer_bindings_.find(details::BufferTarget(readTarget));
    auto write_it = buffer_bindings_.find(details::BufferTarget(writeTarget));
    if (read_it == buffer_bindings_.end() || write_it == buffer_bindings_.end())
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    auto src = read_it->second;
    auto dst = write_it->second;
    if (!src || !dst || src->gpu_id < 0 || dst->gpu_id < 0)
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    // Not implemented: issue GPU copy command; fallback no-op
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
    if (!current_vertex_array_object_)
      return;
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
    if (!current_vertex_array_object_)
      return;
  }

  void TrContextWebGL::glFlushMappedBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenBuffers(WebGLsizei n, WebGLuint *buffers)
  {
    glGenTypedObjects(buffers_, n, buffers);
  }

  void TrContextWebGL::glGetBufferParameter(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    auto it = buffer_bindings_.find(details::BufferTarget(target));
    if (it == buffer_bindings_.end() || !it->second || !params)
      return;
    auto &buffer = it->second;
    if (pname == WEBGL_BUFFER_SIZE)
      *params = buffer->size;
    else if (pname == WEBGL_BUFFER_USAGE)
      *params = buffer->usage_hint;
  }

  void TrContextWebGL::glGetBufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    glGetBufferParameter(target, pname, params);
  }

  void TrContextWebGL::glGetBufferPointerv(WebGLenum target, WebGLenum pname, WebGLvoid **params)
  {
    if (params)
      *params = nullptr;
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
    auto it = buffer_bindings_.find(details::BufferTarget(target));
    if (it == buffer_bindings_.end() || !it->second)
      return;
    auto &buffer = it->second;
    if (buffer->gpu_id < 0)
      return;
    auto gpu_buffer = getRenderResource()->getBuffer(buffer->gpu_id);
    gpu_buffer->unmap();
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
    if (!current_vertex_array_object_)
      return;
    if (current_vertex_array_object_->buffer_layouts.size() <= index)
      current_vertex_array_object_->buffer_layouts.resize(index + 1);
    auto &layout = current_vertex_array_object_->buffer_layouts[index];
    layout.stepMode = divisor > 0 ? GPUVertexStepMode::kInstance : GPUVertexStepMode::kVertex;
    current_vertex_array_object_->vertex_state.bufferCount = current_vertex_array_object_->buffer_layouts.size();
    current_vertex_array_object_->vertex_state.buffers = current_vertex_array_object_->buffer_layouts.data();
  }

  void TrContextWebGL::glVertexAttribPointer(WebGLuint index,
                                             WebGLint size,
                                             WebGLenum type,
                                             WebGLboolean normalized,
                                             WebGLsizei stride,
                                             WebGLintptr offset)
  {
    if (!current_vertex_array_object_)
      return;
    auto bound_array = buffer_bindings_.find(details::BufferTarget(WEBGL_ARRAY_BUFFER));
    if (bound_array == buffer_bindings_.end() || !bound_array->second)
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    if (current_vertex_array_object_->buffer_layouts.size() <= index)
      current_vertex_array_object_->buffer_layouts.resize(index + 1);
    auto &layout = current_vertex_array_object_->buffer_layouts[index];
    layout.arrayStride = stride;
    layout.stepMode = GPUVertexStepMode::kVertex;

    GPUVertexAttribute attr{};
    attr.shaderLocation = index;
    attr.offset = static_cast<uint64_t>(offset);
    switch (type)
    {
    case WEBGL_FLOAT:
      attr.format = size == 1 ? GPUVertexFormat::kFloat32 :
                    size == 2 ? GPUVertexFormat::kFloat32x2 :
                    size == 3 ? GPUVertexFormat::kFloat32x3 : GPUVertexFormat::kFloat32x4;
      break;
    case WEBGL_UNSIGNED_BYTE:
      attr.format = normalized ? (size == 1 ? GPUVertexFormat::kUnorm8 :
                                  size == 2 ? GPUVertexFormat::kUnorm8x2 : GPUVertexFormat::kUnorm8x4)
                               : (size == 1 ? GPUVertexFormat::kUint8 :
                                  size == 2 ? GPUVertexFormat::kUint8x2 : GPUVertexFormat::kUint8x4);
      break;
    case WEBGL_BYTE:
      attr.format = normalized ? (size == 1 ? GPUVertexFormat::kSnorm8 :
                                  size == 2 ? GPUVertexFormat::kSnorm8x2 : GPUVertexFormat::kSnorm8x4)
                               : (size == 1 ? GPUVertexFormat::kSint8 :
                                  size == 2 ? GPUVertexFormat::kSint8x2 : GPUVertexFormat::kSint8x4);
      break;
    case WEBGL_UNSIGNED_SHORT:
      attr.format = normalized ? (size == 1 ? GPUVertexFormat::kUnorm16 :
                                  size == 2 ? GPUVertexFormat::kUnorm16x2 : GPUVertexFormat::kUnorm16x4)
                               : (size == 1 ? GPUVertexFormat::kUint16 :
                                  size == 2 ? GPUVertexFormat::kUint16x2 : GPUVertexFormat::kUint16x4);
      break;
    case WEBGL_SHORT:
      attr.format = normalized ? (size == 1 ? GPUVertexFormat::kSnorm16 :
                                  size == 2 ? GPUVertexFormat::kSnorm16x2 : GPUVertexFormat::kSnorm16x4)
                               : (size == 1 ? GPUVertexFormat::kSint16 :
                                  size == 2 ? GPUVertexFormat::kSint16x2 : GPUVertexFormat::kSint16x4);
      break;
    case WEBGL_INT:
      attr.format = size == 1 ? GPUVertexFormat::kSint32 :
                    size == 2 ? GPUVertexFormat::kSint32x2 :
                    size == 3 ? GPUVertexFormat::kSint32x3 : GPUVertexFormat::kSint32x4;
      break;
    case WEBGL_UNSIGNED_INT:
      attr.format = size == 1 ? GPUVertexFormat::kUint32 :
                    size == 2 ? GPUVertexFormat::kUint32x2 :
                    size == 3 ? GPUVertexFormat::kUint32x3 : GPUVertexFormat::kUint32x4;
      break;
    default:
      attr.format = GPUVertexFormat::kFloat32;
      break;
    }
    current_vertex_array_object_->attributes.push_back(attr);
    current_vertex_array_object_->vertex_state.bufferCount = current_vertex_array_object_->buffer_layouts.size();
    current_vertex_array_object_->vertex_state.buffers = current_vertex_array_object_->buffer_layouts.data();
  }

  void TrContextWebGL::glVertexAttribIPointer(WebGLuint index,
                                              WebGLint size,
                                              WebGLenum type,
                                              WebGLsizei stride,
                                              WebGLintptr offset)
  {
    if (!current_vertex_array_object_)
      return;
    if (current_vertex_array_object_->buffer_layouts.size() <= index)
      current_vertex_array_object_->buffer_layouts.resize(index + 1);
    auto &layout = current_vertex_array_object_->buffer_layouts[index];
    layout.arrayStride = stride;
    layout.stepMode = GPUVertexStepMode::kVertex;

    GPUVertexAttribute attr{};
    attr.shaderLocation = index;
    attr.offset = static_cast<uint64_t>(offset);
    switch (type)
    {
    case WEBGL_INT:
      attr.format = size == 1 ? GPUVertexFormat::kSint32 :
                    size == 2 ? GPUVertexFormat::kSint32x2 :
                    size == 3 ? GPUVertexFormat::kSint32x3 : GPUVertexFormat::kSint32x4;
      break;
    case WEBGL_UNSIGNED_INT:
      attr.format = size == 1 ? GPUVertexFormat::kUint32 :
                    size == 2 ? GPUVertexFormat::kUint32x2 :
                    size == 3 ? GPUVertexFormat::kUint32x3 : GPUVertexFormat::kUint32x4;
      break;
    default:
      attr.format = GPUVertexFormat::kSint32;
      break;
    }
    current_vertex_array_object_->attributes.push_back(attr);
    current_vertex_array_object_->vertex_state.bufferCount = current_vertex_array_object_->buffer_layouts.size();
    current_vertex_array_object_->vertex_state.buffers = current_vertex_array_object_->buffer_layouts.data();
  }
}
