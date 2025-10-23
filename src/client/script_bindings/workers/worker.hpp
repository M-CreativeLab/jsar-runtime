#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>
#include <client/workers/worker.hpp>

namespace script_bindings
{
  namespace workers_bindings
  {
    class Worker;
    using WorkerBase = scripting_base::ObjectWrap<Worker, client_workers::Worker, EventTarget>;

    class Worker : public WorkerBase
    {
      friend class scripting_base::ObjectWrap<Worker, client_workers::Worker, EventTarget>;
      using WorkerBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "Worker";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      Worker(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    protected:
      void onCreated() override;

    private:
      void PostMessage(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Terminate(const v8::FunctionCallbackInfo<v8::Value> &args);

      // Event handlers
      static void OnMessage(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void OnMessageError(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void OnError(const v8::FunctionCallbackInfo<v8::Value> &args);

      // Helpers
      static bool ValidateAndUnwrap(const v8::FunctionCallbackInfo<v8::Value> &args, Worker **, std::string &);
      static std::shared_ptr<dom::events::MessageEvent> CreateMessageEvent(v8::Isolate *isolate,
                                                                           v8::Local<v8::Object> value);

    private:
      v8::Global<v8::Object> worker_impl_handle_;

    private:
      static v8::Persistent<v8::Function> WorkerImplConstructor;
    };
  }
}
