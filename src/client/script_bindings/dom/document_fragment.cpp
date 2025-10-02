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
    InstanceReadonlyAccessor(isolate, instanceTemplate, "childElementCount", &DocumentFragment::ChildElementCountGetter);
    InstanceReadonlyAccessor(isolate, instanceTemplate, "firstElementChild", &DocumentFragment::FirstElementChildGetter);
    InstanceReadonlyAccessor(isolate, instanceTemplate, "lastElementChild", &DocumentFragment::LastElementChildGetter);

    // Add methods
    InstanceMethod(isolate, prototypeTemplate, "getElementById", &DocumentFragment::GetElementById);
    InstanceMethod(isolate, prototypeTemplate, "getElementsByTagName", &DocumentFragment::GetElementsByTagName);
    InstanceMethod(isolate, prototypeTemplate, "getElementsByClassName", &DocumentFragment::GetElementsByClassName);
    InstanceMethod(isolate, prototypeTemplate, "querySelector", &DocumentFragment::QuerySelector);
    InstanceMethod(isolate, prototypeTemplate, "querySelectorAll", &DocumentFragment::QuerySelectorAll);
  }

  DocumentFragment::DocumentFragment(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : DocumentFragmentBase(isolate, args)
  {
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
  void DocumentFragment::ChildElementCountGetter(const PropertyCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.childElementCount getter called" << endl;
    // TODO: Return the actual child element count
    info.GetReturnValue().Set(0);
  }

  void DocumentFragment::FirstElementChildGetter(const PropertyCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.firstElementChild getter called" << endl;
    // TODO: Return the first element child
    info.GetReturnValue().SetNull();
  }

  void DocumentFragment::LastElementChildGetter(const PropertyCallbackInfo<Value> &info)
  {
    cout << "DocumentFragment.lastElementChild getter called" << endl;
    // TODO: Return the last element child
    info.GetReturnValue().SetNull();
  }
}