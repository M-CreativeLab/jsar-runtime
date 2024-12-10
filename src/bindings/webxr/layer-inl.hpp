#include <client/per_process.hpp>
#include "./layer.hpp"

namespace bindings
{
  template <typename ObjectType, typename HandleType>
  XRLayerBase<ObjectType, HandleType>::XRLayerBase(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ObjectType>(info)
  {
    clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);
  }
}
