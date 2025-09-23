#include "./mutation_record.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  void MutationRecord::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    // Add read-only properties
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "type").ToLocalChecked(),
                                  TypeGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "target").ToLocalChecked(),
                                  TargetGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "addedNodes").ToLocalChecked(),
                                  AddedNodesGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "removedNodes").ToLocalChecked(),
                                  RemovedNodesGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "previousSibling").ToLocalChecked(),
                                  PreviousSiblingGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nextSibling").ToLocalChecked(),
                                  NextSiblingGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "attributeName").ToLocalChecked(),
                                  AttributeNameGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "attributeNamespace").ToLocalChecked(),
                                  AttributeNamespaceGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "oldValue").ToLocalChecked(),
                                  OldValueGetter);
  }

  Local<Object> MutationRecord::NewInstance(Isolate *isolate, shared_ptr<dom::MutationRecord> nativeRecord)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Function> constructor = MutationRecord::GetConstructorFunction(isolate);
    Local<Object> instance = constructor->NewInstance(context, 0, nullptr).ToLocalChecked();

    MutationRecord *wrapper = new MutationRecord(isolate, *reinterpret_cast<const FunctionCallbackInfo<Value> *>(&instance));
    wrapper->SetNativeInstance(nativeRecord);
    MutationRecord::Wrap(isolate, instance, wrapper);

    return instance;
  }

  Local<Function> MutationRecord::Initialize(Isolate *isolate)
  {
    return MutationRecord::ObjectWrap::Initialize(isolate);
  }

  MutationRecord::MutationRecord(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
    : MutationRecordBase(isolate, args)
  {
    // MutationRecord constructor
  }

  // Property getters
  void MutationRecord::TypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.type getter called" << endl;
    // TODO: Return the mutation type ("attributes", "childList", "characterData")
    info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "childList").ToLocalChecked());
  }

  void MutationRecord::TargetGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.target getter called" << endl;
    // TODO: Return the target node that was mutated
    info.GetReturnValue().SetNull();
  }

  void MutationRecord::AddedNodesGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.addedNodes getter called" << endl;
    // TODO: Return NodeList of added nodes
    info.GetReturnValue().Set(Array::New(info.GetIsolate()));
  }

  void MutationRecord::RemovedNodesGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.removedNodes getter called" << endl;
    // TODO: Return NodeList of removed nodes
    info.GetReturnValue().Set(Array::New(info.GetIsolate()));
  }

  void MutationRecord::PreviousSiblingGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.previousSibling getter called" << endl;
    // TODO: Return the previous sibling of added/removed nodes
    info.GetReturnValue().SetNull();
  }

  void MutationRecord::NextSiblingGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.nextSibling getter called" << endl;
    // TODO: Return the next sibling of added/removed nodes
    info.GetReturnValue().SetNull();
  }

  void MutationRecord::AttributeNameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.attributeName getter called" << endl;
    // TODO: Return the local name of the changed attribute
    info.GetReturnValue().SetNull();
  }

  void MutationRecord::AttributeNamespaceGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.attributeNamespace getter called" << endl;
    // TODO: Return the namespace of the changed attribute
    info.GetReturnValue().SetNull();
  }

  void MutationRecord::OldValueGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "MutationRecord.oldValue getter called" << endl;
    // TODO: Return the old value (for attributes and characterData)
    info.GetReturnValue().SetNull();
  }
}