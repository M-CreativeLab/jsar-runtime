#pragma once

#include <napi.h>

#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class VirtualTransformWrap : public Napi::ObjectWrap<VirtualTransformWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        VirtualTransformWrap(const Napi::CallbackInfo &info);
        VirtualTransform *getNativeHandle();

    public:
        static Napi::FunctionReference *constructor;

    private:
        VirtualTransform *native_handle_;
    };
} // namespace gom
