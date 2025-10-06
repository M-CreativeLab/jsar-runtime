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
    EscapableHandleScope scope(isolate);
    return scope.Escape(object_handle_.Get(isolate));
  }

  Local<Object> BaseObject::This() const
  {
    EscapableHandleScope scope(current_isolate_);
    return scope.Escape(object_handle_.Get(current_isolate_));
  }

  void BaseObject::Reset(Local<Object> object)
  {
    Isolate *isolate = current_isolate_;
    HandleScope scope(isolate);

    if (object_handle_.IsEmpty())
    {
      object_handle_.Reset(isolate, object);

      // TODO(yorkie): SetWeak causes this object can be collected by v8, considering mark weak only for the objects
      // created from JavaScript.
      object_handle_.SetWeak(this, Finalizer, WeakCallbackType::kParameter);
    }
  }

  // static
  void BaseObject::Finalizer(const WeakCallbackInfo<BaseObject> &data)
  {
    BaseObject *instance = data.GetParameter();
    if (instance != nullptr)
    {
      instance->object_handle_.Reset();

      // Clear the data handle's ref
      if (instance->data_handle_)
        instance->data_handle_->setReference(nullptr);
    }
    data.SetSecondPassCallback(Cleanup);
  }

  // static
  void BaseObject::Cleanup(const WeakCallbackInfo<BaseObject> &data)
  {
    BaseObject *instance = data.GetParameter();
    if (instance != nullptr)
      delete instance;
  }
}
