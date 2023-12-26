#pragma once

#include <napi.h>

#include "transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class VirtualGameObjectModelWrap : public Napi::ObjectWrap<VirtualGameObjectModelWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        VirtualGameObjectModelWrap(const Napi::CallbackInfo &info);
        ~VirtualGameObjectModelWrap();

    private:
        Napi::Value SetTitle(const Napi::CallbackInfo &info);
        Napi::Value SetMetadata(const Napi::CallbackInfo &info);
        Napi::Value CreateGameObjectAsChild(const Napi::CallbackInfo &info);
        Napi::Value CreateAnimationGroupAsChild(const Napi::CallbackInfo &info);
        Napi::Value CreatePropertyChange(const Napi::CallbackInfo &info);
        Napi::Value CreateMaterialSyncChange(const Napi::CallbackInfo &info);
        Napi::Value CreateAnimationGroupChange(const Napi::CallbackInfo &info);
        Napi::Value SerializeAndWrite(const Napi::CallbackInfo &info);
        Napi::Value IsBufferEmpty(const Napi::CallbackInfo &info);
        Napi::Value FetchBufferFromWritter(const Napi::CallbackInfo &info);
        Napi::Value FetchAndClearBufferFromWritter(const Napi::CallbackInfo &info);

    private:
        VirtualGameObjectModel *native_handle_;
    };
} // namespace gom
