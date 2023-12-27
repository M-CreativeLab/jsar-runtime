#pragma once

#include <napi.h>

#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
  class VirtualMatrixWrap : public Napi::ObjectWrap<VirtualMatrixWrap>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    VirtualMatrixWrap(const Napi::CallbackInfo &info);
    VirtualMatrix *getNativeHandle();

  public:
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value Update(const Napi::CallbackInfo &info);

  private:
    VirtualMatrix *native_handle_;
  };
} // namespace gom
