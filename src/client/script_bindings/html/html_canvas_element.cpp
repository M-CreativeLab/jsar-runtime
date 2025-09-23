#include "./html_canvas_element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::html_bindings
{
  void HTMLCanvasElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
    Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

    // Canvas methods
    prototypeTemplate->Set(isolate, "getContext", FunctionTemplate::New(isolate, GetContext));
    prototypeTemplate->Set(isolate, "toDataURL", FunctionTemplate::New(isolate, ToDataURL));
    prototypeTemplate->Set(isolate, "toBlob", FunctionTemplate::New(isolate, ToBlob));

    // Canvas properties
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                  WidthGetter,
                                  WidthSetter);
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                  HeightGetter,
                                  HeightSetter);
  }

  Local<Object> HTMLCanvasElement::NewInstance(Isolate *isolate, shared_ptr<dom::HTMLCanvasElement> nativeElement)
  {
    EscapableHandleScope scope(isolate);
    if (nativeElement == nullptr)
    {
      return scope.Escape(Local<Object>());
    }
    else
    {
      return scope.Escape(HTMLCanvasElementBase::NewInstance(isolate, nativeElement).As<Object>());
    }
  }

  Local<Function> HTMLCanvasElement::Initialize(Isolate *isolate)
  {
    return HTMLCanvasElement::ObjectWrap::Initialize(isolate);
  }

  HTMLCanvasElement::HTMLCanvasElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLCanvasElementBase(isolate, args)
  {
    // HTMLCanvasElement constructor
    cout << "HTMLCanvasElement V8 wrapper created" << endl;
  }

  // Canvas methods
  void HTMLCanvasElement::GetContext(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "HTMLCanvasElement.getContext requires a context type").ToLocalChecked()));
      return;
    }

    cout << "HTMLCanvasElement.getContext() called" << endl;

    // TODO: Implement context creation
    // Context types: "2d", "webgl", "webgl2", "webgpu"
    // Should return the appropriate rendering context object

    info.GetReturnValue().SetNull();
  }

  void HTMLCanvasElement::ToDataURL(const FunctionCallbackInfo<Value> &info)
  {
    cout << "HTMLCanvasElement.toDataURL() called" << endl;

    // TODO: Implement canvas to data URL conversion
    // Optional arguments: type (string), quality (number)
    // Should return a data: URL containing the canvas content

    info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), "data:,").ToLocalChecked());
  }

  void HTMLCanvasElement::ToBlob(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();

    if (info.Length() < 1 || !info[0]->IsFunction())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "HTMLCanvasElement.toBlob requires a callback function").ToLocalChecked()));
      return;
    }

    cout << "HTMLCanvasElement.toBlob() called" << endl;

    // TODO: Implement canvas to blob conversion
    // Arguments: callback (function), type (string optional), quality (number optional)
    // Should call the callback with a Blob containing the canvas content

    info.GetReturnValue().SetUndefined();
  }

  // Property implementations
  void HTMLCanvasElement::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLCanvasElement.width getter called" << endl;
    // TODO: Return the canvas width
    info.GetReturnValue().Set(300); // Default width
  }

  void HTMLCanvasElement::WidthSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    cout << "HTMLCanvasElement.width setter called" << endl;
    // TODO: Set the canvas width and clear the canvas
  }

  void HTMLCanvasElement::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    cout << "HTMLCanvasElement.height getter called" << endl;
    // TODO: Return the canvas height
    info.GetReturnValue().Set(150); // Default height
  }

  void HTMLCanvasElement::HeightSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    cout << "HTMLCanvasElement.height setter called" << endl;
    // TODO: Set the canvas height and clear the canvas
  }
}