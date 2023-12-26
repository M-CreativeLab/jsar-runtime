#pragma once

#include <napi.h>

#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class VirtualQuaternionWrap : public Napi::ObjectWrap<VirtualQuaternionWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        VirtualQuaternionWrap(const Napi::CallbackInfo &info);
        VirtualQuaternion *getNativeHandle();

    public:
        static Napi::FunctionReference *constructor;

    private:
        VirtualQuaternion *native_handle_;
    };
} // namespace gom
