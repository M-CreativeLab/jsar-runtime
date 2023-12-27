#pragma once

#include <napi.h>

#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
  class VirtualVector3Wrap : public Napi::ObjectWrap<VirtualVector3Wrap>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    VirtualVector3Wrap(const Napi::CallbackInfo &info);
    VirtualVector3 *getNativeHandle();

  public:
    static Napi::FunctionReference *constructor;

  private:
    VirtualVector3 *native_handle_;
  };
} // namespace gom
