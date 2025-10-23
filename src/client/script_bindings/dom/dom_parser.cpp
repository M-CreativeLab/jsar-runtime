#include "./dom_parser.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void DOMParser::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Add methods
    prototypeTemplate->Set(isolate, "parseFromString", FunctionTemplate::New(isolate, ParseFromString));
  }

  Local<Object> DOMParser::NewInstance(Isolate *isolate, shared_ptr<dom::DOMParser> nativeParser)
  {
    EscapableHandleScope scope(isolate);

    if (nativeParser == nullptr)
    {
      return scope.Escape(Local<Object>());
    }
    else
    {
      return scope.Escape(DOMParserBase::NewInstance(isolate, nativeParser).As<Object>());
    }
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