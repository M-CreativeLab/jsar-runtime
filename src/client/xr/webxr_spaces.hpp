#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "./common.hpp"

namespace client_xr
{
  class XRSpace
  {
  public:
    XRSpace(bool isReferenceSpace = false)
        : lastFrameId_(-1),
          baseMatrix_(1.0f),
          isReferenceSpace_(isReferenceSpace)
    {
    }

  public:
    glm::mat4 &baseMatrix();
    glm::mat4 &inverseBaseMatrix();
    void ensurePoseUpdated(uint32_t frameId, std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest);

  public:
    virtual void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest);

  public:
    XRSpaceSubType subType;

  private:
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
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

  public:
    XRReferenceSpaceType referenceSpaceType() const { return type_; }

  public:
    glm::mat4 offsetMatrix;

  private:
    XRReferenceSpaceType type_;
  };

  class XRViewSpace : public XRSpace
  {
  public:
    XRViewSpace(XRViewSpaceType type, glm::mat4 projectionMatrix)
        : XRSpace(),
          type_(type),
          projectionMatrix_(projectionMatrix)
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

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
        : XRSpace(), inputSource(inputSource), index(index)
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

  public:
    XRInputSource *inputSource;
    XRJointIndex index;
  };

  class XRTargetRayOrGripSpace : public XRSpace
  {
  public:
    XRTargetRayOrGripSpace(XRInputSource *inputSource)
        : XRSpace(), inputSource(inputSource)
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

  public:
    XRInputSource *inputSource;
  };
}
