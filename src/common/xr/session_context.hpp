#pragma once

#include <vector>
#include "idgen.hpp"
#include "common/zone.hpp"
#include "./common.hpp"

namespace xr
{
  class TrXRSessionContextData
  {
  public:
    TrXRSessionContextData(uint32_t id) : id(id)
    {
    }

  public:
    uint32_t id;
    uint32_t stereoId;
  };

  class TrXRSessionContextZone : public TrZone<TrXRSessionContextData>
  {
  public:
    TrXRSessionContextZone(string filename, TrZoneType type) : TrZone<TrXRSessionContextData>(filename, type)
    {
      if (type == TrZoneType::Server)
        data = std::make_unique<TrXRSessionContextData>();
      else
        data.reset(getData());
    }
  };
}
