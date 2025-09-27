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

    if (nativeText == nullptr)
    {
      return scope.Escape(Local<Object>());
    }

    return scope.Escape(scripting_base::ObjectWrap<Text, ::dom::Text, CharacterData>::NewInstance(isolate, nativeText).As<Object>());
  }

  // static
  Local<Function> Text::Initialize(Isolate *isolate)
  {
    return scripting_base::ObjectWrap<Text, ::dom::Text, CharacterData>::Initialize(isolate);
  }

  Text::Text(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : scripting_base::ObjectWrap<Text, ::dom::Text, CharacterData>(isolate, args)
  {
  }

  // Property getters and setters

  // static
  void Text::WholeTextGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, CharacterData>::Unwrap(info.This());
    if (text == nullptr || text->inner() == nullptr)
    {
      info.GetReturnValue().SetEmptyString();
      return;
    }

    // For now, return the same as data. In a full implementation, this would
    // concatenate adjacent text nodes
    string wholeText = text->inner()->data();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, wholeText.c_str()).ToLocalChecked());
  }

  // Methods

  // static
  void Text::SplitText(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Text *text = scripting_base::ObjectWrap<Text, ::dom::Text, CharacterData>::Unwrap(info.This());
    if (text == nullptr || text->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    // TODO: Implement splitText - should create a new Text node
    cout << "splitText called" << endl;
    info.GetReturnValue().SetNull();
  }
}
