#include "./dom_string_map.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void DOMStringMap::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    // Set up named property handlers for dynamic access to dataset attributes
    instanceTemplate->SetHandler(NamedPropertyHandlerConfiguration(
      NamedPropertyGetter,
      NamedPropertySetter,
      NamedPropertyQuery,
      NamedPropertyDeleter,
      NamedPropertyEnumerator));

    // Set up indexed property handlers
    instanceTemplate->SetHandler(IndexedPropertyHandlerConfiguration(
      IndexedPropertyGetter,
      IndexedPropertySetter,
      IndexedPropertyQuery,
      IndexedPropertyDeleter,
      IndexedPropertyEnumerator));
  }

  Local<Object> DOMStringMap::NewInstance(Isolate *isolate)
  {
    EscapableHandleScope scope(isolate);
    return scope.Escape(DOMStringMapBase::NewInstance(isolate, nullptr).As<Object>());
  }

  Local<Function> DOMStringMap::Initialize(Isolate *isolate)
  {
    return DOMStringMap::ObjectWrap::Initialize(isolate);
  }

  DOMStringMap::DOMStringMap(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : DOMStringMapBase(isolate, args)
  {
    // DOMStringMap constructor
  }

  // Indexed property handlers
  void DOMStringMap::IndexedPropertyGetter(uint32_t index, const PropertyCallbackInfo<Value> &info)
  {
    cout << "DOMStringMap indexed getter[" << index << "] called" << endl;
    // TODO: Implement indexed access
    info.GetReturnValue().SetUndefined();
  }

  void DOMStringMap::IndexedPropertySetter(uint32_t index, Local<Value> value, const PropertyCallbackInfo<Value> &info)
  {
    cout << "DOMStringMap indexed setter[" << index << "] called" << endl;
    // TODO: Implement indexed assignment
    info.GetReturnValue().Set(value);
  }

  void DOMStringMap::IndexedPropertyQuery(uint32_t index, const PropertyCallbackInfo<Integer> &info)
  {
    cout << "DOMStringMap indexed query[" << index << "] called" << endl;
    // TODO: Check if indexed property exists
    info.GetReturnValue().Set(Integer::New(info.GetIsolate(), None));
  }

  void DOMStringMap::IndexedPropertyDeleter(uint32_t index, const PropertyCallbackInfo<Boolean> &info)
  {
    cout << "DOMStringMap indexed deleter[" << index << "] called" << endl;
    // TODO: Implement indexed deletion
    info.GetReturnValue().Set(false);
  }

  void DOMStringMap::IndexedPropertyEnumerator(const PropertyCallbackInfo<Array> &info)
  {
    cout << "DOMStringMap indexed enumerator called" << endl;
    // TODO: Return array of indices
    info.GetReturnValue().Set(Array::New(info.GetIsolate()));
  }

  // Named property handlers
  void DOMStringMap::NamedPropertyGetter(Local<v8::Name> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    String::Utf8Value propertyName(isolate, property);
    cout << "DOMStringMap named getter[" << *propertyName << "] called" << endl;

    // TODO: Get dataset attribute value by name
    info.GetReturnValue().SetUndefined();
  }

  void DOMStringMap::NamedPropertySetter(Local<v8::Name> property, Local<Value> value, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    String::Utf8Value propertyName(isolate, property);
    cout << "DOMStringMap named setter[" << *propertyName << "] called" << endl;

    // TODO: Set dataset attribute value by name
    info.GetReturnValue().Set(value);
  }

  void DOMStringMap::NamedPropertyQuery(Local<v8::Name> property, const PropertyCallbackInfo<Integer> &info)
  {
    Isolate *isolate = info.GetIsolate();
    String::Utf8Value propertyName(isolate, property);
    cout << "DOMStringMap named query[" << *propertyName << "] called" << endl;

    // TODO: Check if named property exists
    info.GetReturnValue().Set(Integer::New(isolate, None));
  }

  void DOMStringMap::NamedPropertyDeleter(Local<v8::Name> property, const PropertyCallbackInfo<Boolean> &info)
  {
    Isolate *isolate = info.GetIsolate();
    String::Utf8Value propertyName(isolate, property);
    cout << "DOMStringMap named deleter[" << *propertyName << "] called" << endl;

    // TODO: Delete dataset attribute by name
    info.GetReturnValue().Set(false);
  }

  void DOMStringMap::NamedPropertyEnumerator(const PropertyCallbackInfo<Array> &info)
  {
    cout << "DOMStringMap named enumerator called" << endl;
    // TODO: Return array of property names
    info.GetReturnValue().Set(Array::New(info.GetIsolate()));
  }
}
