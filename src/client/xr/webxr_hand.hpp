#pragma once

#include "./common.hpp"

namespace client_xr
{
  class XRHand
  {
  public:
    XRHand()
    {
    }

  private:
    XRInputSource *inputSource_;
  };
}
