#pragma once

#include <map>
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  enum class WebGLBufferBindingTarget
  {
    kArrayBuffer = WEBGL_ARRAY_BUFFER,
    kElementArrayBuffer = WEBGL_ELEMENT_ARRAY_BUFFER,

    // WebGL2
    kCopyReadBuffer = WEBGL2_COPY_READ_BUFFER,
    kCopyWriteBuffer = WEBGL2_COPY_WRITE_BUFFER,
    kTransformFeedbackBuffer = WEBGL2_TRANSFORM_FEEDBACK_BUFFER,
    kUniformBuffer = WEBGL2_UNIFORM_BUFFER,
    kPixelPackBuffer = WEBGL2_PIXEL_PACK_BUFFER,
    kPixelUnpackBuffer = WEBGL2_PIXEL_UNPACK_BUFFER,
  };

  enum class WebGLBufferUsage
  {
    kStaticDraw = WEBGL_STATIC_DRAW,
    kDynamicDraw = WEBGL_DYNAMIC_DRAW,
    kStreamDraw = WEBGL_STREAM_DRAW,

    // WebGL2
    kStaticRead = WEBGL2_STATIC_READ,
    kDynamicRead = WEBGL2_DYNAMIC_READ,
    kStreamRead = WEBGL2_STREAM_READ,
    kStaticCopy = WEBGL2_STATIC_COPY,
    kDynamicCopy = WEBGL2_DYNAMIC_COPY,
    kStreamCopy = WEBGL2_STREAM_COPY,
  };

  class WebGLBuffer : public WebGLObject
  {
  public:
    WebGLBuffer()
        : WebGLObject(WebGLObjectType::Buffer)
    {
    }
  };
}
