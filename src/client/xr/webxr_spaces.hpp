#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <math/matrix.hpp>
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
    virtual ~XRSpace() = default;

  public:
    glm::mat4 &baseMatrix() { return baseMatrix_; }
    glm::mat4 &inverseBaseMatrix()
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
    XRViewSpace(XRViewSpaceType type, glm::mat4 projectionMatrix = glm::mat4(1.0f))
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

  /**
   * Convert a joint index to a string.
   *
   * @param index The joint index.
   * @returns The joint name.
   */
  inline std::string to_string(XRJointIndex index)
  {
    switch (index)
    {
    case XRJointIndex::JointWrist:
      return "wrist";
    case XRJointIndex::JointThumbMetacarpal:
      return "thumb-metacarpal";
    case XRJointIndex::JointThumbPhalanxProximal:
      return "thumb-phalanx-proximal";
    case XRJointIndex::JointThumbPhalanxDistal:
      return "thumb-phalanx-distal";
    case XRJointIndex::JointThumbTip:
      return "thumb-tip";
    case XRJointIndex::JointIndexFingerMetacarpal:
      return "index-finger-metacarpal";
    case XRJointIndex::JointIndexFingerPhalanxProximal:
      return "index-finger-phalanx-proximal";
    case XRJointIndex::JointIndexFingerPhalanxIntermediate:
      return "index-finger-phalanx-intermediate";
    case XRJointIndex::JointIndexFingerPhalanxDistal:
      return "index-finger-phalanx-distal";
    case XRJointIndex::JointIndexFingerTip:
      return "index-finger-tip";
    case XRJointIndex::JointMiddleFingerMetacarpal:
      return "middle-finger-metacarpal";
    case XRJointIndex::JointMiddleFingerPhalanxProximal:
      return "middle-finger-phalanx-proximal";
    case XRJointIndex::JointMiddleFingerPhalanxIntermediate:
      return "middle-finger-phalanx-intermediate";
    case XRJointIndex::JointMiddleFingerPhalanxDistal:
      return "middle-finger-phalanx-distal";
    case XRJointIndex::JointMiddleFingerTip:
      return "middle-finger-tip";
    case XRJointIndex::JointRingFingerMetacarpal:
      return "ring-finger-metacarpal";
    case XRJointIndex::JointRingFingerPhalanxProximal:
      return "ring-finger-phalanx-proximal";
    case XRJointIndex::JointRingFingerPhalanxIntermediate:
      return "ring-finger-phalanx-intermediate";
    case XRJointIndex::JointRingFingerPhalanxDistal:
      return "ring-finger-phalanx-distal";
    case XRJointIndex::JointRingFingerTip:
      return "ring-finger-tip";
    case XRJointIndex::JointPinkyFingerMetacarpal:
      return "pinky-finger-metacarpal";
    case XRJointIndex::JointPinkyFingerPhalanxProximal:
      return "pinky-finger-phalanx-proximal";
    case XRJointIndex::JointPinkyFingerPhalanxIntermediate:
      return "pinky-finger-phalanx-intermediate";
    case XRJointIndex::JointPinkyFingerPhalanxDistal:
      return "pinky-finger-phalanx-distal";
    case XRJointIndex::JointPinkyFingerTip:
      return "pinky-finger-tip";
    default:
      return "unknown";
    }
  }

  class XRJointSpace : public XRSpace
  {
  public:
    XRJointSpace(XRInputSource *inputSource, XRJointIndex index)
        : XRSpace(),
          inputSource(inputSource),
          index(index),
          name(to_string(index))
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
