#pragma once

#include <napi.h>

#include "proto/transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class VirtualTextureWrap : public Napi::ObjectWrap<VirtualTextureWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        VirtualTextureWrap(const Napi::CallbackInfo &info);

    private:
        // guid
        Napi::Value GetGuid(const Napi::CallbackInfo &info);
        void SetGuid(const Napi::CallbackInfo &info, const Napi::Value &value);

        // height
        Napi::Value GetHeight(const Napi::CallbackInfo &info);
        void SetHeight(const Napi::CallbackInfo &info, const Napi::Value &value);

        // width
        Napi::Value GetWidth(const Napi::CallbackInfo &info);
        void SetWidth(const Napi::CallbackInfo &info, const Napi::Value &value);

        // invertY
        Napi::Value GetInvertY(const Napi::CallbackInfo &info);
        void SetInvertY(const Napi::CallbackInfo &info, const Napi::Value &value);

        // Methods
        Napi::Value SetPixels(const Napi::CallbackInfo &info);
        Napi::Value SetSourceUrl(const Napi::CallbackInfo &info);

    public:
        static Napi::FunctionReference *constructor;
        VirtualTexture *getNativeHandle();

    private:
        VirtualTexture *native_handle_;
    };
} // namespace gom
