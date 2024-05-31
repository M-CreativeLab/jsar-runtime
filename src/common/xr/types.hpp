#pragma once

#include "common/events/event.hpp"

namespace xr
{
  enum TrStereoRenderingMode
  {
    TR_MULTI_PASS = 0,
    TR_SINGLE_PASS = 1,
    TR_SINGLE_PASS_INSTANCED = 2,
    TR_SINGLE_PASS_MULTIVIEW = 3,
    TR_UNKNOWN = -1,
  };

  class TrDeviceInitResponse : public events::TrRpcResponse
  {
  public:
    TrDeviceInitResponse() : events::TrRpcResponse()
    {
      enabled = false;
      isDeviceActive = false;
      stereoRenderingMode = TrStereoRenderingMode::TR_UNKNOWN;
    }

  public:
    string serialize()
    {
      doc.AddMember("enabled", enabled, doc.GetAllocator());
      doc.AddMember("isDeviceActive", isDeviceActive, doc.GetAllocator());
      doc.AddMember("stereoRenderingMode", stereoRenderingMode, doc.GetAllocator());
      return events::TrRpcResponse::serialize();
    }

  public:
    bool enabled;
    bool isDeviceActive;
    TrStereoRenderingMode stereoRenderingMode;
  };
}
