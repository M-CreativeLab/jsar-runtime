#pragma once

#include <napi.h>

#include "transmute_vgom.pb.h"

using namespace transmute::protocol::gom;

namespace gom
{
    class GameObjectPropertyChangeWrap : public Napi::ObjectWrap<GameObjectPropertyChangeWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        GameObjectPropertyChangeWrap(const Napi::CallbackInfo &info);

    public:
        static Napi::FunctionReference *constructor;

    private:
        PropertyChangeOnGameObject *native_handle_;
    };
} // namespace gom
