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
    void initialize(std::shared_ptr<xr::Device> xrDevice)
    {
      auto mainController = xrDevice->getMainControllerInputSource();
      if (mainController != nullptr)
      {
        mainController->enabled = true;
        mainController->setName("Main");
        mainController->setTargetRayBaseMatrix(mainControllerTargetRay);
      }
    }
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
                              windowCtx->aspect / 2, // aspect ratio for each eye
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
    void updateMainInputSourceTargetRay(const glm::vec3& origin, const glm::vec3& direction)
    {
      // Normalize the direction
      glm::vec3 dir = glm::normalize(direction);

      // Constuct the base coordinate system
      glm::vec3 forward = -dir;
      glm::vec3 up = glm::vec3(0, 1, 0);
      glm::vec3 right = glm::normalize(glm::cross(forward, up));

      // Handle the case when the forward and up are parallel
      if (glm::length(right) < 0.001f)
      {
        up = glm::vec3(0, 0, 1);
        right = glm::normalize(glm::cross(forward, up));
      }

      // Re-calculate the up vector
      up = glm::normalize(glm::cross(right, forward));

      // Construct the rotation matrix
      glm::mat4 rotation = glm::mat4(1.0f);
      rotation[0] = glm::vec4(right, 0);
      rotation[1] = glm::vec4(up, 0);
      rotation[2] = glm::vec4(forward, 0);

      glm::mat4 translation = glm::translate(glm::mat4(1.0f), origin);
      mainControllerTargetRay = translation * rotation;
    }
    void writeInputSources()
    {
      assert(xrDevice != nullptr && "XR device is not initialized.");
      auto mainController = xrDevice->getMainControllerInputSource();
      if (mainController != nullptr)
        mainController->setTargetRayBaseMatrix(mainControllerTargetRay);
    }

  private:
    std::shared_ptr<xr::Device> xrDevice = nullptr;
    glm::vec3 viewerPosition;
    glm::quat viewerOrientation;
    glm::vec3 eyePosition[2];
    glm::quat eyeOrientation[2];

  private: // projection
    WindowContext *windowCtx;
    float near = 0.01f;
    float far = 100.0f;
    float fov = 60.0f;

  private: // Input sources
    glm::mat4 mainControllerTargetRay = glm::mat4(1.0f);
  };
}
