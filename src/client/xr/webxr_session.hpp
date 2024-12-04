#pragma once

#include <memory>

namespace client_xr
{
  class XRSession
  {
  public:
    static std::shared_ptr<XRSession> Make()
    {
      return std::make_shared<XRSession>();
    }

  public:
    XRSession() {}
  };
}