#pragma once

#include <map>
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  enum class WebGLShaderType
  {
    kVertex = WEBGL_VERTEX_SHADER,
    kFragment = WEBGL_FRAGMENT_SHADER,
  };

  class WebGLShader : public WebGLObject
  {
  public:
    WebGLShader(WebGLShaderType type)
        : WebGLObject(WebGLObjectType::Shader)
        , type(type)
    {
    }

  public:
    WebGLShaderType type;
  };
}
