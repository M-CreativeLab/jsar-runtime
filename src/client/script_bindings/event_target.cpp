#include <iostream>
#include "./event_target.hpp"
#include "./event.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  size_t EventListenersList::count() const
  {
    return this->size();
  }

  void EventListenersList::addListener(v8::Isolate *isolate, v8::Local<v8::Function> listener)
  {
    auto persistent = std::make_shared<v8::Persistent<v8::Function>>(isolate, listener);
    this->emplace_back(persistent);
  }

  void EventListenersList::removeListener(v8::Isolate *isolate, v8::Local<v8::Function> listener)
  {
    for (auto it = this->begin(); it != this->end();)
    {
      if (it->get()->Get(isolate) == listener)
      {
        it->get()->Reset();
        it = this->erase(it);
      }
      else
      {
        ++it;
      }
    }
  }

  void EventListenersList::dispatchEvent(v8::Isolate *isolate, std::shared_ptr<dom::Event> event)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    for (const auto &listener : *this)
    {
      Local<Function> func = listener->Get(isolate);
      if (!func.IsEmpty())
      {
        // Create a new Event object to pass to the listener
        Local<Object> eventObj = Event::GetOrNewInstance(isolate, event);
        if (!eventObj.IsEmpty())
        {
          Local<Value> argv[] = {eventObj};
          func->Call(context, Null(isolate), 1, argv).ToLocalChecked();
        }
      }
    }
  }

  // static
  void EventTarget::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // Add methods
    InstanceMethod(isolate, prototype, "addEventListener", &EventTarget::AddEventListener);
    InstanceMethod(isolate, prototype, "removeEventListener", &EventTarget::RemoveEventListener);
    InstanceMethod(isolate, prototype, "dispatchEvent", &EventTarget::DispatchEvent);
  }

  // static
  void EventTarget::EventCallback(uv_async_t *handle)
  {
    assert(handle != nullptr && "uv_async_t handle is null");
    assert(handle->data != nullptr && "uv_async_t handle data is null");

    EventTarget *self = static_cast<EventTarget *>(handle->data);
    self->didDispatchPendingEvent();
  }

  EventTarget::EventTarget(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
      : EventTargetBase(isolate, args)
      , async_handle_(nullptr)
  {
  }

  EventTarget::~EventTarget()
  {
    // Clean up any event listeners or resources
    if (hasData())
      handle()->resetGlobalEventListener();

    // Close the async handle if it exists
    if (async_handle_)
    {
      uv_close(reinterpret_cast<uv_handle_t *>(async_handle_.get()), nullptr);
      async_handle_.reset();
    }
  }

  void EventTarget::onDataUpdated()
  {
    async_handle_ = make_unique<uv_async_t>();
    async_handle_->data = this;
    uv_async_init(getEventLoop(), async_handle_.get(), EventCallback);

    handle()->resetGlobalEventListener([this](dom::DOMEventType type, shared_ptr<dom::Event> event)
                                       { this->listenerCallback(type, event); });
  }

  void EventTarget::listenerCallback(dom::DOMEventType type, shared_ptr<dom::Event> event)
  {
    shared_lock<shared_mutex> lock(event_listeners_mutex_);
    if (event_listeners_.size() == 0)
      return;

    auto listeners = event_listeners_.find(event->typeStr());
    if (listeners == event_listeners_.end() ||
        listeners->second.count() == 0)
      return;

    // Dispatch the event to the found listeners
    setPendingEventAndDispatch(event, listeners->second);
  }

  void EventTarget::setPendingEventAndDispatch(shared_ptr<dom::Event> event, const EventListenersList &listeners)
  {
    unique_lock<mutex> lock(dispatch_mutex_);
    dispatch_cv_.wait(lock, [this]()
                      { return pending_event_ == nullptr; });
    pending_event_ = event;

    // Send async signal to the event loop to process the event
    uv_async_send(async_handle_.get());
  }

  void EventTarget::didDispatchPendingEvent()
  {
    unique_lock<mutex> lock(dispatch_mutex_);
    if (!pending_event_) [[unlikely]]
      return;

    {
      HandleScope scope(current_isolate_);
      Local<Context> context = current_isolate_->GetCurrentContext();

      shared_lock<shared_mutex> lock(event_listeners_mutex_);
      auto listeners = event_listeners_.find(pending_event_->typeStr());
      if (listeners != event_listeners_.end())
        listeners->second.dispatchEvent(current_isolate_, pending_event_);
    }
    pending_event_.reset();
    dispatch_cv_.notify_all();
  }

  void EventTarget::AddEventListener(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "addEventListener", "addEventListener requires at least 2 arguments")));
      return;
    }

    if (!info[0]->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "addEventListener", "First argument must be a string")));
      return;
    }

    if (!info[1]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "addEventListener", "Second argument must be a function")));
      return;
    }

    String::Utf8Value typeString(isolate, info[0]);
    Local<Function> listener = info[1].As<Function>();

    {
      unique_lock<shared_mutex> lock(event_listeners_mutex_);
      string eventType = *typeString;
      if (event_listeners_.find(eventType) == event_listeners_.end())
        event_listeners_[eventType] = EventListenersList();
      event_listeners_[eventType].addListener(isolate, listener);
    }
    info.GetReturnValue().SetUndefined();
  }

  void EventTarget::RemoveEventListener(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "removeEventListener requires at least 2 arguments").ToLocalChecked()));
      return;
    }

    String::Utf8Value typeString(isolate, info[0]);
    Local<Function> listenerToRemove = info[1].As<Function>();

    {
      unique_lock<shared_mutex> lock(event_listeners_mutex_);
      string eventType = *typeString;
      auto it = event_listeners_.find(eventType);
      if (it != event_listeners_.end())
      {
        it->second.removeListener(isolate, listenerToRemove);
        if (it->second.count() == 0)
        {
          event_listeners_.erase(it);
        }
      }
    }
  }

  void EventTarget::DispatchEvent(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "dispatchEvent", "dispatchEvent requires 1 argument")));
      info.GetReturnValue().SetUndefined();
      return;
    }

    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "dispatchEvent", "First argument must be an Event object")));
      info.GetReturnValue().SetUndefined();
      return;
    }

    Event *event = Event::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (event == nullptr || event->handle() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "dispatchEvent", "Invalid Event object")));
      info.GetReturnValue().SetUndefined();
      return;
    }

    bool success = handle()->dispatchEvent(event->handle());
    info.GetReturnValue().Set(Boolean::New(isolate, success));
  }
}
