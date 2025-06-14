#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <idgen.hpp>
#include "common/xr/types.hpp"
#include "common/classes.hpp"
#include "common/collision/ray.hpp"
#include "common/collision/culling/bounding_info.hpp"

namespace xr
{
  using namespace std;
  using namespace renderer;
  using namespace collision;

  class TrXRSession final
  {
  public:
    /**
     * Construct a new instance of `xr::TrXRSession`.
     *
     * @param sessionId The session id.
     * @param device The WebXR device.
     * @param content The content source which the WebXR session belongs to.
     * @param mode The requested session mode.
     * @param init The requested session initialization info.
     * @returns The new `xr::TrXRSession` instance.
     */
    static inline std::shared_ptr<TrXRSession> Make(uint32_t sessionId, Device *device, std::shared_ptr<TrContentRuntime> content, TrXRSessionMode mode, TrXRSessionInit &init)
    {
      return std::make_shared<TrXRSession>(sessionId, device, content, mode, init);
    }

  public:
    TrXRSession(uint32_t id, Device *device, std::shared_ptr<TrContentRuntime> content, TrXRSessionMode mode, TrXRSessionInit &init);
    ~TrXRSession();

  public:
    /**
     * Execute the session tick.
     */
    void tick();
    /**
     * Check if this WebXR session belongs to the content by its pid.
     */
    bool belongsTo(pid_t contentPid);
    /**
     * Check if this WebXR session is active.
     *
     * @returns true if the session is active, otherwise false.
     */
    bool isActive();
    glm::mat4 getLocalBaseMatrix();
    /**
     * Set the local base matrix of this session.
     *
     * @param matrix The local base matrix.
     */
    void setLocalBaseMatrix(glm::mat4 matrix);
    /**
     * Get the collision box of this session.
     */
    void getCollisionBox(float *outMin, float *outMax)
    {
      return contextZone->getCollisionBoxMinMax(outMin, outMax);
    }
    /**
     * Set the stereo frame id to the session's context zone.
     *
     * @param id The stereo id.
     */
    void setStereoId(uint32_t id)
    {
      contextZone->setStereoId(id);
    }
    /**
     * Set the pending stereo frames count to the session's context zone.
     *
     * @param count The pending stereo frames count.
     */
    void setPendingStereoFramesCount(int count)
    {
      contextZone->setPendingStereoFramesCount(count);
    }
    /**
     * Update the states in zone.
     */
    void updateStatesInZone()
    {
      contextZone->setInFrustum(isInFrustum());
    }
    /**
     * Check if the session's content in the viewer's frustum.
     */
    bool isInFrustum();
    /**
     * Check if the session's content is completely in the viewer's frustum.
     */
    bool isCompletelyInFrustum();

  public:
    /**
     * The XR session id.
     */
    uint32_t id;
    /**
     * The XR session mode.
     */
    TrXRSessionMode mode;
    /**
     * The XR session initialization info including required and optional features.
     */
    TrXRSessionInit init;
    /**
     * The base matrix of this session that represents how the session's content local space is transformed in the world
     * space.
     *
     * NOTE: This base matrix scaling is always (1, 1, 1) because the WebXR `XRSpace` not includes the scale, or ensure the
     * scale is (1, 1, 1).
     */
    glm::mat4 baseMatrix;
    /**
     * The recommanded content scale for this session, this value is used for computing the default bounding info with the
     * base matrix which's scale will be ignored.
     *
     * And this value is also exposed to the application side to adjust the content size to match the default bounding info.
     */
    float recommendedContentSize = 0.3f;
    /**
     * The bounding info of this session, it is used for native frustum culling.
     *
     * TODO: This could be updated by application side to optimize the culling.
     * TODO: Support the mesh-based bounding info?
     */
    culling::TrBoundingInfo boundingInfo;
    uint32_t nextStereoId;

  private:
    Device *device = nullptr;
    TrConstellation *constellation = nullptr;
    std::weak_ptr<TrContentRuntime> content;
    /**
     * The session context zone for session-related shared data to client-side.
     */
    unique_ptr<TrXRSessionContextZone> contextZone;
  };
}
