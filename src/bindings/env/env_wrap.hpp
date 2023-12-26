#pragma once

#include <napi.h>

using namespace std;

namespace env
{
    class EnvironmentWrap : public Napi::ObjectWrap<EnvironmentWrap>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        EnvironmentWrap(const Napi::CallbackInfo &info);

    public:
        void SetReadyContext(const char *contextJson);

    private:
        /**
         * Get the ready context.
         */
        Napi::Value GetReadyContext(const Napi::CallbackInfo &info);
        /**
         * Mark the runtime is available to execute xsml source.
         */
        Napi::Value MarkRuntimeAvailable(const Napi::CallbackInfo &info);
        /**
         * Mark a GOM buffer(by a channel id) is errored and set the error code. Internally it finds the GOM buffer and mark it as errored.
         */
        Napi::Value MarkGomBufferAsErrored(const Napi::CallbackInfo &info);

    private:
        static Napi::FunctionReference *constructor;
    };
} // namespace env
