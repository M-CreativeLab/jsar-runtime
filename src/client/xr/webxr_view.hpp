#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "./common.hpp"
#include "./webxr_viewport.hpp"

namespace client_xr
{
  class XRView
  {
  public:
    XRView(uint32_t index);

  public:
    XREye eye();
    glm::mat4 transformMatrix();

  public:
    void requestViewportScale(float scale);

  private:
    uint32_t index_;
    uint32_t sessionId_;
    glm::mat4 transformMatrix_;
    glm::mat4 projectionMatrix_;
    XRViewport viewport_;
    float viewportScale_ = 1.0f;
  };
}
