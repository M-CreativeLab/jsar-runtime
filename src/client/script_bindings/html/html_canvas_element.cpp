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

    // Canvas properties
    InstanceAccessor(isolate,
                     instanceTemplate,
                     "width",
                     &HTMLCanvasElement::WidthGetter,
                     &HTMLCanvasElement::WidthSetter);
    InstanceAccessor(isolate,
                     instanceTemplate,
                     "height",
                     &HTMLCanvasElement::HeightGetter,
                     &HTMLCanvasElement::HeightSetter);

    // Canvas methods
    InstanceMethod(isolate, prototypeTemplate, "getContext", &HTMLCanvasElement::GetContext);
    InstanceMethod(isolate, prototypeTemplate, "toDataURL", &HTMLCanvasElement::ToDataURL);
    InstanceMethod(isolate, prototypeTemplate, "toBlob", &HTMLCanvasElement::ToBlob);
  }

  HTMLCanvasElement::HTMLCanvasElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLCanvasElementBase(isolate, args, true)
  {
  }

  void HTMLCanvasElement::WidthGetter(const PropertyCallbackInfo<Value> &info)
  {
    HandleScope scope(info.GetIsolate());
    int width = handle()->width();
    info.GetReturnValue().Set(Integer::New(info.GetIsolate(), width));
  }

  void HTMLCanvasElement::WidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "set", "width must be a number")));
      return;
    }

    int width = value->Int32Value(context).FromMaybe(0);
    handle()->setWidth(width);
  }

  void HTMLCanvasElement::HeightGetter(const PropertyCallbackInfo<Value> &info)
  {
    HandleScope scope(info.GetIsolate());
    int height = handle()->height();
    info.GetReturnValue().Set(Integer::New(info.GetIsolate(), height));
  }

  void HTMLCanvasElement::HeightSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "set", "height must be a number")));
      return;
    }

    int height = value->Int32Value(context).FromMaybe(0);
    handle()->setHeight(height);
  }

  // Canvas methods
  void HTMLCanvasElement::GetContext(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "getContext", "requires at least 1 argument")));
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
}
