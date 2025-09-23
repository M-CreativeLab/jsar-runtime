#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/mutation_record.hpp>

namespace script_bindings
{
  class MutationRecord;
  using MutationRecordBase = scripting_base::ObjectWrap<MutationRecord, dom::MutationRecord>;

  /**
   * MutationRecord wrapper for V8 objects using scripting_base::ObjectWrap.
   * 
   * This class wraps dom::MutationRecord objects for use in V8 JavaScript execution contexts.
   * It provides the standard DOM MutationRecord interface for representing individual DOM mutations.
   */
  class MutationRecord : public MutationRecordBase
  {
    using MutationRecordBase::ObjectWrap;

  public:
    /**
     * The name of the MutationRecord class for V8.
     */
    static std::string Name()
    {
      return "MutationRecord";
    }

    /**
     * Configure the V8 function template with MutationRecord properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 MutationRecord instance from a native dom::MutationRecord.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::MutationRecord> nativeRecord);

    /**
     * Initialize the MutationRecord class and register it with V8.
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

  public:
    MutationRecord(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    // Property getters
    static void TypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void TargetGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void AddedNodesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void RemovedNodesGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void PreviousSiblingGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void NextSiblingGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void AttributeNameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void AttributeNamespaceGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void OldValueGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  };
}