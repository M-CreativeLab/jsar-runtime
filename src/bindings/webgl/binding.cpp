#include "binding.hpp"
#include "rendering_context.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "uniform_location.hpp"

Napi::Object InitWebglModule(Napi::Env env, Napi::Object exports)
{
  webgl::WebGLRenderingContext::Init(env, exports);
  webgl::WebGL2RenderingContext::Init(env, exports);
  webgl::WebGLProgram::Init(env);
  webgl::WebGLTexture::Init(env);
  webgl::WebGLUniformLocation::Init(env);
  // webgl::WebGLShader::Init(env, exports);
  return exports;
}
