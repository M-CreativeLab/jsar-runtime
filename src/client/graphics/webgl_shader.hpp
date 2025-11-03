#pragma once

#include <map>
#include <memory>
#include <string>
#include <glslang/Public/ShaderLang.h>
#include <common/command_buffers/details/program.hpp>
#include "./webgl_object.hpp"

namespace endor
{
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
      WebGLShader(WebGLShaderType type);

      glslang::TShader *glslangShader()
      {
        return glslang_shader_.get();
      }

      void setSource(const std::string &source);
      const std::string &source() const;
      void compile();
      const char *getInfoLog();

      bool hasDeleteStatus() const;
      bool hasCompileStatus() const;
      bool getDeleteStatus();
      bool getCompileStatus();

    private:
      void setShaderParameters(bool deleteStatus, bool compileStatus);

    public:
      WebGLShaderType type;

    private:
      std::string source_;
      const char *source_cstr_ = nullptr;

      std::unique_ptr<glslang::TShader> glslang_shader_;
      std::optional<bool> delete_status_;
      std::optional<bool> compile_status_;

    private:
      static constexpr int s_DefaultVersion = 300;
    };
  }
} // namespace endor
