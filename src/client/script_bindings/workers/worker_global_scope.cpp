#include "./worker_global_scope.hpp"

using namespace std;
using namespace v8;

namespace script_bindings::workers_bindings
{
  void WorkerGlobalScope::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    auto prototype = tpl->PrototypeTemplate();
    auto instance = tpl->InstanceTemplate();

    InstanceMethod(isolate, prototype, "fetch", &WorkerGlobalScope::Fetch);
  }

  Local<ObjectTemplate> WorkerGlobalScope::GetInstanceTemplate(Isolate *isolate)
  {
    EscapableHandleScope scope(isolate);
    return scope.Escape(GetFunctionTemplate(isolate)->InstanceTemplate());
  }

  Local<Object> WorkerGlobalScope::MakeAndWrap(Isolate *isolate,
                                               Local<Object> object,
                                               shared_ptr<client_workers::WorkerGlobalScope> data_handle)
  {
    EscapableHandleScope scope(isolate);
    WorkerGlobalScope *workerGlobalScope = new WorkerGlobalScope(isolate, data_handle);
    workerGlobalScope->onCreated();
    Wrap(isolate, object, workerGlobalScope);
    return scope.Escape(object);
  }

  WorkerGlobalScope::WorkerGlobalScope(Isolate *isolate, shared_ptr<client_workers::WorkerGlobalScope> data_handle)
      : WorkerGlobalScopeBase(isolate)
  {
    setData(data_handle);
    assert(handle() != nullptr && "Native WorkerGlobalScope must not be null.");
  }

  void WorkerGlobalScope::Fetch(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "fetch", "Not implemented.")));
    return;
  }
}
