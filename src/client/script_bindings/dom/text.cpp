#include "./text.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  // static
  void Text::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);

    // Set up the instance template
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add property accessors (data and length inherited from CharacterData)
    prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "wholeText").ToLocalChecked(),
                                   WholeTextGetter,
                                   nullptr,
                                   Local<Value>(),
                                   AccessControl::DEFAULT,
                                   PropertyAttribute::ReadOnly);

    // Add methods (inherited methods from CharacterData)
    prototypeTemplate->Set(String::NewFromUtf8(isolate, "splitText").ToLocalChecked(),
                           FunctionTemplate::New(isolate, SplitText));
  }

  // static
  Local<Object> Text::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Text> nativeText)
  {
    EscapableHandleScope scope(isolate);
    assert(nativeText != nullptr && "Text instance is null.");
    return scope.Escape(TextBase::NewInstance(isolate, nativeText).As<Object>());
  }

  Text::Text(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : TextBase(isolate, args)
  {
  }

  // Property getters and setters

  // static
  void Text::WholeTextGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Text *text = Unwrap(isolate, info.This());
    if (text == nullptr || text->handle() == nullptr)
    {
      info.GetReturnValue().SetEmptyString();
      return;
    }

    // For now, return the same as data. In a full implementation, this would
    // concatenate adjacent text nodes
    string wholeText = text->handle()->data();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, wholeText.c_str()).ToLocalChecked());
  }

  // Methods

  // static
  void Text::SplitText(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Text *text = Unwrap(isolate, info.This());
    if (text == nullptr || text->handle() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    // TODO: Implement splitText - should create a new Text node
    cout << "splitText called" << endl;
    info.GetReturnValue().SetNull();
  }
}
