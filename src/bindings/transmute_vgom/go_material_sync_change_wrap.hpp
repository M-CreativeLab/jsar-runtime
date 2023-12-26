#pragma once

#include <napi.h>

#include "transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class GameObjectMaterialSyncChangeWrap : public Napi::ObjectWrap<GameObjectMaterialSyncChangeWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        GameObjectMaterialSyncChangeWrap(const Napi::CallbackInfo &info);

    public:
        static Napi::FunctionReference *constructor;

    private:
        MaterialSyncChangeOnGameObject *native_handle_;
    };
} // namespace gom
