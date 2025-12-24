#include <iostream>
#include <renderer/gles/gpu_buffer_impl.hpp>

namespace gles
{
  using namespace commandbuffers;

  static GLenum ChooseTarget(GPUBufferUsage usage)
  {
    if ((usage & GPUBufferUsage::kVertex) == GPUBufferUsage::kVertex)
      return GL_ARRAY_BUFFER;
    if ((usage & GPUBufferUsage::kIndex) == GPUBufferUsage::kIndex)
      return GL_ELEMENT_ARRAY_BUFFER;
    return GL_ARRAY_BUFFER;
  }

  GPUBufferImpl::GPUBufferImpl(Ref<GPUDeviceBase> device,
                               const GPUBufferDescriptor &descriptor)
      : GPUBufferBase(device, descriptor)
      , target_(ChooseTarget(descriptor.usage))
      , buffer_(0)
  {
    // glGenBuffers(1, &buffer_);
    // glBindBuffer(target_, buffer_);
    // glBufferData(target_, static_cast<GLsizeiptr>(descriptor.size), nullptr, GL_STATIC_DRAW);
  }

  GPUBufferImpl::~GPUBufferImpl()
  {
    if (buffer_ != 0)
    {
      GLuint id = buffer_;
      glDeleteBuffers(1, &id);
      buffer_ = 0;
    }
  }

  gpu::MaybeError GPUBufferImpl::uploadData(uint64_t bufferOffset,
                                            const void *data,
                                            size_t size)
  {
    if (data == nullptr || size == 0)
      return {};
    // glBindBuffer(target_, buffer_);
    // glBufferSubData(target_, static_cast<GLintptr>(bufferOffset), static_cast<GLsizeiptr>(size), data);
    return {};
  }
}
