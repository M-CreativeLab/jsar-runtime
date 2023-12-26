#pragma once

#include <napi.h>

#include "transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class VirtualGameObjectWrap : public Napi::ObjectWrap<VirtualGameObjectWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        VirtualGameObjectWrap(const Napi::CallbackInfo &info);

    private:
        Napi::Value GetGuid(const Napi::CallbackInfo &info);

    public:
        static Napi::FunctionReference *constructor;

    private:
        VirtualGameObject *native_handle_;
    };
} // namespace gom
