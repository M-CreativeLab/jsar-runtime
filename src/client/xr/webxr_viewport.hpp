#pragma once

#include <client/scripting_base/v8_object_holder.hpp>
#include <xr/viewport.hpp>

namespace endor
{
  namespace client_xr
  {
    class XRViewport : public xr::Viewport,
                       public scripting_base::JSObjectHolder
    {
      using xr::Viewport::Viewport;
    };
  }
} // namespace endor
