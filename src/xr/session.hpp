#pragma once

#include <glm/glm.hpp>
#include "idgen.hpp"
#include "common/xr/types.hpp"
#include "common/classes.hpp"
#include "common/collision/ray.hpp"
#include "common/collision/culling/bounding_info.hpp"

namespace xr
{
  using namespace std;
  using namespace renderer;
  using namespace collision;

  class TrXRSession
  {
  public:
    TrXRSession(uint32_t id, Device *xrDevice, TrContentRenderer *contentRenderer, TrXRSessionInit &init);
    ~TrXRSession();

  public:
    /**
     * Check if the session belongs to the content by `contentRenderer` pointer.
     */
    bool belongsTo(TrContentRenderer *contentRenderer);
    /**
     * Check if the session belongs to the content by its pid.
     */
    bool belongsTo(pid_t contentPid);
    /**
     * Check if this session is active.
     *
     * @returns true if the session is active, otherwise false.
     */
    bool isActive();
    glm::mat4 getLocalBaseMatrix();
    void setLocalBaseMatrix(glm::mat4 matrix);
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

  private:
    Device *xrDevice = nullptr;
    TrConstellation *constellation = nullptr;
    TrContentRenderer *contentRenderer = nullptr;
  };
}
