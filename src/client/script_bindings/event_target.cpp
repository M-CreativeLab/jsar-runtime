#include <iostream>
#include <string>
#include <sstream>
#include <client/scripting_base/v8_utils.hpp>

#include "./event_target.hpp"
#include "./event.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void EventListenersList::reset()
  {
    clear();
  }

  size_t EventListenersList::count() const
  {
    return size();
  }

  void EventListenersList::addListener(Isolate *isolate, Local<Function> listener)
  {
    HandleScope scope(isolate);
    auto persistent = make_shared<Global<Function>>(isolate, listener);
    emplace_back(persistent);
  }

  void EventListenersList::removeListener(Isolate *isolate, Local<Function> listener)
  {
    for (auto it = begin(); it != end();)
    {
      if (it->get()->Get(isolate) == listener)
      {
        it->get()->Reset();
        it = erase(it);
      }
      else
      {
        ++it;
      }
    }
  }

  void EventListenersList::dispatchEvent(Isolate *isolate, Local<Value> recv, shared_ptr<dom::Event> event) const
  {
    if (count() == 0 || event == nullptr)
      return;

    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    // Wrap the native event into a V8 Event object
    Local<Object> eventObj = Event::GetOrNewInstance(isolate, event);
    assert(!eventObj.IsEmpty() && "Failed to create Event object");

    // Call each listener with the event object
    for (const auto &listener : *this)
    {
      Local<Function> func = listener->Get(isolate);
      if (!func.IsEmpty())
      {
        constexpr int argc = 1;
        Local<Value> argv[argc] = {eventObj};

        TryCatch try_catch(isolate);
        MaybeLocal<Value> res = func->Call(context, recv, argc, argv);
        if (res.IsEmpty() || try_catch.HasCaught())
        {
          string message = scripting_base::ReportExceptionToString(isolate, try_catch.Exception());
          cerr << "Failed to dispatch event '" << event->typeStr() << "': " << message << endl;
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

  EventTarget::EventTarget(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
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
      auto afterClose = [](uv_handle_t *handle)
      {
        delete reinterpret_cast<uv_async_t *>(handle);
      };
      uv_close(reinterpret_cast<uv_handle_t *>(async_handle_.release()), afterClose);
    }
  }

  void EventTarget::registerEvent(const string &event_type)
  {
    if (matchRegisteredEvent(event_type))
      return; // Already registered

    // Register the event type
    registered_event_types_.emplace_back(event_type);
    initAsyncHandle();
  }

  bool EventTarget::matchRegisteredEvent(const string &event_type) const
  {
    return find(registered_event_types_.begin(),
                registered_event_types_.end(),
                event_type) != registered_event_types_.end();
  }

  void EventTarget::onDataUpdated()
  {
    handle()->resetGlobalEventListener([this](dom::DOMEventType type, shared_ptr<dom::Event> event)
                                       { this->listenerCallback(type, event); });
  }

  void EventTarget::initAsyncHandle()
  {
    if (async_handle_)
      return;

    async_handle_ = make_unique<uv_async_t>();
    async_handle_->data = this;
    uv_async_init(getEventLoop(), async_handle_.get(), EventCallback);
    creating_async_thread_id_ = this_thread::get_id();
  }

  void EventTarget::listenerCallback(dom::DOMEventType type, shared_ptr<dom::Event> event)
  {
    string event_type = event->typeStr();

    bool is_event_registered = false;
    bool is_listener_found = false;
    EventListenersList listeners;
    {
      shared_lock<shared_mutex> lock(event_listeners_mutex_);

      if (event_listeners_.size() == 0 &&
          registered_event_types_.size() == 0)
      {
        return;
      }

      // Check if the event type is registered
      if (find(registered_event_types_.begin(),
               registered_event_types_.end(),
               event_type) != registered_event_types_.end())
        is_event_registered = true;

      // Check if there are listeners for the event type
      const auto &it = event_listeners_.find(event_type);
      if (it != event_listeners_.end() &&
          it->second.count() > 0)
      {
        is_listener_found = true;
        listeners = it->second; // Copy the listeners to avoid holding the lock during dispatch
      }
    }

    // Skip if the event is neither registered nor has listeners
    if (!is_event_registered &&
        !is_listener_found)
    {
      return;
    }

    // Dispatch the event to the found listeners
    setPendingEventAndDispatch(event, listeners);
  }

  void EventTarget::setPendingEventAndDispatch(shared_ptr<dom::Event> event, const EventListenersList &listeners)
  {
    if (this_thread::get_id() == creating_async_thread_id_)
    {
      // Directly call if on the same thread as async handle creation
      didDispatchEvent(current_isolate_, event, listeners);
    }
    else
    {
      {
        unique_lock<mutex> lock(dispatch_mutex_);
        if (pending_event_ != nullptr)
        {
          dispatch_cv_.wait(lock, [this]()
                            { return pending_event_ == nullptr; });
        }
        pending_event_ = event;
        pending_listeners_ = listeners;
      }

      // Send async signal to the event loop to process the event
      assert(async_handle_ != nullptr && "Async handle is not initialized.");
      uv_async_send(async_handle_.get());
    }
  }

  void EventTarget::didDispatchPendingEvent()
  {
    unique_lock<mutex> lock(dispatch_mutex_);
    if (pending_event_) [[likely]]
    {
      Isolate::Scope isolate_scope(current_isolate_);
      didDispatchEvent(current_isolate_, pending_event_, pending_listeners_);

      pending_event_.reset();
      pending_listeners_.reset();
    }
    dispatch_cv_.notify_all();
  }

  void EventTarget::didDispatchEvent(Isolate *isolate,
                                     shared_ptr<dom::Event> event,
                                     const EventListenersList &listeners)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    string event_type = event->typeStr();

    // Dispatch the registered event first
    if (matchRegisteredEvent(event_type))
    {
      Local<String> event_handler_name = String::NewFromUtf8(isolate, ("on" + event_type).c_str())
                                           .ToLocalChecked();
      Local<Value> event_handler_val = This()->Get(context, event_handler_name).ToLocalChecked();
      if (event_handler_val->IsFunction())
      {
        Local<Function> event_handler = event_handler_val.As<Function>();
        Local<Object> event_object = Event::GetOrNewInstance(isolate, event);
        if (!event_object.IsEmpty())
        {
          Local<Value> argv[] = {event_object};
          // TODO(yorkie): Handle exceptions
          event_handler->Call(context, This(), 1, argv).ToLocalChecked();
        }
      }
    }

    // Dispatch the event to the listeners
    listeners.dispatchEvent(isolate, value(), event);
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
    string eventType = *typeString;

    // Initialize the async handle if not already done
    initAsyncHandle();

    // Add the listener to the list for the event type
    {
      unique_lock<shared_mutex> lock(event_listeners_mutex_);
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
