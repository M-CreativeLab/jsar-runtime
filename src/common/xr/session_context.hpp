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
                                                           stereoId(that.stereoId.load())
    {
      memcpy(localBaseMatrix, that.localBaseMatrix, sizeof(localBaseMatrix));
    }
    ~TrXRSessionContextData()
    {
    }

  public:
    int getPendingStereoFramesCount()
    {
      return pendingStereoFramesCount.load();
    }
    void setPendingStereoFramesCount(int count)
    {
      pendingStereoFramesCount = count;
    }
    void setLocalBaseMatrix(glm::mat4 &baseMatrix)
    {
      memcpy(localBaseMatrix, glm::value_ptr(baseMatrix), sizeof(localBaseMatrix));
    }
    void setLocalBaseMatrix(float *matrixValues)
    {
      memcpy(localBaseMatrix, matrixValues, sizeof(localBaseMatrix));
    }

  public:
    /**
     * The session id.
     */
    atomic<uint32_t> sessionId;
    /**
     * The frame request id.
     */
    atomic<uint32_t> stereoId;
    /**
     * The pending stereo frames to be executed.
     *
     * NOTE: Client will read this value to know how busy the server is, but the strategy to skip frames is up to the client-side.
     */
    atomic<int> pendingStereoFramesCount = 0;
    /**
     * The session's local base matrix.
     */
    float localBaseMatrix[16];
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

  public:
    uint32_t getSessionId() { return data->sessionId; }
    uint32_t getStereoId() { return data->stereoId; }
    int getPendingStereoFramesCount() { return data->pendingStereoFramesCount; }
    void setStereoId(uint32_t id) { data.get()->stereoId = id; }
    void setPendingStereoFramesCount(int count) { data->setPendingStereoFramesCount(count); }
    void setLocalBaseMatrix(float *matrixValues) { data->setLocalBaseMatrix(matrixValues); }
  };
}
