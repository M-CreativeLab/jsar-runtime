#pragma once

#include <glm/glm.hpp>
#include "idgen.hpp"
#include "common/xr/types.hpp"
#include "common/classes.hpp"

namespace xr
{
  using namespace std;
  using namespace renderer;

  class TrXRSession
  {
  public:
    TrXRSession(uint32_t id, TrContentRenderer *contentRenderer, TrXRSessionInit &init);
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
    glm::mat4 getLocalBaseMatrix() { return localBaseMatrix; }
    void setLocalBaseMatrix(glm::mat4 matrix) { localBaseMatrix = matrix; }

  public:
    uint32_t id;
    TrXRSessionInit init;

  private:
    TrContentRenderer *contentRenderer = nullptr;
    glm::mat4 localBaseMatrix = glm::mat4(1.0f);
  };
}
