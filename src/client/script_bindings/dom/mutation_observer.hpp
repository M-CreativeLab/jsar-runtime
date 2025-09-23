#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/mutation_observer.hpp>

namespace script_bindings
{
  class MutationObserver;
  using MutationObserverBase = scripting_base::ObjectWrap<MutationObserver, dom::MutationObserver>;

  /**
   * MutationObserver wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::MutationObserver objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM MutationObserver interface for observing DOM mutations.
   */
  class MutationObserver : public MutationObserverBase
  {
    using MutationObserverBase::ObjectWrap;

  public:
    /**
     * The name of the MutationObserver class for V8.
     */
    static std::string Name()
    {
      return "MutationObserver";
    }

    /**
     * Configure the V8 function template with MutationObserver properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 MutationObserver instance from a native dom::MutationObserver.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::MutationObserver> nativeObserver);

    /**
     * Initialize the MutationObserver class and register it with V8.
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

  public:
    MutationObserver(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Core methods
    static void Observe(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Disconnect(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void TakeRecords(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}