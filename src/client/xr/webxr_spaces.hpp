#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <math/matrix.hpp>
#include "./common.hpp"

namespace client_xr
{
#define XRSPACE_RELATIVE_TRANSFORM(space, baseSpace) baseSpace->inverseBaseMatrix() * space->baseMatrix()

  class XRSpace
  {
  public:
    XRSpace(bool isReferenceSpace = false)
        : lastFrameId_(-1),
          baseMatrix_(1.0f),
          isReferenceSpace_(isReferenceSpace)
    {
    }
    virtual ~XRSpace() = default;

  public:
    inline bool isReferenceSpace() const { return isReferenceSpace_; }
    inline glm::mat4 &baseMatrix() { return baseMatrix_; }
    inline glm::mat4 &inverseBaseMatrix()
    {
      if (isInverseMatrixDirty_)
      {
        inverseMatrixCache_ = glm::inverse(baseMatrix_);
        isInverseMatrixDirty_ = false;
      }
      return inverseMatrixCache_;
    }
    void ensurePoseUpdated(uint32_t frameId, std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest)
    {
      if (lastFrameId_ != frameId)
      {
        lastFrameId_ = frameId;
        onPoseUpdate(session, frameRequest);
      }
    }

  public:
    virtual void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest)
    {
      isInverseMatrixDirty_ = true;
    }

  public:
    XRSpaceSubType subType;

  protected:
    bool isReferenceSpace_;
    int32_t lastFrameId_;
    glm::mat4 baseMatrix_;
    glm::mat4 inverseMatrixCache_;
    bool isInverseMatrixDirty_ = true;
  };

  class XRReferenceSpace : public XRSpace
  {
  public:
    XRReferenceSpace(XRReferenceSpaceType type)
        : XRSpace(true),
          type_(type)
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override
    {
      if (type_ == XRReferenceSpaceType::kViewer)
      {
        baseMatrix_ = glm::make_mat4(frameRequest.viewerBaseMatrix);
        XRSpace::onPoseUpdate(session, frameRequest);
      }
      else if (type_ == XRReferenceSpaceType::kLocal)
      {
        baseMatrix_ = glm::make_mat4(frameRequest.localBaseMatrix);
        XRSpace::onPoseUpdate(session, frameRequest);
      }
      else if (type_ == XRReferenceSpaceType::kUnbounded)
      {
        baseMatrix_ = glm::make_mat4(frameRequest.localBaseMatrix) * math::getOriginMatrix();
        XRSpace::onPoseUpdate(session, frameRequest);
      }
    }

  public:
    XRReferenceSpaceType referenceSpaceType() const { return type_; }

  public:
    XRReferenceSpace getOffsetReferenceSpace(glm::mat4 offsetMatrix)
    {
      XRReferenceSpace offsetReferenceSpace(*this);
      offsetReferenceSpace.offsetMatrix_ = offsetMatrix;
      return offsetReferenceSpace;
    }

  private:
    XRReferenceSpaceType type_;
    glm::mat4 offsetMatrix_;
  };

  class XRViewSpace : public XRSpace
  {
  public:
    /**
     * Create a new `XRViewSpace` instance.
     * 
     * @param type The view space type.
     * @param projectionMatrix The projection matrix.
     * @returns The new `XRViewSpace` instance.
     */
    static inline std::shared_ptr<XRViewSpace> Make(XRViewSpaceType type, glm::mat4 projectionMatrix = glm::mat4(1.0f))
    {
      return std::make_shared<XRViewSpace>(type, projectionMatrix);
    }

  public:
    XRViewSpace(XRViewSpaceType type, glm::mat4 projectionMatrix)
        : XRSpace(),
          type_(type),
          projectionMatrix_(projectionMatrix)
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override
    {
      if (type_ == XRViewSpaceType::kNone)
        return; // No need to update the pose if the type is none.

      if (type_ != frameRequest.viewIndex)
      {
        std::cerr << "failed to update pose for XRViewSpace: viewIndex mismatch" << std::endl;
        return;
      }

      // TODO: check if a device
      auto &view = frameRequest.views[frameRequest.viewIndex];
      baseMatrix_ = glm::inverse(view.getViewMatrix());
      projectionMatrix_ = view.getProjectionMatrix();
      XRSpace::onPoseUpdate(session, frameRequest);
    }

  public:
    XREye eye() const { return type_; }
    glm::mat4 projectionMatrix() const { return projectionMatrix_; }

  private:
    XRViewSpaceType type_;
    glm::mat4 projectionMatrix_;
  };

  class XRJointSpace : public XRSpace
  {
  public:
    XRJointSpace(XRInputSource *inputSource, XRJointIndex index)
        : XRSpace(),
          inputSource(inputSource),
          index(index),
          name(xr::to_string(index))
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override
    {
      // TODO
      XRSpace::onPoseUpdate(session, frameRequest);
    }

  public:
    XRInputSource *inputSource;
    XRJointIndex index;
    std::string name;
  };

  class XRTargetRayOrGripSpace : public XRSpace
  {
  public:
    XRTargetRayOrGripSpace(XRInputSource *inputSource, XRSpaceSubType targetRayOrGrip)
        : XRSpace(), inputSource(inputSource), subType(targetRayOrGrip)
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override
    {
      // TODO
      XRSpace::onPoseUpdate(session, frameRequest);
    }

  public:
    XRInputSource *inputSource;
    XRSpaceSubType subType;
  };
}
