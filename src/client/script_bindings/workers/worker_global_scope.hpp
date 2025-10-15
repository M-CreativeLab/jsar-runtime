#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>
#include <client/workers/worker_global_scope.hpp>

namespace script_bindings
{
  namespace workers_bindings
  {
    class WorkerGlobalScope;
    using WorkerGlobalScopeBase = scripting_base::ObjectWrap<WorkerGlobalScope,
                                                             client_workers::WorkerGlobalScope,
                                                             EventTarget>;

    class WorkerGlobalScope : public WorkerGlobalScopeBase
    {
      using WorkerGlobalScopeBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "WorkerGlobalScope";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::ObjectTemplate> GetInstanceTemplate(v8::Isolate *isolate);
      /**
       * Create a new `WorkerGlobalScope` object and wrap it within the given object.
       */
      static v8::Local<v8::Object> MakeAndWrap(v8::Isolate *isolate,
                                               v8::Local<v8::Object> object,
                                               std::shared_ptr<client_workers::WorkerGlobalScope>);

    public:
      WorkerGlobalScope(v8::Isolate *, std::shared_ptr<client_workers::WorkerGlobalScope>);
    };
  }
}
