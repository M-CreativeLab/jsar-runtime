#include <client/graphics/webgl_shader.hpp>
#include "./shader_base.hpp"
#include "./material_base.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  string ShaderPreprocessor::PreprocessSource(string source, const vector<string> &defines, WebGLShaderType shaderType)
  {
    // The flag to check if the shader has multiview
    bool shouldMultiview = false;

    // Add the defines
    for (const auto &define : defines)
    {
      source = "#define " + define + "\n" + source;
      if (define == Material::kRequireMultiviewDefine)
        shouldMultiview = true;
    }

    if (shaderType == WebGLShaderType::kVertex)
    {
      if (shouldMultiview)
      {
        // Add the multiview extension
        std::vector<std::string> prependLines = {
          "#extension GL_OVR_multiview2 : require",
          "layout(num_views = 2) in;",
          "#define VIEW_ID gl_ViewID_OVR"};
        source = ConcatSource(source, prependLines);
      }
    }

    // Prepend WebGL 2.0 version: #version 300 es
    source = "#version 300 es\n" + source;

    // Return the preprocessed source
    return source;
  }

  string ShaderPreprocessor::ConcatSource(const string &source, const vector<string> &lines)
  {
    string r;
    for (const auto &line : lines)
      r += line + "\n";
    return r + source;
  }
}
