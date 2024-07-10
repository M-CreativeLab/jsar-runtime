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
    TrXRSession(uint32_t id, Device* xrDevice, TrContentRenderer *contentRenderer, TrXRSessionInit &init);
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
    glm::mat4 getLocalBaseMatrix() { return baseMatrix; }
    void setLocalBaseMatrix(glm::mat4 matrix)
    {
      baseMatrix = matrix;
      boundingInfo.update(baseMatrix);
    }
    bool isInFrustum();
    bool isCompletelyInFrustum();

  public:
    uint32_t id;
    TrXRSessionInit init;
    glm::mat4 baseMatrix;
    culling::TrBoundingInfo boundingInfo;

  private:
    Device *xrDevice = nullptr;
    TrContentRenderer *contentRenderer = nullptr;
  };
}
