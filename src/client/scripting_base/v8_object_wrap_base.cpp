#include <node/node.h>
#include <client/per_process.hpp>

#include "./v8_object_wrap_base.hpp"
#include "./v8_object_wrap.hpp"
#include "./v8_object_holder.hpp"

using namespace std;
using namespace v8;

namespace scripting_base
{
  BaseObject::BaseObject(Isolate *isolate)
      : current_isolate_(isolate)
      , napi_env_(nullptr)
  {
  }

  BaseObject::BaseObject(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : current_isolate_(isolate)
      , napi_env_(nullptr)
  {
  }

  BaseObject::BaseObject(const BaseObject &other)
      : current_isolate_(other.current_isolate_)
      , napi_env_(other.napi_env_)
  {
  }

  BaseObject::~BaseObject()
  {
    object_handle_.Reset();
  }

  uv_loop_t *BaseObject::getEventLoop() const
  {
    auto current_event_loop = TrClientContextPerProcess::Get()->getScriptingEventLoop();
    assert(current_event_loop != nullptr && "event loop is not available");
    return current_event_loop;
  }

  bool BaseObject::hasData() const
  {
    return data_handle_ != nullptr;
  }

  void BaseObject::setData(std::shared_ptr<JSObjectHolder> handle)
  {
    // Update the data handle.
    data_handle_ = handle;

    // Set the weak reference back to this object.
    if (data_handle_ != nullptr)
      data_handle_->setReference(this);

    // Notify subclass that the data has been updated.
    onDataUpdated();
  }

  void BaseObject::setNapiEnv(napi_env env)
  {
    napi_env_ = env;
  }

  Local<Object> BaseObject::getJSObject(Isolate *isolate) const
  {
    return object_handle_.Get(isolate);
  }

  Local<Object> BaseObject::This() const
  {
    EscapableHandleScope scope(current_isolate_);
    return scope.Escape(object_handle_.Get(current_isolate_));
  }
}
