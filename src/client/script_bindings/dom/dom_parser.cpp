#include "./dom_parser.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  void DOMParser::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add methods
    prototypeTemplate->Set(isolate, "parseFromString",
                          FunctionTemplate::New(isolate, ParseFromString));
  }

  Local<Object> DOMParser::NewInstance(Isolate *isolate, shared_ptr<dom::DOMParser> nativeParser)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Function> constructor = DOMParser::GetConstructorFunction(isolate);
    Local<Object> instance = constructor->NewInstance(context, 0, nullptr).ToLocalChecked();

    DOMParser *wrapper = new DOMParser(isolate, *reinterpret_cast<const FunctionCallbackInfo<Value> *>(&instance));
    wrapper->SetNativeInstance(nativeParser);
    DOMParser::Wrap(isolate, instance, wrapper);

    return instance;
  }

  Local<Function> DOMParser::Initialize(Isolate *isolate)
  {
    return DOMParser::ObjectWrap::Initialize(isolate);
  }

  DOMParser::DOMParser(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
    : DOMParserBase(isolate, args)
  {
    // DOMParser constructor
  }

  // Method implementations
  void DOMParser::ParseFromString(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "DOMParser.parseFromString requires 2 arguments: string and mimeType").ToLocalChecked()));
      return;
    }

    cout << "DOMParser.parseFromString called" << endl;
    
    // TODO: Implement actual parsing logic
    // Arguments: str (string to parse), type (MIME type)
    // Should return a Document object
    
    info.GetReturnValue().SetNull();
  }
}