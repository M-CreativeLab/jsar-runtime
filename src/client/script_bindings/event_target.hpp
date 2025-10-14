#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <node/uv.h>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/dom_event_target.hpp>

namespace script_bindings
{
  class EventTarget;
  using EventTargetBase = scripting_base::ObjectWrap<EventTarget, ::dom::DOMEventTarget>;

  class EventListenersList : std::vector<std::shared_ptr<v8::Global<v8::Function>>>
  {
  public:
    EventListenersList() = default;
    ~EventListenersList()
    {
      for (auto &listener : *this)
        listener->Reset();
      this->clear();
    }

  public:
    size_t count() const;
    void addListener(v8::Isolate *isolate, v8::Local<v8::Function> listener);
    void removeListener(v8::Isolate *isolate, v8::Local<v8::Function> listener);
    void dispatchEvent(v8::Isolate *isolate, v8::Local<v8::Value> recv, std::shared_ptr<dom::Event> event);
  };

  class EventTarget : public EventTargetBase
  {
    using EventTargetBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "EventTarget";
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static void EventCallback(uv_async_t *handle);

  public:
    virtual dom::DOMEventTargetType eventTargetType() const
    {
      return dom::DOMEventTargetType::kEventTarget;
    }

    EventTarget(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual ~EventTarget() override;

  protected:
    void registerEvent(const std::string &eventType);
    bool matchRegisteredEvent(const std::string &eventType) const;

  private:
    void onDataUpdated() override;

    // internal event methods
    void initAsyncHandle();
    void listenerCallback(dom::DOMEventType type, shared_ptr<dom::Event> event);
    void setPendingEventAndDispatch(shared_ptr<dom::Event> event, const EventListenersList &listeners);
    void didDispatchPendingEvent();

    // Exposed Event methods
    void AddEventListener(const v8::FunctionCallbackInfo<v8::Value> &info);
    void RemoveEventListener(const v8::FunctionCallbackInfo<v8::Value> &info);
    void DispatchEvent(const v8::FunctionCallbackInfo<v8::Value> &info);

  private:
    std::vector<std::string> registered_event_types_;
    std::shared_ptr<dom::Event> pending_event_;
    std::unique_ptr<uv_async_t> async_handle_;                // libuv async handle for event dispatching if dispatcher is on another thread
    std::optional<std::thread::id> creating_async_thread_id_; // Thread that created the async handle

    std::mutex dispatch_mutex_;
    std::condition_variable dispatch_cv_;
    std::shared_mutex event_listeners_mutex_;
    std::unordered_map<std::string, EventListenersList> event_listeners_;
  };
}
