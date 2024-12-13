#include <client/per_process.hpp>
#include "./layer.hpp"

namespace bindings
{
  template <typename ObjectType, typename HandleType>
  XRLayerBase<ObjectType, HandleType>::XRLayerBase(const Napi::CallbackInfo &info)
      : XRHandleWrap<ObjectType, HandleType>(info, false)
  {
    clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);
  }
}
