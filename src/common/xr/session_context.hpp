#pragma once

#include <vector>
#include <optional>
#include "idgen.hpp"
#include "common/zone.hpp"
#include "./common.hpp"

namespace xr
{
  class TrXRSessionContextData
  {
  public:
    TrXRSessionContextData(uint32_t sessionId) : sessionId(sessionId)
    {
      glm::mat4 identity = glm::mat4(1.0f);
      memcpy(localBaseMatrix, glm::value_ptr(identity), sizeof(localBaseMatrix));
    }
    TrXRSessionContextData(TrXRSessionContextData &that) : sessionId(that.sessionId.load()),
                                                           stereoId(that.stereoId.load()),
                                                           timestampOnSettingStereoId(that.timestampOnSettingStereoId),
                                                           pendingStereoFramesCount(that.pendingStereoFramesCount.load())
    {
      memcpy(localBaseMatrix, that.localBaseMatrix, sizeof(localBaseMatrix));
    }
    ~TrXRSessionContextData()
    {
    }

  public:
    void setStereoId(uint32_t id)
    {
      stereoId = id;
      auto now = chrono::time_point_cast<chrono::milliseconds>(chrono::steady_clock::now());
      timestampOnSettingStereoId = now.time_since_epoch().count();
    }
    int getPendingStereoFramesCount()
    {
      return pendingStereoFramesCount.load();
    }
    void setPendingStereoFramesCount(int count)
    {
      pendingStereoFramesCount = count;
    }
    void setInFrustum(bool value)
    {
      inFrustum = value;
    }
    void setLocalBaseMatrix(glm::mat4 &baseMatrix)
    {
      memcpy(localBaseMatrix, glm::value_ptr(baseMatrix), sizeof(localBaseMatrix));
    }
    void setLocalBaseMatrix(float *matrixValues)
    {
      memcpy(localBaseMatrix, matrixValues, sizeof(localBaseMatrix));
    }
    void getCollisionBoxMinMax(float *min, float *max)
    {
      min[0] = collisionBoxMinMax[0];
      min[1] = collisionBoxMinMax[1];
      min[2] = collisionBoxMinMax[2];
      max[0] = collisionBoxMinMax[3];
      max[1] = collisionBoxMinMax[4];
      max[2] = collisionBoxMinMax[5];
    }
    void setCollisionBoxMinMax(float *min, float *max)
    {
      collisionBoxMinMax[0] = min[0];
      collisionBoxMinMax[1] = min[1];
      collisionBoxMinMax[2] = min[2];
      collisionBoxMinMax[3] = max[0];
      collisionBoxMinMax[4] = max[1];
      collisionBoxMinMax[5] = max[2];
    }

  public: // Fields for the server-side
    /**
     * The session id.
     */
    atomic<uint32_t> sessionId;
    /**
     * The frame request id.
     */
    atomic<uint32_t> stereoId;
    /**
     * The timestamp will be updated when setting the stereo id.
     */
    long long timestampOnSettingStereoId;
    /**
     * The pending stereo frames to be executed.
     *
     * NOTE: Client will read this value to know how busy the server is, but the strategy to skip frames is up to the client-side.
     */
    atomic<int> pendingStereoFramesCount = 0;
    /**
     * The flag to indicate if the session's content is in the viewer's frustum, the client-side will read this value to know
     * if the session's content should be visible.
     */
    bool inFrustum = false;
    /**
     * The session's local base matrix.
     */
    float localBaseMatrix[16];

  public: // Fields for the client-side
    /**
     * The min & max vectors to describe the collision box.
     */
    float collisionBoxMinMax[6] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
  };

  class TrXRSessionContextZone : public TrZone<TrXRSessionContextData>
  {
  public:
    TrXRSessionContextZone(string filename, TrZoneType type, optional<uint32_t> sessionId = nullopt) : TrZone<TrXRSessionContextData>(filename, type)
    {
      if (type == TrZoneType::Server)
      {
        assert(sessionId.has_value());
        data = std::make_unique<TrXRSessionContextData>(sessionId.value());
      }
      else
      {
        data.reset(getData());
      }
    }

  protected:
    void updateData(TrXRSessionContextData *sharedSessionContext)
    {
      data->setCollisionBoxMinMax(sharedSessionContext->collisionBoxMinMax,
                                  sharedSessionContext->collisionBoxMinMax + 3);
    }

  public:
    uint32_t getSessionId() { return data->sessionId; }
    uint32_t getStereoId() { return data->stereoId; }
    int getPendingStereoFramesCount() { return data->pendingStereoFramesCount; }
    void setStereoId(uint32_t id) { data->setStereoId(id); }
    void setPendingStereoFramesCount(int count) { data->setPendingStereoFramesCount(count); }
    void setInFrustum(bool value) { data->setInFrustum(value); }
    void setLocalBaseMatrix(float *matrixValues) { data->setLocalBaseMatrix(matrixValues); }
    void getCollisionBoxMinMax(float *min, float *max) { data->getCollisionBoxMinMax(min, max); }
    void setCollisionBoxMinMax(float *min, float *max) { data->setCollisionBoxMinMax(min, max); }
  };
}
