#include <chrono>
#include <glslang/Public/ResourceLimits.h>
#include "./webgl_shader.hpp"

using namespace std;

namespace endor
{
  namespace client_graphics
  {
    inline EShLanguage EShLang(WebGLShaderType type)
    {
      switch (type)
      {
      case WebGLShaderType::kVertex:
        return EShLangVertex;
      case WebGLShaderType::kFragment:
        return EShLangFragment;
      default:
        throw runtime_error("Unknown WebGLShaderType");
      }
    }

    WebGLShader::WebGLShader(WebGLShaderType type)
        : WebGLObject(WebGLObjectType::Shader)
        , type(type)
        , glslang_shader_(make_unique<glslang::TShader>(EShLang(type)))
    {
      assert(glslang_shader_ != nullptr && "Failed to create glslang shader");
      glslang_shader_->setAutoMapBindings(true);
      glslang_shader_->setAutoMapLocations(true);
    }

    void WebGLShader::setSource(const string &source)
    {
      source_ = source;
      source_cstr_ = source_.c_str();
      glslang_shader_->setStrings(&source_cstr_, 1);

      // Configure the environment for WebGL (OpenGL ES)
      glslang_shader_->setEnvInput(glslang::EShSourceGlsl, EShLang(type), glslang::EShClientOpenGL, s_DefaultVersion);
      glslang_shader_->setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
      glslang_shader_->setEnvTarget(glslang::EShTargetNone, glslang::EShTargetSpv_1_0);
    }

    const string &WebGLShader::source() const
    {
      return source_;
    }

    void WebGLShader::compile()
    {
      EShMessages messages = EShMsgDefault;
      messages = (EShMessages)(messages | EShMsgRelaxedErrors);

      if (!glslang_shader_->parse(GetDefaultResources(),
                                  s_DefaultVersion,
                                  EProfile::EEsProfile,
                                  false,
                                  false,
                                  messages))
      {
        auto debug_log = glslang_shader_->getInfoDebugLog();
        cerr << "Shader Debug Log:" << endl
             << debug_log << endl;
        throw runtime_error("Shader compilation failed: " + string(glslang_shader_->getInfoLog()));
      }
    }

    const char *WebGLShader::getInfoLog()
    {
      return glslang_shader_->getInfoLog();
    }

    bool WebGLShader::hasDeleteStatus() const
    {
      return delete_status_.has_value();
    }

    bool WebGLShader::hasCompileStatus() const
    {
      return compile_status_.has_value();
    }

    bool WebGLShader::getDeleteStatus()
    {
      return delete_status_.value_or(false);
    }

    bool WebGLShader::getCompileStatus()
    {
      return compile_status_.value_or(false);
    }

    void WebGLShader::setShaderParameters(bool deleteStatus, bool compileStatus)
    {
      delete_status_ = deleteStatus;
      compile_status_ = compileStatus;
    }
  }
}
