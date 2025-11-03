#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/mutation_observer.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace dom_bindings
    {
      class MutationObserver;
      using MutationObserverBase = scripting_base::ObjectWrap<MutationObserver, dom::MutationObserver>;

      class MutationObserver : public MutationObserverBase
      {
        using MutationObserverBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "MutationObserver";
        }

        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::MutationObserver> nativeObserver);
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
  }
} // namespace endor