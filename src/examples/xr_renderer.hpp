#pragma once

#include <glm/glm.hpp>
#include "./window_ctx.hpp"

namespace jsar::example
{
#define XR_EYE_SPAN 0.08f /** The length between 2 eyes */

  class XRStereoscopicRenderer
  {
  public:
    XRStereoscopicRenderer(WindowContext *windowCtx) : windowCtx(windowCtx)
    {
      float eyeOffset = XR_EYE_SPAN / 2;
      viewerPosition = glm::vec3(0.0f, 0.0f, 0.7f);
      {
        glm::vec3 viewerForward(0.0f, 0.0f, -1.0f);
        viewerOrientation = glm::quatLookAt(glm::normalize(viewerForward), glm::vec3(0, 1, 0));
      }
      eyePosition[0] = glm::vec3(viewerPosition.x - eyeOffset, viewerPosition.y, viewerPosition.z);
      eyePosition[1] = glm::vec3(viewerPosition.x + eyeOffset, viewerPosition.y, viewerPosition.z);
      {
        glm::vec3 eyeForward(0.01f, 0.0f, -1.0f);
        eyeOrientation[0] = glm::quatLookAt(glm::normalize(eyeForward), glm::vec3(0, 1, 0));
      }
      {
        glm::vec3 eyeForward(-0.01f, 0.0f, -1.0f);
        eyeOrientation[1] = glm::quatLookAt(glm::normalize(eyeForward), glm::vec3(0, 1, 0));
      }
    }
    ~XRStereoscopicRenderer()
    {
    }

  public:
    glm::mat4 getViewerBaseMatrix()
    {
      return glm::translate(glm::mat4(1.0f), viewerPosition) * glm::mat4_cast(viewerOrientation);
    }
    glm::mat4 getViewMatrixForEye(int eyeIndex)
    {
      assert(eyeIndex < 2);
      auto viewBaseMatrix = glm::translate(glm::mat4(1.0f), eyePosition[eyeIndex]) * glm::mat4_cast(eyeOrientation[eyeIndex]);
      return glm::inverse(viewBaseMatrix);
    }
    glm::mat4 getProjectionMatrix()
    {
      return glm::perspective(glm::radians(fov),
                              windowCtx->aspect / 2,  // aspect ratio for each eye
                              near, far);
    }
    void moveViewerForward(float zOffset)
    {
      viewerPosition.z += zOffset;
      eyePosition[0].z = viewerPosition.z;
      eyePosition[1].z = viewerPosition.z;
    }
    void rotateViewerByAxisY(float angle)
    {
      auto rotation = glm::angleAxis(angle, glm::vec3(0, 1, 0));
      viewerOrientation = rotation * viewerOrientation;
      eyeOrientation[0] = rotation * eyeOrientation[0];
      eyeOrientation[1] = rotation * eyeOrientation[1];
    }

  private:
    glm::vec3 viewerPosition;
    glm::quat viewerOrientation;
    glm::vec3 eyePosition[2];
    glm::quat eyeOrientation[2];

  private: // projection
    WindowContext *windowCtx;
    float near = 0.01f;
    float far = 100.0f;
    float fov = 60.0f;
  };
}
