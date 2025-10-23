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
      class MutationRecord;
      using MutationRecordBase = scripting_base::ObjectWrap<MutationRecord, dom::MutationRecord>;

      class MutationRecord : public MutationRecordBase
      {
        using MutationRecordBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "MutationRecord";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        MutationRecord(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void TypeGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void TargetGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void AddedNodesGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void RemovedNodesGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void PreviousSiblingGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void NextSiblingGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void AttributeNameGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void AttributeNamespaceGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
        void OldValueGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor