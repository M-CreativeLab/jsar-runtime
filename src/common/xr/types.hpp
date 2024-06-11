#pragma once

#include "common/events/event.hpp"

namespace xr
{
  enum class TrStereoRenderingMode
  {
    TR_MULTI_PASS = 0,
    TR_SINGLE_PASS = 1,
    TR_SINGLE_PASS_INSTANCED = 2,
    TR_SINGLE_PASS_MULTIVIEW = 3,
    TR_UNKNOWN = -1,
  };

  class TrDeviceInit
  {
  public:
    bool enabled = false;
    bool active = false;
    TrStereoRenderingMode stereoRenderingMode = TrStereoRenderingMode::TR_UNKNOWN;
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
      auto &allocator = doc.GetAllocator();
      dataValue.SetObject();
      dataValue.AddMember("enabled", enabled, allocator);
      dataValue.AddMember("isDeviceActive", isDeviceActive, allocator);
      dataValue.AddMember("stereoRenderingMode", static_cast<int>(stereoRenderingMode), allocator);
      return events::TrRpcResponse::serialize();
    }

  public:
    bool enabled;
    bool isDeviceActive;
    TrStereoRenderingMode stereoRenderingMode;
  };
}
