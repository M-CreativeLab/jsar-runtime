#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <math/matrix.hpp>
#include "./common.hpp"

namespace client_xr
{
#define TR_XRSPACE_RELATIVE_TRANSFORM(space, baseSpace) baseSpace->inverseBaseMatrix() * space->baseMatrix()

  class XRSpace
  {
  public:
    XRSpace(bool isReferenceSpace = false)
        : lastFrameId_(-1),
          baseMatrix_(1.0f),
          isReferenceSpace_(isReferenceSpace)
    {
    }
    XRSpace(glm::mat4 baseMatrix, XRSpaceSubType subType)
        : subType(subType),
          lastFrameId_(-1),
          baseMatrix_(baseMatrix),
          isReferenceSpace_(false)
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
    XRSpaceSubType subType = XRSpaceSubType::kUnset;

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
    /**
     * Create a new `XRReferenceSpace` instance.
     *
     * @param type The reference space type, such as `XRReferenceSpaceType::kViewer`.
     * @returns The new `XRReferenceSpace` instance.
     */
    static std::shared_ptr<XRReferenceSpace> Make(XRReferenceSpaceType type)
    {
      return std::make_shared<XRReferenceSpace>(type);
    }

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
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

  public:
    XREye eye() const { return type_; }
    glm::mat4 &projectionMatrix() { return projectionMatrix_; }

  private:
    XRViewSpaceType type_;
    glm::mat4 projectionMatrix_;
  };

  class XRJointSpace : public XRSpace
  {
  public:
    /**
     * Create a new `XRJointSpace` instance.
     *
     * @param inputSource The input source.
     * @param index The joint index.
     * @returns The new `XRJointSpace` instance.
     */
    static inline std::shared_ptr<XRJointSpace> Make(std::shared_ptr<XRInputSource> inputSource, XRJointIndex index)
    {
      return std::make_shared<XRJointSpace>(inputSource, index);
    }

  public:
    XRJointSpace(std::shared_ptr<XRInputSource> inputSource, XRJointIndex index)
        : XRSpace(),
          inputSource(inputSource),
          index(index),
          name(xr::to_string(index))
    {
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

  public:
    std::shared_ptr<XRInputSource> inputSource;
    XRJointIndex index;
    std::string name;
  };

  class XRTargetRayOrGripSpace : public XRSpace
  {
  public:
    /**
     * Create a new `XRTargetRayOrGripSpace` instance.
     *
     * @param inputSource The input source.
     * @param targetRayOrGrip The target ray or grip space.
     * @returns The new `XRTargetRayOrGripSpace` instance.
     */
    static std::shared_ptr<XRTargetRayOrGripSpace> Make(std::shared_ptr<XRInputSource> inputSource, XRSpaceSubType targetRayOrGrip)
    {
      return std::make_shared<XRTargetRayOrGripSpace>(inputSource, targetRayOrGrip);
    }

  public:
    XRTargetRayOrGripSpace(std::shared_ptr<XRInputSource> inputSource, XRSpaceSubType targetRayOrGrip)
        : XRSpace(), inputSource(inputSource)
    {
      this->subType = targetRayOrGrip;
    }

  public:
    void onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest) override;

  public:
    std::shared_ptr<XRInputSource> inputSource;
  };
}
