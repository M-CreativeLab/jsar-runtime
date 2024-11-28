#pragma once

#include "./webgl_object.hpp"

namespace client_graphics
{
  enum class WebGLQueryTarget : uint32_t
  {
    kAnySamplesPassed = WEBGL2_ANY_SAMPLES_PASSED,
    kAnySamplesPassedConservative = WEBGL2_ANY_SAMPLES_PASSED_CONSERVATIVE,
    kTransformFeedbackPrimitivesWritten = WEBGL2_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
  };

  class WebGLQuery : public WebGLObject
  {
  public:
    WebGLQuery() : WebGLObject(WebGLObjectType::Query) {}
  };
}
