#pragma once

#include <map>
#include <string>

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
    friend class WebGLContext;

  public:
    WebGLShader(WebGLShaderType type)
        : WebGLObject(WebGLObjectType::Shader)
        , type(type)
    {
    }

    bool hasDeleteStatus() const
    {
      return delete_status_.has_value();
    }
    bool hasCompileStatus() const
    {
      return compile_status_.has_value();
    }
    bool getDeleteStatus()
    {
      return delete_status_.value_or(false);
    }
    bool getCompileStatus()
    {
      return compile_status_.value_or(false);
    }

  private:
    void setShaderParameters(bool deleteStatus, bool compileStatus)
    {
      delete_status_ = deleteStatus;
      compile_status_ = compileStatus;
    }

  public:
    WebGLShaderType type;
    std::string source;

  private:
    std::optional<bool> delete_status_;
    std::optional<bool> compile_status_;
  };
}
