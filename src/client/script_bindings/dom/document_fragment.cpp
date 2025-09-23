#include "./document_fragment.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void DocumentFragment::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add properties
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "childElementCount").ToLocalChecked(),
                                  ChildElementCountGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "firstElementChild").ToLocalChecked(),
                                  FirstElementChildGetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "lastElementChild").ToLocalChecked(),
                                  LastElementChildGetter);

    // Add methods
    prototypeTemplate->Set(isolate, "querySelector", FunctionTemplate::New(isolate, QuerySelector));
    prototypeTemplate->Set(isolate, "querySelectorAll", FunctionTemplate::New(isolate, QuerySelectorAll));
    prototypeTemplate->Set(isolate, "getElementById", FunctionTemplate::New(isolate, GetElementById));
    prototypeTemplate->Set(isolate, "getElementsByTagName", FunctionTemplate::New(isolate, GetElementsByTagName));
    prototypeTemplate->Set(isolate, "getElementsByClassName", FunctionTemplate::New(isolate, GetElementsByClassName));
  }

  Local<Object> DocumentFragment::NewInstance(Isolate *isolate, shared_ptr<dom::DocumentFragment> native)
  {
    EscapableHandleScope scope(isolate);

    if (native == nullptr)
    {
      return scope.Escape(Local<Object>());
    }
    else
    {
      return scope.Escape(DocumentFragmentBase::NewInstance(isolate, native).As<Object>());
    }
  }

  Local<Function> DocumentFragment::Initialize(Isolate *isolate)
  {
    return DocumentFragment::ObjectWrap::Initialize(isolate);
  }

  DocumentFragment::DocumentFragment(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : DocumentFragmentBase(isolate, args)
  {
    // DocumentFragment constructor
  }

  // Method implementations
  void DocumentFragment::QuerySelector(const FunctionCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.querySelector called" << endl;
    // TODO: Implement querySelector functionality
    info.GetReturnValue().SetNull();
  }

  void DocumentFragment::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.querySelectorAll called" << endl;
    // TODO: Implement querySelectorAll functionality
    info.GetReturnValue().SetNull();
  }

  void DocumentFragment::GetElementById(const FunctionCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.getElementById called" << endl;
    // TODO: Implement getElementById functionality
    info.GetReturnValue().SetNull();
  }

  void DocumentFragment::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.getElementsByTagName called" << endl;
    // TODO: Implement getElementsByTagName functionality
    info.GetReturnValue().SetNull();
  }

  void DocumentFragment::GetElementsByClassName(const FunctionCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.getElementsByClassName called" << endl;
    // TODO: Implement getElementsByClassName functionality
    info.GetReturnValue().SetNull();
  }

  // Property getters
  void DocumentFragment::ChildElementCountGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.childElementCount getter called" << endl;
    // TODO: Return the actual child element count
    info.GetReturnValue().Set(0);
  }

  void DocumentFragment::FirstElementChildGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.firstElementChild getter called" << endl;
    // TODO: Return the first element child
    info.GetReturnValue().SetNull();
  }

  void DocumentFragment::LastElementChildGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.lastElementChild getter called" << endl;
    // TODO: Return the last element child
    info.GetReturnValue().SetNull();
  }
}