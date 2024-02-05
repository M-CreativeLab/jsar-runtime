#include "binding.hpp"
#include "rendering_context.hpp"
#include "program.hpp"

Napi::Object InitWebglModule(Napi::Env env, Napi::Object exports)
{
  webgl::WebGLRenderingContext::Init(env, exports);
  // webgl::WebGLProgram::Init(env, exports);
  // webgl::WebGLShader::Init(env, exports);
  return exports;
}
