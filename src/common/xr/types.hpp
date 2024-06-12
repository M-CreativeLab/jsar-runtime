#pragma once

#include "common/events/event.hpp"

namespace xr
{
  enum class TrStereoRenderingMode
  {
    MultiPass = 0,
    SinglePass = 1,
    SinglePassInstanced = 2,
    SinglePassMultiview = 3,
    Unknown = -1,
  };

  enum class TrSessionMode
  {
    ImmersiveAR = 0,
    ImmersiveVR = 1,
    Inline = 2,
  };

  class TrDeviceInit
  {
  public:
    bool enabled = false;
    bool active = false;
    TrStereoRenderingMode stereoRenderingMode = TrStereoRenderingMode::Unknown;
    /** No need to set */
    int commandChanPort = 0;
  };

  class TrDeviceInitResponse : public events::TrRpcResponse
  {
  public:
    TrDeviceInitResponse() : events::TrRpcResponse()
    {
      enabled = false;
      isDeviceActive = false;
      stereoRenderingMode = TrStereoRenderingMode::Unknown;
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
