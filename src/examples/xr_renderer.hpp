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
      viewer_position_ = glm::vec3(0.0f, 0.0f, 0.7f);
      {
        glm::vec3 viewerForward(0.0f, 0.0f, -1.0f);
        viewer_orientation_ = glm::quatLookAt(glm::normalize(viewerForward), glm::vec3(0, 1, 0));
      }
      eye_position_[0] = glm::vec3(viewer_position_.x - eyeOffset, viewer_position_.y, viewer_position_.z);
      eye_position_[1] = glm::vec3(viewer_position_.x + eyeOffset, viewer_position_.y, viewer_position_.z);
      {
        glm::vec3 eyeForward(0.01f, 0.0f, -1.0f);
        eye_orientation_[0] = glm::quatLookAt(glm::normalize(eyeForward), glm::vec3(0, 1, 0));
      }
      {
        glm::vec3 eyeForward(-0.01f, 0.0f, -1.0f);
        eye_orientation_[1] = glm::quatLookAt(glm::normalize(eyeForward), glm::vec3(0, 1, 0));
      }
    }
    ~XRStereoscopicRenderer()
    {
    }

  public:
    glm::vec3 viewerPosition() { return viewer_position_; }
    glm::vec3 eyePosition(int eyeIndex)
    {
      assert(eyeIndex < 2);
      return eye_position_[eyeIndex];
    }

    void initialize(std::shared_ptr<xr::Device> xrDevice)
    {
      assert(xrDevice != nullptr && "XR device is not initialized.");
      this->xrDevice = xrDevice;

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
      return glm::translate(glm::mat4(1.0f), viewer_position_) * glm::mat4_cast(viewer_orientation_);
    }
    glm::mat4 getViewMatrixForEye(int eyeIndex)
    {
      assert(eyeIndex < 2);
      auto viewBaseMatrix =
          glm::translate(glm::mat4(1.0f), eye_position_[eyeIndex]) * glm::mat4_cast(eye_orientation_[eyeIndex]);
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
      viewer_position_.z += zOffset;
      eye_position_[0].z = viewer_position_.z;
      eye_position_[1].z = viewer_position_.z;
    }
    void rotateViewerByAxisY(float angle)
    {
      auto rotation = glm::angleAxis(angle, glm::vec3(0, 1, 0));
      viewer_orientation_ = rotation * viewer_orientation_;
      eye_orientation_[0] = rotation * eye_orientation_[0];
      eye_orientation_[1] = rotation * eye_orientation_[1];
    }
    void updateMainInputSourceTargetRay(const glm::vec3 &origin, const glm::vec3 &dir)
    {
      glm::vec3 forward = dir;
      glm::vec3 up = glm::vec3(0, 1, 0);
      glm::vec3 right = glm::cross(forward, up);
      up = glm::cross(right, forward);

      glm::mat4 baseMatrix = glm::mat4(1.0f);
      baseMatrix[0] = glm::vec4(right, 0);
      baseMatrix[1] = glm::vec4(up, 0);
      baseMatrix[2] = glm::vec4(-forward, 0);
      baseMatrix[3] = glm::vec4(origin, 1);
      mainControllerTargetRay = baseMatrix;
    }
    void updateMainInputSourcePrimaryAction(bool pressed)
    {
      assert(xrDevice != nullptr);
      auto mainController = xrDevice->getMainControllerInputSource();
      if (mainController != nullptr)
        mainController->primaryActionPressed = pressed;
    }
    void writeInputSources()
    {
      assert(xrDevice != nullptr);
      auto mainController = xrDevice->getMainControllerInputSource();
      if (mainController != nullptr)
        mainController->setTargetRayBaseMatrix(mainControllerTargetRay);
    }

  private:
    std::shared_ptr<xr::Device> xrDevice = nullptr;
    glm::vec3 viewer_position_;
    glm::quat viewer_orientation_;
    glm::vec3 eye_position_[2];
    glm::quat eye_orientation_[2];

  private: // projection
    WindowContext *windowCtx;
    float near = 0.01f;
    float far = 100.0f;
    float fov = 60.0f;

  private: // Input sources
    glm::mat4 mainControllerTargetRay = glm::mat4(1.0f);
  };
}
